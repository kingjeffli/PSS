// MessageService.h
// ������Ϣ������Ϣ���ɸ�������߼�������ȥִ��
// ���쵽�˹���ͼ��ݣ��о��¹���ĺ����ɣ�������д���������˼��
// ��ʱ���������ǵ�֧���£�PSS�Ż��ߵĸ�Զ��
// ���ϵ��кý���������ɼ���Ҷ�������ʹ���С�
// ��ʱ��ƽ������������
// add by freeeyes
// 2009-01-29

#include "MessageManager.h"
#if PSS_PLATFORM == PLATFORM_WIN
#include "ProConnectHandler.h"
#else
#include "ConnectHandler.h"
#endif

bool Delete_CommandInfo(_ClientCommandInfo* pClientCommandInfo)
{
    return pClientCommandInfo->m_u4CurrUsedCount == 0;
}

CMessageManager::CMessageManager(void)
{
    m_u2MaxModuleCount     = 0;
    m_u4MaxCommandCount    = 0;
    m_u4CurrCommandCount   = 0;
    m_u4UpdateIndex        = 0;
}

CMessageManager::~CMessageManager(void)
{
    OUR_DEBUG((LM_INFO, "[CMessageManager::~CMessageManager].\n"));
}

void CMessageManager::Init(uint16 u2MaxModuleCount, uint32 u4MaxCommandCount)
{
    //��ʼ����������
    m_objClientCommandList.Init((int)u4MaxCommandCount);

    //��ʼ��HashTable
    m_objModuleClientList.Init((int)u2MaxModuleCount);

    m_u2MaxModuleCount  = u2MaxModuleCount;
    m_u4MaxCommandCount = u4MaxCommandCount;
    m_u4UpdateIndex     = 0;
}

bool CMessageManager::AddClientCommand(uint16 u2CommandID, CClientCommand* pClientCommand, const char* pModuleName)
{
    return AddClientCommand_Ex(u2CommandID, pClientCommand, pModuleName, NULL);
}

bool CMessageManager::AddClientCommand(uint16 u2CommandID, CClientCommand* pClientCommand, const char* pModuleName, _ClientIPInfo objListenInfo)
{
    return AddClientCommand_Ex(u2CommandID, pClientCommand, pModuleName, &objListenInfo);
}

bool CMessageManager::UnloadModuleCommand(const char* pModuleName, uint8 u1LoadState, uint32 u4ThreadCount)
{
    string strModuleName  = pModuleName;
    string strModuleN     = "";
    string strModulePath  = "";
    string strModuleParam = "";
    uint32 u4TmpUpdateIndex = 0;

    //������ض�����ز���
    _ModuleInfo* pModuleInfo = App_ModuleLoader::instance()->GetModuleInfo(pModuleName);

    if(NULL != pModuleInfo)
    {
        //��ȡ������Ϣ
        strModuleN     = pModuleInfo->strModuleName;
        strModulePath  = pModuleInfo->strModulePath;
        strModuleParam = pModuleInfo->strModuleParam;
    }

    _ModuleClient* pModuleClient = m_objModuleClientList.Get_Hash_Box_Data(strModuleName.c_str());

    if(NULL != pModuleClient)
    {
        //�Ӳ��Ŀǰע������������ҵ����иò������Ϣ��һ�����ͷ�
        for(uint32 u4Index = 0; u4Index < (uint32)pModuleClient->m_vecClientCommandInfo.size(); u4Index++)
        {
            //�������б���ɾ��ָ�������ӳ���ϵ
            DeleteCommandByModule(pModuleClient->m_vecClientCommandInfo[u4Index]);
        }

        u4TmpUpdateIndex++;

        //ж�ز����Ϣ(��������ж�أ��ڶ�ʱ��������й����̶߳��������ˣ���ж��ָ����ģ��,�Ƚ���Ҫж�ص�ģ�������Ҫж�صĶ���)
        if (false == App_ModuleLoader::instance()->MoveUnloadList(pModuleName, u4TmpUpdateIndex, u4ThreadCount, u1LoadState, strModulePath, strModuleN, strModuleParam))
        {
            OUR_DEBUG((LM_ERROR, "[CMessageManager::UnloadModuleCommand]MoveUnloadList error(%s).\n", pModuleName));
        }
    }

    m_u4UpdateIndex = u4TmpUpdateIndex;
    return true;
}

int CMessageManager::GetCommandCount()
{
    return (int)m_u4CurrCommandCount;
}

void CMessageManager::Close()
{
    //��رյ���������
    vector<CClientCommandList*> vecClientCommandList;
    m_objClientCommandList.Get_All_Used(vecClientCommandList);

    uint32 u4Size = (uint32)vecClientCommandList.size();

    for (uint32 i = 0; i < u4Size; i++)
    {
        SAFE_DELETE(vecClientCommandList[i]);
    }

    m_objClientCommandList.Close();

    vector<_ModuleClient*> vecModuleClient;
    m_objModuleClientList.Get_All_Used(vecModuleClient);

    u4Size = (uint32)vecModuleClient.size();

    for (uint32 i = 0; i < u4Size; i++)
    {
        SAFE_DELETE(vecModuleClient[i]);
    }

    m_objModuleClientList.Close();

    m_u2MaxModuleCount  = 0;
    m_u4MaxCommandCount = 0;

}

CHashTable<_ModuleClient>* CMessageManager::GetModuleClient()
{
    return &m_objModuleClientList;
}

uint32 CMessageManager::GetWorkThreadCount()
{
    return App_MessageServiceGroup::instance()->GetWorkThreadCount();
}

uint32 CMessageManager::GetWorkThreadByIndex(uint32 u4Index)
{
    return App_MessageServiceGroup::instance()->GetWorkThreadIDByIndex(u4Index);
}

NAMESPACE::uint16 CMessageManager::GetMaxCommandCount()
{
    return m_u4MaxCommandCount;
}

uint32 CMessageManager::GetUpdateIndex()
{
    return m_u4UpdateIndex;
}

CHashTable<CClientCommandList>* CMessageManager::GetHashCommandList()
{
    return &m_objClientCommandList;
}

bool CMessageManager::AddClientCommand_Ex(uint16 u2CommandID, CClientCommand* pClientCommand, const char* pModuleName, _ClientIPInfo* pListenInfo)
{
    if (NULL == pClientCommand)
    {
        OUR_DEBUG((LM_ERROR, "[CMessageManager::AddClientCommand_Ex] u2CommandID = %d pClientCommand is NULL.\n", u2CommandID));
        return false;
    }

    //�������ļ���ȡTimeout����
    xmlCommandsTimeout::_CommandsTimeout* pCommandTimeout = GetXmlConfigAttribute(xmlCommandsTimeout)->GetCommandAlert(u2CommandID);

    CClientCommandList* pClientCommandList = GetClientCommandExist(u2CommandID);

    if (NULL != pClientCommandList)
    {
        //�����ʱʱ�䲻Ϊ�գ�����Ϊ��ʱʱ��
        if (NULL != pCommandTimeout)
        {
            pClientCommandList->SetCommandTimeout(pCommandTimeout->Timeout);
        }

        //�������Ѵ���
        _ClientCommandInfo* pClientCommandInfo = pClientCommandList->AddClientCommand(pClientCommand, pModuleName, pListenInfo);
        //���������ID
        pClientCommandInfo->m_u2CommandID = u2CommandID;

        //���ӵ�ģ������
        string strModule = pModuleName;
        _ModuleClient* pModuleClient = m_objModuleClientList.Get_Hash_Box_Data(strModule.c_str());

        if (NULL == pModuleClient)
        {
            //�Ҳ����������µ�ģ����Ϣ
            pModuleClient = new _ModuleClient();

            pModuleClient->m_vecClientCommandInfo.push_back(pClientCommandInfo);
            m_objModuleClientList.Add_Hash_Data(strModule.c_str(), pModuleClient);
        }
        else
        {
            //�ҵ��ˣ����ӽ�ȥ
            pModuleClient->m_vecClientCommandInfo.push_back(pClientCommandInfo);
        }

        OUR_DEBUG((LM_ERROR, "[CMessageManager::AddClientCommand_Ex] u2CommandID = %d Add OK***.\n", u2CommandID));
    }
    else
    {
        //��������δ����
        pClientCommandList = new CClientCommandList(u2CommandID);

        _ClientCommandInfo* pClientCommandInfo = pClientCommandList->AddClientCommand(pClientCommand, pModuleName, pListenInfo);
        //���������ID
        pClientCommandInfo->m_u2CommandID = u2CommandID;

        //���ӵ�ģ������
        string strModule = pModuleName;
        _ModuleClient* pModuleClient = m_objModuleClientList.Get_Hash_Box_Data(strModule.c_str());

        if (NULL == pModuleClient)
        {
            //�Ҳ����������µ�ģ����Ϣ
            pModuleClient = new _ModuleClient();
            pModuleClient->m_vecClientCommandInfo.push_back(pClientCommandInfo);
            m_objModuleClientList.Add_Hash_Data(strModule.c_str(), pModuleClient);
        }
        else
        {
            //�ҵ��ˣ����ӽ�ȥ
            pModuleClient->m_vecClientCommandInfo.push_back(pClientCommandInfo);
        }

        char szCommandID[10] = { '\0' };
        sprintf_safe(szCommandID, 10, "%d", u2CommandID);
        m_objClientCommandList.Add_Hash_Data(szCommandID, pClientCommandList);
        m_u4CurrCommandCount++;
        OUR_DEBUG((LM_ERROR, "[CMessageManager::AddClientCommand_Ex]AddClientCommand u2CommandID = %d Add OK***.\n", u2CommandID));
    }

    return true;
}

void CMessageManager::DeleteCommandByModule(_ClientCommandInfo* pClientCommandInfo)
{
    uint16 u2CommandID = pClientCommandInfo->m_u2CommandID;
    CClientCommandList* pClientCommandList = GetClientCommandExist(u2CommandID);

    if (NULL == pClientCommandList)
    {
        return;
    }

    for (int i = 0; i < pClientCommandList->GetCount(); i++)
    {
        //�ҵ��Ǹ�Ψһ
        if (pClientCommandList->GetClientCommandIndex(i) == pClientCommandInfo)
        {
            //�ҵ��ˣ��ͷ�֮
            if (false == pClientCommandList->DelClientCommand(pClientCommandInfo->m_pClientCommand))
            {
                OUR_DEBUG((LM_INFO, "[CMessageManager::UnloadModuleCommand]DelClientCommand(%d) is fail.\n", pClientCommandInfo->m_u2CommandID));
            }
            else
            {
                OUR_DEBUG((LM_INFO, "[CMessageManager::UnloadModuleCommand]DelClientCommand(%d) is OK.\n", pClientCommandInfo->m_u2CommandID));
            }

            //�����ָ���µ������Ѿ������ڣ���ɾ��֮
            if (pClientCommandList->GetCount() == 0)
            {
                SAFE_DELETE(pClientCommandList);
                char szCommandID[10] = { '\0' };
                sprintf_safe(szCommandID, 10, "%d", u2CommandID);
                m_objClientCommandList.Del_Hash_Data(szCommandID);
                m_u4CurrCommandCount--;
            }

            break;
        }
    }
}

bool CMessageManager::DelClientCommand(uint16 u2CommandID, CClientCommand* pClientCommand)
{
    char szCommandID[10] = { '\0' };
    sprintf_safe(szCommandID, 10, "%d", u2CommandID);

    CClientCommandList* pClientCommandList = GetClientCommandExist(u2CommandID);

    if (NULL != pClientCommandList)
    {
        if (true == pClientCommandList->DelClientCommand(pClientCommand))
        {
            SAFE_DELETE(pClientCommandList);
            m_objClientCommandList.Del_Hash_Data(szCommandID);
            OUR_DEBUG((LM_ERROR, "[CMessageManager::DelClientCommand] u2CommandID = %d List Del OK.\n", u2CommandID));
        }
        else
        {
            OUR_DEBUG((LM_ERROR, "[CMessageManager::DelClientCommand] u2CommandID = %d Del Command OK.\n", u2CommandID));
        }

        return true;
    }
    else
    {
        OUR_DEBUG((LM_ERROR, "[CMessageManager::DelClientCommand] u2CommandID = %d is not exist.\n", u2CommandID));
        return false;
    }
}

CClientCommandList* CMessageManager::GetClientCommandExist(uint16 u2CommandID)
{
    char szCommandID[10] = { '\0' };
    sprintf_safe(szCommandID, 10, "%d", u2CommandID);
    CClientCommandList* pClientCommandList = m_objClientCommandList.Get_Hash_Box_Data(szCommandID);

    return pClientCommandList;
}
