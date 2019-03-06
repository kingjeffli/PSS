#include "FileLogger.h"

CLogFile::CLogFile(const char* pFileRoot, uint32 u4BufferSize, uint32 u4FileMaxSize)
    : m_u4BufferSize(u4BufferSize)
{
    m_u2CurrFileIndex = 1;
    m_u2LogID = 0;
    m_nDisplay = 0;
    m_u4CurrFileSize = 0;
    m_StrServerName = "";
    m_u2Level = 0;
    m_StrlogType = "ServerError";
    m_pBuffer = new char[u4BufferSize];   //������������־ƴ��ʱ������
    m_u4FileMaxSize = u4FileMaxSize * MAX_BUFF_1024 * MAX_BUFF_1024;
    m_szLogTime[0] = '\0';
    sprintf_safe(m_szFileRoot, MAX_BUFF_100, "%s", pFileRoot);
}

CLogFile::~CLogFile()
{
    OUR_DEBUG((LM_INFO, "[CLogFile::~CLogFile].\n"));
    SAFE_DELETE_ARRAY(m_pBuffer);
    m_u4BufferSize = 0;
    m_File.close();
    OUR_DEBUG((LM_INFO, "[CLogFile::~CLogFile] End.\n"));
}

void CLogFile::Init()
{
    //�������ʼ����ȡ��ǰ�ļ��е��ļ������ź��ļ���С
    ACE_Date_Time dt;
    char szDateBuff[MAX_TIME_SIZE] = { '\0' };

    //�����ж��ļ��Ƿ����
    while (true)
    {
        sprintf_safe(szDateBuff, MAX_TIME_SIZE, "_%04d-%02d-%02d_%d.log", dt.year(), dt.month(), dt.day(), m_u2CurrFileIndex);
        ACE_TString strLogModulePath = m_szFileRoot;
        ACE_TString strLogName = strLogModulePath + "/Log/" + m_StrlogType + "/" + m_StrlogName + "/" + m_StrServerName + m_StrlogName + szDateBuff;

        int nRet = ACE_OS::access(strLogName.c_str(), R_OK);

        if (0 == nRet)
        {
            //����ļ��Ѵ��ڣ��ж��ļ������Ƿ񳬹���ֵ
            FILE* fp = ACE_OS::fopen(strLogName.c_str(), "r");

            if (NULL == fp)
            {
                OUR_DEBUG((LM_INFO, "[CLogFile::Init]File(%s) fopen error.\n", strLogName.c_str()));
                return;
            }

            fseek(fp, 0L, SEEK_END);
            uint32 u4FileSize = ftell(fp);
            fclose(fp);

            if (u4FileSize >= m_u4FileMaxSize)
            {
                m_u2CurrFileIndex++;
            }
            else
            {
                m_u4CurrFileSize = u4FileSize;
                return;
            }
        }
        else
        {
            int nError = errno;

            if (EACCES == nError || ENOENT == nError)
            {
                //�ҵ��˲����ڵ��ļ�
                return;
            }
            else
            {
                OUR_DEBUG((LM_INFO, "[CLogFile::Init]File(%s) access error(%d).\n", strLogName.c_str(), nError));
                return;
            }
        }
    }
}

void CLogFile::SetFileRoot(const char* pFileRoot)
{
    sprintf_safe(m_szFileRoot, MAX_BUFF_100, "%s", pFileRoot);
}

char* CLogFile::GetFileRoot()
{
    return m_szFileRoot;
}

void CLogFile::SetFileAddr(ACE_FILE_Addr& objFileAddr)
{
    m_FileAddr = objFileAddr;
}

ACE_FILE_Addr& CLogFile::GetFileAddr()
{
    return m_FileAddr;
}

void CLogFile::SetConnector(ACE_FILE_Connector& objConnector)
{
    m_Connector = objConnector;
}

ACE_FILE_Connector& CLogFile::GetConnector()
{
    return m_Connector;
}

void CLogFile::SetFileIO(ACE_FILE_IO& objFile)
{
    m_File = objFile;
}

ACE_FILE_IO& CLogFile::GetFileIO()
{
    return m_File;
}

void CLogFile::SetLogTime(const char* pLogTime)
{
    sprintf_safe(m_szLogTime, MAX_TIME_SIZE, "%s", pLogTime);
}

char* CLogFile::GetLogTime()
{
    return m_szLogTime;
}

void CLogFile::SetBufferSize(uint32 u4BufferSize)
{
    SAFE_DELETE_ARRAY(m_pBuffer);
    m_pBuffer = new char[u4BufferSize];   //������������־ƴ��ʱ������
    m_u4BufferSize = u4BufferSize;
}

void CLogFile::SetFileMaxSize(uint32 u4FileMaxSize)
{
    m_u4FileMaxSize = u4FileMaxSize;
}

void CLogFile::SetCurrFileIndex(uint16 u2CurrFileIndex)
{
    m_u2CurrFileIndex = u2CurrFileIndex;
}

void CLogFile::SetCurrFileSize(uint32 u4CirrFileSize)
{
    m_u4CurrFileSize = u4CirrFileSize;
}

uint32 CLogFile::GetBufferSize()
{
    return m_u4BufferSize;
}

uint32 CLogFile::GetFileMaxSize()
{
    return m_u4FileMaxSize;
}

uint16 CLogFile::GetCurrFileIndex()
{
    return m_u2CurrFileIndex;
}

uint32 CLogFile::GetCurrFileSize()
{
    return m_u4CurrFileSize;
}

int CLogFile::doLog(_LogBlockInfo* pLogBlockInfo)
{
    //ÿ���Զ����
    CheckTime();

    //��¼��ǰ�ļ�д�볤��
    m_u4CurrFileSize += pLogBlockInfo->m_u4Length;

    ACE_Date_Time dt;
    char szDateBuff[MAX_TIME_SIZE] = { '\0' };

    sprintf_safe(szDateBuff, MAX_TIME_SIZE, "%04d-%02d-%02d %02d:%02d:%02d%02d,", dt.year(), dt.month(), dt.day(), dt.hour(), dt.minute(), dt.second(), dt.microsec() / 10000);

    //ƴ��ʵ�ʵ���־�ַ���
    sprintf_safe(m_pBuffer, m_u4BufferSize, "%s %s\n", szDateBuff, pLogBlockInfo->m_pBlock);

    if (m_nDisplay == 0)
    {
        //������־
        int nLen = (int)m_File.send(m_pBuffer, strlen(m_pBuffer));

        if (nLen != (int)strlen(m_pBuffer))
        {
            OUR_DEBUG((LM_INFO, "[%s]Write error[%s].\n", m_StrlogName.c_str(), m_pBuffer));
        }
    }
    else
    {
        //�������Ļ
        OUR_DEBUG((LM_INFO, "%s.\n", m_pBuffer));
    }

    //�鿴�Ƿ�Ҫ�����ʼ�
    if (pLogBlockInfo->m_u4MailID > 0 && false == SendMail(pLogBlockInfo))
    {
        OUR_DEBUG((LM_INFO, "[CLogFile::doLog](%s)Send mail fail.\n", m_StrlogName.c_str()));
    }

    //����Ƿ񳬹����ļ��飬��������ˣ�����һ������־�ļ���
    CheckLogFileBlock();

    return 0;
}

bool CLogFile::SendMail(_LogBlockInfo* pLogBlockInfo, xmlMails::_Mail* pMailInfo)
{
    //�����ʼ�
    xmlMails::_Mail* pMailAlert = NULL;

    //���û�е������ã���������ļ���ֱ�ӻ��
    if(NULL == pMailInfo)
    {
        pMailAlert = GetXmlConfigAttribute(xmlMails)->GetMailAlert(pLogBlockInfo->m_u4MailID);
    }
    else
    {
        pMailAlert = pMailInfo;
    }

    if (NULL == pMailAlert)
    {
        OUR_DEBUG((LM_ERROR, "[CLogFile::SendMail]MailID(%d) is no find.\n", pLogBlockInfo->m_u4MailID));
        return false;
    }

    char szMailURL[MAX_BUFF_100] = { '\0' };

    sprintf_safe(szMailURL, MAX_BUFF_100, "%s:%d", pMailAlert->MailUrl.c_str(), pMailAlert->MailPort);

    //����smtps�ʼ�
    int nRet = Send_Mail_From_Ssl(pMailAlert->fromMailAddr.c_str(),
                                  pMailAlert->MailPass.c_str(),
                                  pMailAlert->fromMailAddr.c_str(),
                                  pMailAlert->toMailAddr.c_str(),
                                  szMailURL,
                                  pLogBlockInfo->m_szMailTitle,
                                  pLogBlockInfo->m_pBlock);

    if (0 != nRet)
    {
        return false;
    }
    else
    {
        return true;
    }
}

ACE_TString& CLogFile::GetLoggerName()
{
    return m_StrlogName;
}

ACE_TString& CLogFile::GetServerName()
{
    return m_StrServerName;
}

int CLogFile::GetDisPlay()
{
    return m_nDisplay;
}

void CLogFile::SetDisplay(int nDisplay)
{
    if (nDisplay == 0)
    {
        m_nDisplay = 0;
    }
    else
    {
        m_nDisplay = 1;
    }
}

void CLogFile::SetLoggerClass(int nType)
{
    if (nType == 1)  //�����1��������Ӫ��־��������Ǵ�����־
    {
        m_StrlogType = LOGTYPE_OPERATION;
    }
    else
    {
        m_StrlogType = LOGTYPE_ERROR;
    }

    OUR_DEBUG((LM_INFO, "[ServerLogger](%d)m_StrlogType=%s.\n", nType, m_StrlogType.c_str()));
}

int CLogFile::GetLoggerClass()
{
    if (LOGTYPE_OPERATION == m_StrlogType)
    {
        return 1;
    }
    else
    {
        return 2;
    }
}

void CLogFile::SetLoggerName(const char* szLoggerName)
{
    m_StrlogName = szLoggerName;
}

void CLogFile::SetLoggerID(uint16 u2LogID)
{
    m_u2LogID = u2LogID;
}

uint16 CLogFile::GetLoggerID()
{
    return m_u2LogID;
}

void CLogFile::SetLevel(uint16 u2Level)
{
    m_u2Level = u2Level;
}

uint16 CLogFile::GetLevel()
{
    return m_u2Level;
}

void CLogFile::SetServerName(const char* szServerName)
{
    m_StrServerName = szServerName;
}

bool CLogFile::Run()
{
    ACE_Date_Time dt;
    char szDateBuff[MAX_TIME_SIZE] = { '\0' };

    dt.update(ACE_OS::gettimeofday());

    CreatePath();       //���Ŀ¼�������򴴽�Ŀ¼

    sprintf_safe(szDateBuff, MAX_TIME_SIZE, "_%04d-%02d-%02d_%d.log", dt.year(), dt.month(), dt.day(), m_u2CurrFileIndex);
    sprintf_safe(m_szLogTime, MAX_TIME_SIZE, "%04d-%02d-%02d", dt.year(), dt.month(), dt.day());

    ACE_TString strLogModulePath = m_szFileRoot;
    ACE_TString strLogName = strLogModulePath + "/Log/" + m_StrlogType + "/" + m_StrlogName + "/" + m_StrServerName + m_StrlogName + szDateBuff;

    m_File.close();

    m_FileAddr.set((ACE_TCHAR*)strLogName.c_str());

    if (m_Connector.connect(m_File, m_FileAddr, 0, ACE_Addr::sap_any, 0, O_WRONLY | O_CREAT | O_APPEND) == -1)
    {
        OUR_DEBUG((LM_INFO, "[ServerLogger]Create file error[%s].\n", strLogName.c_str()));
        return false;
    }

    //���õ�ǰ�ļ���С��0
    m_u4CurrFileSize = 0;

    OUR_DEBUG((LM_INFO, "[ServerLogger]Create file ok[%s].\n", strLogName.c_str()));
    return true;
}

void CLogFile::CheckTime()
{
    ACE_Date_Time dt;
    dt.update(ACE_OS::gettimeofday());
    char szDate[MAX_TIME_SIZE] = { '\0' };
    sprintf_safe(szDate, MAX_TIME_SIZE, "%04d-%02d-%02d", dt.year(), dt.month(), dt.day());

    if (ACE_OS::strcmp(szDate, m_szLogTime) != 0 && false == Run())
    {
        OUR_DEBUG((LM_INFO, "[ServerLogger](%s)Run fail.\n", m_StrlogName.c_str()));
    }
}

void CLogFile::CheckLogFileBlock()
{
    //�鿴�ļ��Ƿ񳬹����������
    if (m_u4CurrFileSize >= m_u4FileMaxSize)
    {
        //�������ļ�
        m_u2CurrFileIndex++;

        if (false == Run())
        {
            OUR_DEBUG((LM_INFO, "[ServerLogger](%s)Run fail.\n", m_StrlogName.c_str()));
        }
    }
}

void CLogFile::CreatePath()
{
    int n4Return = -1;
    int nError = 0;
    char szPath[MAX_CMD_NUM] = { '\0' };
    sprintf_safe(szPath, MAX_CMD_NUM, "%s/Log/", m_szFileRoot);
    n4Return = ACE_OS::mkdir(szPath);
    nError = errno;

    if (-1 == n4Return && EEXIST != nError)
    {
        OUR_DEBUG((LM_INFO, "[ServerLogger](%s)CreatePath fail.\n", szPath));
    }

    sprintf_safe(szPath, MAX_CMD_NUM, "%s/Log/%s/", m_szFileRoot, m_StrlogType.c_str());
    n4Return = ACE_OS::mkdir(szPath);
    nError = errno;

    if (-1 == n4Return && EEXIST != nError)
    {
        OUR_DEBUG((LM_INFO, "[ServerLogger](%s)CreatePath fail.\n", szPath));
    }

    sprintf_safe(szPath, MAX_CMD_NUM, "%s/Log/%s/%s", m_szFileRoot, m_StrlogType.c_str(), m_StrlogName.c_str());
    n4Return = ACE_OS::mkdir(szPath);
    nError = errno;

    if (-1 == n4Return && EEXIST != nError)
    {
        OUR_DEBUG((LM_INFO, "[ServerLogger](%s)CreatePath fail.\n", szPath));
    }
}

//********************************************************

CFileLogger::CFileLogger()
{
    m_pLogFileList   = NULL;
    m_nCount         = 0;
    m_u4BlockSize    = 0;
    m_u4PoolCount    = 0;
    m_u4CurrLogLevel = 0;
    m_szLogRoot[0]   = '\0';
}

CFileLogger::~CFileLogger()
{
    OUR_DEBUG((LM_INFO, "[CFileLogger::~CFileLogger].\n"));
    Close();
    OUR_DEBUG((LM_INFO, "[CFileLogger::~CFileLogger]End.\n"));
}

void CFileLogger::Close()
{
    if(NULL != m_pLogFileList)
    {
        for(int i = 0; i < m_nCount; i++)
        {
            if(NULL != m_pLogFileList[i])
            {
                SAFE_DELETE(m_pLogFileList[i]);
            }
        }

        SAFE_DELETE_ARRAY(m_pLogFileList);
        m_nCount = 0;
    }
}

int CFileLogger::DoLog(int nLogType, _LogBlockInfo* pLogBlockInfo)
{
    //����LogTypeȡ�࣬��õ�ǰ��־ӳ��λ��
    int nIndex = nLogType % m_nCount;

    if(NULL != m_pLogFileList[nIndex])
    {
        m_pLogFileList[nIndex]->doLog(pLogBlockInfo);
    }

    return 0;
}

int CFileLogger::GetLogTypeCount()
{
    return m_nCount;
}

bool CFileLogger::Init()
{
    CXmlOpeation objXmlOpeation;

    //Ĭ�ϵ����ļ������8M
    uint32 u4FileMaxSize            = MAX_BUFF_500;
    uint16 u2LogID                  = 0;
    uint16 u2LogLevel               = 0;
    uint8  u1FileClass              = 0;
    uint8  u1DisPlay                = 0;
    char szFile[MAX_BUFF_1024]      = {'\0'};
    char szFileName[MAX_BUFF_100]   = {'\0'};
    char szServerName[MAX_BUFF_100] = {'\0'};
    vector<_Log_File_Info> objvecLogFileInfo;

    Close();

    sprintf_safe(szFile, MAX_BUFF_1024, "%s", FILELOG_CONFIG);

    if(false == objXmlOpeation.Init(szFile))
    {
        OUR_DEBUG((LM_ERROR,"[CFileLogger::Init] Read Configfile[%s] failed\n", szFile));
        return false;
    }

    //�õ�����������
    objXmlOpeation.Read_XML_Data_Single_String("ServerLogHead", "Text", szServerName, MAX_BUFF_100);
    OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]strServerName=%s\n", szServerName));

    //�õ�������־����С
    objXmlOpeation.Read_XML_Data_Single_Uint32("ServerLogHead", "LogFileMaxSize", u4FileMaxSize);

    //�õ�����·��
    objXmlOpeation.Read_XML_Data_Single_String("LogPath", "Path", m_szLogRoot, MAX_BUFF_100);
    OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]m_strRoot=%s\n", m_szLogRoot));

    //�õ���־��������Ϣ����־��Ĵ�С
    objXmlOpeation.Read_XML_Data_Single_Uint32("LogPool", "BlockSize", m_u4BlockSize);

    //�õ���־��������Ϣ�����������־��ĸ���
    objXmlOpeation.Read_XML_Data_Single_Uint32("LogPool", "PoolCount", m_u4PoolCount);

    //�õ���־���еĵ�ǰ��־����
    //�˹��ܸ�л��/ka�̷� �ĺ��뷨����һ�������ٳɶ�ͻ��ۺ���
    objXmlOpeation.Read_XML_Data_Single_Uint32("LogLevel", "CurrLevel", m_u4CurrLogLevel);

    //��������ĸ���
    TiXmlElement* pNextTiXmlElement        = NULL;
    TiXmlElement* pNextTiXmlElementPos     = NULL;
    TiXmlElement* pNextTiXmlElementIdx     = NULL;
    TiXmlElement* pNextTiXmlElementDisplay = NULL;
    TiXmlElement* pNextTiXmlElementLevel   = NULL;

    while(true)
    {
        //�õ���־id
        if(true == objXmlOpeation.Read_XML_Data_Multiple_Uint16("LogInfo", "logid", u2LogID, pNextTiXmlElementIdx))
        {
            OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]u2LogID=%d\n", u2LogID));
        }
        else
        {
            break;
        }

        //�õ���־����
        objXmlOpeation.Read_XML_Data_Multiple_String("LogInfo", "logname", szFileName, MAX_BUFF_100, pNextTiXmlElement);
        OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]strFileValue=%s\n", szFileName));

        //�õ���־����
        objXmlOpeation.Read_XML_Data_Multiple_Uint8("LogInfo", "logtype", u1FileClass, pNextTiXmlElementPos);
        OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]u1FileClass=%d\n", u1FileClass));

        //�õ���־�����Դ��0Ϊ������ļ���1Ϊ�������Ļ
        objXmlOpeation.Read_XML_Data_Multiple_Uint8("LogInfo", "Display", u1DisPlay, pNextTiXmlElementDisplay);
        OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]u1DisPlay=%d\n", u1DisPlay));

        //�õ���־��ǰ����
        objXmlOpeation.Read_XML_Data_Multiple_Uint16("LogInfo", "Level", u2LogLevel, pNextTiXmlElementLevel);
        OUR_DEBUG((LM_ERROR, "[CFileLogger::readConfig]u4LogLevel=%d\n", u2LogLevel));

        //���뻺��
        _Log_File_Info obj_Log_File_Info;
        obj_Log_File_Info.m_u2LogID     = u2LogID;
        obj_Log_File_Info.m_u1FileClass = u1FileClass;
        obj_Log_File_Info.m_u1DisPlay   = u1DisPlay;
        obj_Log_File_Info.m_u2LogLevel  = u2LogLevel;
        sprintf_safe(obj_Log_File_Info.m_szFileName, 100, "%s", szFileName);

        objvecLogFileInfo.push_back(obj_Log_File_Info);
        m_nCount++;
    }

    //���������б�
    m_pLogFileList = new CLogFile*[m_nCount];
    memset(m_pLogFileList, 0, sizeof(CLogFile*)*m_nCount);

    for(int i = 0; i < (int)objvecLogFileInfo.size(); i++)
    {
        int nPos = objvecLogFileInfo[i].m_u2LogID % m_nCount;
        CLogFile* pLogFile = new CLogFile(m_szLogRoot, m_u4BlockSize, u4FileMaxSize);

        pLogFile->SetLoggerName(objvecLogFileInfo[i].m_szFileName);
        pLogFile->SetLoggerID(objvecLogFileInfo[i].m_u2LogID);
        pLogFile->SetLoggerClass(objvecLogFileInfo[i].m_u1FileClass);
        pLogFile->SetLevel(objvecLogFileInfo[i].m_u2LogLevel);
        pLogFile->SetServerName(szServerName);
        pLogFile->SetDisplay(objvecLogFileInfo[i].m_u1DisPlay);
        pLogFile->Init();

        if (false == pLogFile->Run())
        {
            OUR_DEBUG((LM_INFO, "[CFileLogger::Init]Run error.\n"));
        }

        m_pLogFileList[nPos] = pLogFile;
    }

    return true;
}

bool CFileLogger::ReSet(uint32 u4CurrLogLevel)
{
    //������־�ȼ�
    m_u4CurrLogLevel = u4CurrLogLevel;
    return true;
}

uint32 CFileLogger::GetBlockSize()
{
    return m_u4BlockSize;
}

uint32 CFileLogger::GetPoolCount()
{
    return m_u4PoolCount;
}

uint32 CFileLogger::GetCurrLevel()
{
    return m_u4CurrLogLevel;
}

uint16 CFileLogger::GetLogID(uint16 u2Index)
{
    if(u2Index >= m_nCount)
    {
        return 0;
    }

    return m_pLogFileList[u2Index]->GetLoggerID();
}

char* CFileLogger::GetLogInfoByServerName(uint16 u2LogID)
{
    int nIndex = u2LogID % m_nCount;

    return (char* )m_pLogFileList[nIndex]->GetServerName().c_str();
}

char* CFileLogger::GetLogInfoByLogName(uint16 u2LogID)
{
    int nIndex = u2LogID % m_nCount;

    return (char* )m_pLogFileList[nIndex]->GetLoggerName().c_str();
}

int CFileLogger::GetLogInfoByLogDisplay(uint16 u2LogID)
{
    int nIndex = u2LogID % m_nCount;

    return m_pLogFileList[nIndex]->GetDisPlay();
}

uint16 CFileLogger::GetLogInfoByLogLevel(uint16 u2LogID)
{
    int nIndex = u2LogID % m_nCount;

    return m_pLogFileList[nIndex]->GetLevel();
}

