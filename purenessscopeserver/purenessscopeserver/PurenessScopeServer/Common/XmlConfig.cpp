#include "XmlConfig.h"

/*����Ԫ�ض�Ӧ��Ԫ��λ���Ѿ��ڹ���ʱ��ö��ȷ��*/
IConfigOpeation* IConfigOpeation::_array[XML_Config_MAX];


/*���徲̬��������ʵ��ʵ��ģ�庯��*/
#define DefineClassAndFunc(ClassName)                                   \
static ClassName    this##ClassName;                                    \
template<>                                                              \
ClassName* XMainConfig::GetXmlConfig<ClassName>(XmlConfig config)       \
{                                                                       \
    return dynamic_cast<ClassName*>(IConfigOpeation::_array[config]);   \
}


/*xml�����ļ���Ӧ�������õľ�̬�࣬��Ҫ���������ļ���ǩ����Ҫ�ڴ����Ӿ�̬��������ж���ֱ��ʹ��,Ҳ�������ⲿʹ��*/
/*****************�����ͷ��غ���һһ��Ӧ*********************/
DefineClassAndFunc(xmlSendInfo)
DefineClassAndFunc(xmlNetWorkMode)
DefineClassAndFunc(xmlConnectServer)
DefineClassAndFunc(xmlClientInfo)
DefineClassAndFunc(xmlModuleInfos)
DefineClassAndFunc(xmlModuleMangager)
DefineClassAndFunc(xmlMonitor)
DefineClassAndFunc(xmlThreadInfoAI)
DefineClassAndFunc(xmlThreadInfo)
DefineClassAndFunc(xmlConsole)
DefineClassAndFunc(xmlConsoleKeys)
DefineClassAndFunc(xmlAceDebug)
DefineClassAndFunc(xmlCommandAccount)
DefineClassAndFunc(xmlCoreSetting)
DefineClassAndFunc(xmlServerType)
DefineClassAndFunc(xmlServerID)
DefineClassAndFunc(xmlServerName)
DefineClassAndFunc(xmlServerVersion)
DefineClassAndFunc(xmlPacketParses)
DefineClassAndFunc(xmlBuffPacket)
DefineClassAndFunc(xmlMessage)
DefineClassAndFunc(xmlAlertConnect)
DefineClassAndFunc(xmlIP)
DefineClassAndFunc(xmlClientData)
DefineClassAndFunc(xmlCommandInfo)
DefineClassAndFunc(xmlMail)
DefineClassAndFunc(xmlWorkThreadChart)
DefineClassAndFunc(xmlConnectChart)
DefineClassAndFunc(xmlCommandChart)



bool XMainConfig::Init()
{
	//��ʼ��xml�ļ�
	return Init(MAINCONFIG, XML_Config_RecvInfo, XML_Config_Message)
		&& Init(ALERTCONFIG, XML_Config_AlertConnect, XML_Config_CommandChart);
}

bool XMainConfig::Init(const char* pFileName, XmlConfig start, XmlConfig end)
{
	bool bKet = true;

	if (m_XmlOpeation.Init(pFileName))
	{
		/*
		ע��forѭ��Ϊ2���ж�����
		1.��Χ
		2.init�������ؽ��
		*/
		for (int i = start; i <= end && bKet; ++i)
		{
			bKet = IConfigOpeation::_array[i]->Init(&m_XmlOpeation);
		}
	}
	else
	{
		bKet = false;
		OUR_DEBUG((LM_INFO, "[XMainConfig::Init]File Read Error = %s.\n", pFileName));
	}

	return bKet;
}

/**********************************��ӦXmlConfig����Եĳ�ʼ����Ϊ***************************************/

bool xmlRecvInfo::Init(CXmlOpeation* m_pXmlOpeation)
{
	return m_pXmlOpeation->Read_XML_Data_Single_Uint16("RecvInfo", "RecvQueueTimeout", RecvQueueTimeout)
		&& m_pXmlOpeation->Read_XML_Data_Single_Uint32("RecvInfo", "RecvBuffSize", RecvBuffSize);
}

bool xmlSendInfo::Init(CXmlOpeation* m_pXmlOpeation)
{
	bool bKet = false;

	if (m_pXmlOpeation->Read_XML_Data_Single_Uint16("SendInfo", "SendTimeout", SendTimeout)
		&& m_pXmlOpeation->Read_XML_Data_Single_Uint16("SendInfo", "SendQueueMax", SendQueueMax)
		&& m_pXmlOpeation->Read_XML_Data_Single_Uint16("SendInfo", "PutQueueTimeout", PutQueueTimeout)
		&& m_pXmlOpeation->Read_XML_Data_Single_Uint32("SendInfo", "BlockCount", BlockCount)
		&& m_pXmlOpeation->Read_XML_Data_Single_Uint32("SendInfo", "MaxBlockSize", MaxBlockSize))
	{
		SendDatamark = MaxBlockSize;
		bKet = m_pXmlOpeation->Read_XML_Data_Single_Uint16("SendInfo", "TcpNodelay", TcpNodelay);

		if (TcpNodelay != TCP_NODELAY_ON)
		{
			TcpNodelay = TCP_NODELAY_OFF;
		}
	}

	return bKet;
}

bool xmlNetWorkMode::Init(CXmlOpeation* m_pXmlOpeation)
{
	bool bKet = false;
	string str_mode;
	string str_byte;

	if (m_pXmlOpeation->Read_XML_Data_Single_String("NetWorkMode", "Mode", str_mode)
		&& m_pXmlOpeation->Read_XML_Data_Single_Uint16("NetWorkMode", "BackLog", BackLog)
		&& m_pXmlOpeation->Read_XML_Data_Single_String("NetWorkMode", "ByteOrder", str_byte))
	{
		Mode = (uint8)ACE_OS::atoi(str_mode.c_str());
		ByteOrder = (uint8)ACE_OS::atoi(str_byte.c_str());
		bKet = true;
	}

	return bKet;
}


bool xmlConnectServer::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("ConnectServer", "TimeInterval", TimeInterval)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ConnectServer", "Recvbuff", Recvbuff)
		&& pXmlOpeation->Read_XML_Data_Single_Uint8("ConnectServer", "RunType", RunType)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ConnectServer", "TimeCheck", TimeCheck)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ConnectServer", "Count", Count);
}

bool xmlClientInfo::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("ClientInfo", "HandlerCount", HandlerCount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ClientInfo", "MaxHandlerCount", MaxHandlerCount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ClientInfo", "MaxConnectTime", MaxConnectTime)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ClientInfo", "CheckAliveTime", CheckAliveTime)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ClientInfo", "MaxBuffRecv", MaxBuffRecv);
}

bool xmlModuleInfos::Init(CXmlOpeation* pXmlOpeation)
{
	TiXmlElement* pName = NULL;
	TiXmlElement* pPath = NULL;
	TiXmlElement* pParam = NULL;

	_ModuleInfo moduleInfo;

	while (pXmlOpeation->Read_XML_Data_Multiple_String("ModuleInfo", "ModuleSName", moduleInfo.szModuleName, pName)
		&& pXmlOpeation->Read_XML_Data_Multiple_String("ModuleInfo", "ModuleSPath", moduleInfo.szModulePath, pPath)
		&& pXmlOpeation->Read_XML_Data_Multiple_String("ModuleInfo", "ModuleSParam", moduleInfo.szModuleParam, pParam))
	{
		vec.push_back(moduleInfo);
	}

	return true;
}

bool xmlModuleMangager::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("ModuleManager", "MaxCount", MaxCount);
}

bool xmlMonitor::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("Monitor", "CpuAndMemory", CpuAndMemory)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("Monitor", "CpuMax", CpuMax)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("Monitor", "MemoryMax", MemoryMax);
}

bool xmlThreadInfoAI::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("ThreadInfoAI", "AI", AI)
		&& pXmlOpeation->Read_XML_Data_Single_Uint8("ThreadInfoAI", "ReturnDataType", ReturnDataType)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfoAI", "CheckTime", CheckTime)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfoAI", "TimeoutCount", TimeoutCount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfoAI", "StopTime", StopTime)
		&& pXmlOpeation->Read_XML_Data_Single_String("ThreadInfoAI", "ReturnData", ReturnData);
}

bool xmlThreadInfo::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfo", "ThreadTimeout", ThreadTimeout)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfo", "ThreadTimeCheck", ThreadTimeCheck)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfo", "DisposeTimeout", DisposeTimeout)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ThreadInfo", "PutQueueTimeout", PutQueueTimeout);
}

bool xmlConsole::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("Console", "support", support)
		&& pXmlOpeation->Read_XML_Data_Single_String("Console", "sip", sip)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("Console", "sport", sport);
}

bool xmlConsoleKeys::Init(CXmlOpeation* pXmlOpeation)
{
	TiXmlElement* pKey = NULL;
	_ConsoleKey consolekey;

	while (pXmlOpeation->Read_XML_Data_Multiple_String("ConsoleKey", "Key", consolekey.Key, pKey))
	{
		vec.push_back(consolekey);
	}

	return true;
}

bool xmlAceDebug::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("AceDebug", "TrunOn", TrunOn)
		&& pXmlOpeation->Read_XML_Data_Single_String("AceDebug", "DebugName", DebugName)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("AceDebug", "ChkInterval", ChkInterval)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("AceDebug", "LogFileMaxSize", LogFileMaxSize)
		&& pXmlOpeation->Read_XML_Data_Single_Uint8("AceDebug", "LogFileMaxCnt", LogFileMaxCnt)
		&& pXmlOpeation->Read_XML_Data_Single_String("AceDebug", "Level", Level);
}

bool xmlCommandAccount::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("CommandAccount", "Account", Account)
		&& pXmlOpeation->Read_XML_Data_Single_Uint8("CommandAccount", "FlowAccount", FlowAccount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("CommandAccount", "MaxCommandCount", MaxCommandCount);
}

bool xmlCoreSetting::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("CoreSetting", "CoreNeed", CoreNeed)
		&& pXmlOpeation->Read_XML_Data_Single_String("CoreSetting", "Script", Script);
}

bool xmlServerType::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("ServerType", "Type", Type)
		&& pXmlOpeation->Read_XML_Data_Single_String("ServerType", "name", name)
		&& pXmlOpeation->Read_XML_Data_Single_String("ServerType", "displayname", displayname)
		&& pXmlOpeation->Read_XML_Data_Single_Uint8("ServerType", "Debug", Debug)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ServerType", "DebugSize", DebugSize)
		&& pXmlOpeation->Read_XML_Data_Single_Uint8("ServerType", "IsClose", IsClose);
}

bool xmlServerID::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("ServerID", "id", id);
}

bool xmlServerName::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_String("ServerName", "name", name);
}

bool xmlServerVersion::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_String("ServerVersion", "Version", Version);
}

bool xmlPacketParses::Init(CXmlOpeation* pXmlOpeation)
{
	TiXmlElement* pParseID = NULL;
	TiXmlElement* pPath = NULL;
	TiXmlElement* pName = NULL;
	TiXmlElement* pType = NULL;
	TiXmlElement* pOrg = NULL;

	_PacketParse packetparse;

	while (pXmlOpeation->Read_XML_Data_Multiple_Uint16("PacketParse", "ParseID", packetparse.uParseID, pParseID)
		&& pXmlOpeation->Read_XML_Data_Multiple_String("PacketParse", "ModulePath", packetparse.szModulePath, pPath)
		&& pXmlOpeation->Read_XML_Data_Multiple_String("PacketParse", "ModuleName", packetparse.szModuleName, pName)
		&& pXmlOpeation->Read_XML_Data_Multiple_String("PacketParse", "Type", packetparse.szType, pType)
		&& pXmlOpeation->Read_XML_Data_Multiple_Uint16("PacketParse", "OrgLength", packetparse.uOrgLength, pOrg))
	{
		_vec.push_back(packetparse);
	}

	return true;
}

bool xmlBuffPacket::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("BuffPacket", "Count", Count);
}

bool xmlMessage::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("Message", "Msg_High_mark", Msg_High_mark)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("Message", "Msg_Low_mark", Msg_Low_mark)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("Message", "Msg_Buff_Max_Size", Msg_Buff_Max_Size)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("Message", "Msg_Thread", Msg_Thread)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("Message", "Msg_MaxQueue", Msg_MaxQueue);
}

/******************************alert.xml***************************************/
bool xmlAlertConnect::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("AlertConnect", "ConnectMin", ConnectMin)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("AlertConnect", "ConnectMax", ConnectMax)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("AlertConnect", "DisConnectMin", DisConnectMin)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("AlertConnect", "DisConnectMax", DisConnectMax)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("AlertConnect", "ConnectAlert", ConnectAlert)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("AlertConnect", "MailID", MailID);
}

bool xmlIP::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint8("IP", "IPMax", IPMax)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("IP", "Timeout", Timeout);
}

bool xmlClientData::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint16("ClientData", "RecvPacketCount", RecvPacketCount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ClientData", "RecvDataMax", RecvDataMax)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("ClientData", "SendPacketCount", SendPacketCount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ClientData", "SendDataMax", SendDataMax)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ClientData", "MailID", MailID);
}

bool xmlCommandInfo::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint32("CommandInfo", "CommandID", CommandID)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("CommandInfo", "CommandCount", CommandCount)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("CommandInfo", "MailID", MailID);
}

bool xmlMail::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint16("Mail", "MailID", MailID)
		&& pXmlOpeation->Read_XML_Data_Single_String("Mail", "fromMailAddr", fromMailAddr)
		&& pXmlOpeation->Read_XML_Data_Single_String("Mail", "toMailAddr", toMailAddr)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("Mail", "MailPass", MailPass)
		&& pXmlOpeation->Read_XML_Data_Single_String("Mail", "MailUrl", MailUrl)
		&& pXmlOpeation->Read_XML_Data_Single_Uint16("Mail", "MailPort", MailPort);
}

bool xmlWorkThreadChart::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint16("WorkThreadChart", "JsonOutput", JsonOutput)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("WorkThreadChart", "Count", Count)
		&& pXmlOpeation->Read_XML_Data_Single_String("WorkThreadChart", "File", File);
}

bool xmlConnectChart::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint16("ConnectChart", "JsonOutput", JsonOutput)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("ConnectChart", "Count", Count)
		&& pXmlOpeation->Read_XML_Data_Single_String("ConnectChart", "File", File);
}

bool xmlCommandChart::Init(CXmlOpeation* pXmlOpeation)
{
	return pXmlOpeation->Read_XML_Data_Single_Uint16("CommandChart", "JsonOutput", JsonOutput)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("CommandChart", "Count", Count)
		&& pXmlOpeation->Read_XML_Data_Single_Uint32("CommandChart", "CommandID", CommandID)
		&& pXmlOpeation->Read_XML_Data_Single_String("CommandChart", "File", File);
}