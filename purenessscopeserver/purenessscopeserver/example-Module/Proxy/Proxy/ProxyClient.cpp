#include "ProxyClient.h"

CProxyClient::CProxyClient( void )
{
    m_nIOCount       = 0;
    m_u4RecvAllSize  = 0;
    m_u4SendAllSize  = 0;
    m_u4ConnectID    = 0;
    m_u4WorkThreadID = 0;
    m_pServerObject  = NULL;
}

CProxyClient::~CProxyClient( void )
{
}

bool CProxyClient::Close()
{
    m_ThreadLock.acquire();

    if(m_nIOCount > 0)
    {
        m_nIOCount--;
    }

    m_ThreadLock.release();

    //�ӷ�Ӧ��ע���¼�
    if(m_nIOCount == 0)
    {
        //msg_queue()->deactivate();
        shutdown();
        OUR_DEBUG((LM_ERROR, "[CProxyClient::Close]Close(%s:%d) OK.\n", m_addrRemote.get_host_addr(), m_addrRemote.get_port_number()));

        //��ÿͻ���ID
        uint32 u4ConnectID = App_ProxyThreadManager::instance()->GetProxyClientManager(m_u4WorkThreadID)->FindConnectID(this);

        //ɾ��ӳ���ϵ
        App_ProxyThreadManager::instance()->GetProxyClientManager(m_u4WorkThreadID)->DeleteByProxyClient(this);

        //�ر�Զ�̺Ϳͻ��˵�����
        if(NULL != m_pServerObject)
        {
            if(0 != u4ConnectID)
            {
                m_pServerObject->GetConnectManager()->CloseConnect(u4ConnectID);
            }
        }

        //�ع��ù���ָ��
        delete this;
        return true;
    }

    return false;
}

int CProxyClient::open( void* )
{
    //��������Ϊ������ģʽ
    if (this->peer().enable(ACE_NONBLOCK) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CProxyClient::open]this->peer().enable  = ACE_NONBLOCK error.\n"));
        return -1;
    }

    //���Զ�����ӵ�ַ�Ͷ˿�
    if(this->peer().get_remote_addr(m_addrRemote) == -1)
    {
        OUR_DEBUG((LM_ERROR, "[CProxyClient::open]this->peer().get_remote_addr error.\n"));
        return -1;
    }

    m_nIOCount = 1;

    //����ӳ���ϵ
    App_ProxyThreadManager::instance()->GetProxyClientManager(m_u4WorkThreadID)->Insert(m_u4ConnectID, this);

    //ע����¼�
    this->reactor()->register_handler(this,  ACE_Event_Handler::READ_MASK);

    OUR_DEBUG((LM_ERROR, "[CProxyClient::open]open OK.\n"));

    return 0;

}

int CProxyClient::handle_input( ACE_HANDLE fd /*= ACE_INVALID_HANDLE*/ )
{
    ACE_Time_Value     nowait(MAX_MSG_PACKETTIMEOUT);

    if(fd == ACE_INVALID_HANDLE)
    {
        OUR_DEBUG((LM_ERROR, "[CProxyClient::handle_input]ACE_INVALID_HANDLE.\n"));
        return false;
    }

    //���յ�Զ�̵������������ݣ��������ݷ��ظ��ͻ���
    char szClientBuff[MAX_RECV_BUFF] = {'\0'};

    int nDataLen = (int)this->peer().recv(szClientBuff, MAX_RECV_BUFF, MSG_NOSIGNAL, &nowait);

    if(nDataLen <= 0)
    {
        uint32 u4Error = (uint32)errno;
        OUR_DEBUG((LM_ERROR, "[CProxyClient::handle_input] ConnectID = %d, recv data is error nDataLen = [%d] errno = [%d].\n", m_u4ConnectID, nDataLen, u4Error));
        return -1;
    }
    else
    {
        //OUR_DEBUG((LM_ERROR, "[CProxyClient::handle_input] ConnectID = %d, nDataLen=%d.\n", m_u4ConnectID, nDataLen));
        //������յ������ݷ��͸��ͻ���
        if(NULL != m_pServerObject)
        {
            const char* ptrReturnData = reinterpret_cast<const char*>(szClientBuff);
            m_pServerObject->GetConnectManager()->PostMessage(m_u4ConnectID, ptrReturnData, (uint32)nDataLen, SENDMESSAGE_JAMPNOMAL,
                    (uint16)COMMAND_RETURN_PROXY, PACKET_SEND_IMMEDIATLY, false);
        }
    }

    //�����¼���յ����ֽ���
    m_u4RecvAllSize += nDataLen;

    return 0;
}

int CProxyClient::handle_close( ACE_HANDLE h, ACE_Reactor_Mask mask )
{
    if(h == ACE_INVALID_HANDLE)
    {
        OUR_DEBUG((LM_DEBUG,"[CProxyClient::handle_close] h is NULL mask=%d.\n", m_u4ConnectID, (int)mask));
    }

    Close();
    return 0;
}

bool CProxyClient::SendData(char* pData, int nLen)
{
    ACE_Time_Value     nowait(MAX_MSG_PACKETTIMEOUT);

    int nSendLen = nLen;   //�������ݵ��ܳ���
    int nIsSendSize = 0;

    //ѭ�����ͣ�ֱ�����ݷ�����ɡ�
    while(true)
    {
        if(nSendLen <= 0)
        {
            OUR_DEBUG((LM_ERROR, "[CProxyClient::SendData] ConnectID = %d, nCurrSendSize error is %d.\n", m_u4ConnectID, nSendLen));
            return false;
        }

        int nDataLen = (int)this->peer().send(&pData[nIsSendSize], nSendLen - nIsSendSize, &nowait);
        int nErr = ACE_OS::last_error();

        if(nDataLen <= 0)
        {
            if(nErr == EWOULDBLOCK)
            {
                //������Ͷ��������10������ٷ��͡�
                ACE_Time_Value tvSleep(0, 10 * MAX_BUFF_1000);
                ACE_OS::sleep(tvSleep);
                continue;
            }

            OUR_DEBUG((LM_ERROR, "[CProxyClient::SendData] ConnectID = %d, error = %d.\n", m_u4ConnectID, errno));
            Close();
            return false;
        }
        else if(nDataLen + nIsSendSize >= nSendLen)   //�����ݰ�ȫ��������ϣ���ա�
        {
            //OUR_DEBUG((LM_ERROR, "[CConnectHandler::handle_output] ConnectID = %d, send (%d) OK.\n", GetConnectID(), msg_queue()->is_empty()));
            m_u4SendAllSize += (uint32)nSendLen;
            return true;
        }
        else
        {
            nIsSendSize      += nDataLen;
            continue;
        }
    }

    return true;
}

void CProxyClient::SetServerObject(uint32 u4ConnectID, CServerObject* pServerObject)
{
    m_u4ConnectID   = u4ConnectID;
    m_pServerObject = pServerObject;
}

void CProxyClient::SetWorkThreadID(uint32 u4WorkThreadID)
{
    m_u4WorkThreadID = u4WorkThreadID;
}

uint32 CProxyClient::GetWorkThreadID()
{
    return m_u4WorkThreadID;
}

//******************************************************************

void* Worker(void* arg)
{
    _ProxyClientConnector* pProxyClientConnector = (_ProxyClientConnector* )arg;

    ACE_Reactor* pReactor = NULL;

#ifndef __LINUX__
    OUR_DEBUG((LM_ERROR, "[Worker]***__WINDOWS__****.\n"));
    ACE_Select_Reactor* selectreactor = new ACE_Select_Reactor();
    pReactor = new ACE_Reactor(selectreactor, 1);
    //g_pReactor = ACE_Reactor::instance();
#else
    OUR_DEBUG((LM_ERROR, "[Worker]***__LINUX__****.\n"));
    //�����Linux����������ΪĬ��ʹ��epoll������ļ��������������Ҫ������Ҫ����һ��
    //ĿǰĬ�ϵ���1000
    //�ļ��򿪾����ֵΪ����ǰ��ֵ+ǰ�������򿪵����������+10
    ACE_Dev_Poll_Reactor* devreactor = new ACE_Dev_Poll_Reactor(1000);
    pReactor = new ACE_Reactor(devreactor, 1);
#endif

    pProxyClientConnector->m_objReactorConnect.open(pReactor);
    pProxyClientConnector->m_pReactor    = pReactor;
    pProxyClientConnector->m_blThreadRun = true;

    while(pProxyClientConnector->m_blThreadRun)
    {
        if(pReactor != NULL)
        {
#ifndef __LINUX__
            pReactor->handle_events();
#else
            pReactor->run_reactor_event_loop();
#endif
        }
    }

    OUR_DEBUG((LM_ERROR, "[Worker]Thread is over.\n"));
    return NULL;
}


void CProxyClientThreadManager::Init( int nThreadCount )
{
    //���չ����̳߳�ʼ��ָ��������CProxyClientManager
    for(int i = 0; i < nThreadCount; i++)
    {
        _ProxyClientConnector* pProxyClientConnector = new _ProxyClientConnector();
        pProxyClientConnector->m_ProxyClientManager = new CProxyClientManager();
        m_vecProxyClientManager.push_back(pProxyClientConnector);

        //������Ӧ���߳�
        ACE_thread_t threadId;
        ACE_hthread_t threadHandle;

        ACE_Thread::spawn(
            (ACE_THR_FUNC)Worker,        //�߳�ִ�к���
            (_ProxyClientConnector* )pProxyClientConnector,   //ִ�к�������
            THR_JOINABLE | THR_NEW_LWP,
            &threadId,
            &threadHandle
        );
    }
}