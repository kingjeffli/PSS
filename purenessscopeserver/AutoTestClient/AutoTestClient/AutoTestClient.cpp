// AutoTestClient.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "XmlOpeation.h"
#include "TcpSocketClient.h"

#define XML_PATH "XML_Packet"
#define XML_FILE_RESULT "Auto_Test_Resault.html"

//���в����Ӽ�
void Run_Test(FILE* pFile, _Command_Info obj_Command_Info, const char* pIP, int nPort)
{
	//����Զ�̲���
	ODSocket obj_ODSocket;
	obj_ODSocket.Init();
	obj_ODSocket.Create(AF_INET, SOCK_STREAM, 0);
	bool blState = obj_ODSocket.Connect(pIP, nPort);
	if(false == blState)
	{
		Create_TD_Content(pFile, "error", obj_Command_Info.m_szCommandName, "����ʧ��");
	}
	else
	{
		Create_TD_Content(pFile, "content", obj_Command_Info.m_szCommandName, "���Գɹ�");
	}
	obj_ODSocket.Close();
}

//���в��Լ�����ý��
void Run_Assemble_List(vec_Test_Assemble obj_Test_Assemble_List)
{
	FILE* pFile = fopen(XML_FILE_RESULT, "w");
	if(NULL == pFile)
	{
		printf("[Run_Assemble_List](%s) File Create fail.\n");
		return;
	}

	//����html�ļ�ͷ
	Create_HTML_Begin(pFile);

	for(int i = 0; i < (int)obj_Test_Assemble_List.size(); i++)
	{
		Create_TD_Title(pFile, obj_Test_Assemble_List[i].m_szTestAssembleName, obj_Test_Assemble_List[i].m_szDesc, 
						obj_Test_Assemble_List[i].m_szIP, obj_Test_Assemble_List[i].m_nPort);
		for(int j = 0; j < (int)obj_Test_Assemble_List[i].m_obj_Command_Info_List.size(); j++)
		{
			Run_Test(pFile, obj_Test_Assemble_List[i].m_obj_Command_Info_List[j], 
						obj_Test_Assemble_List[i].m_szIP,
						obj_Test_Assemble_List[i].m_nPort);
		}
	}
	Create_HTML_End(pFile);
}


int main(int argc, char* argv[])
{
	vec_Xml_File_Name obj_vec_Xml_File_Name;
	CXmlOpeation      obj_XmlOpeation;
	vec_Test_Assemble obj_Test_Assemble_List;

	bool blRet = Read_Xml_Folder(XML_PATH, obj_vec_Xml_File_Name);
	if(false == blRet)
	{
		printf("[Main]Get XML path error.\n");
		printf("[Main]please any key to exit.\n");
		getchar();
	}

	for(int i = 0; i < (int)obj_vec_Xml_File_Name.size(); i++)
	{
		_Test_Assemble obj_Test_Assemble;
		obj_XmlOpeation.Parse_XML_Test_Assemble(obj_vec_Xml_File_Name[i].c_str(), obj_Test_Assemble);
		obj_Test_Assemble_List.push_back(obj_Test_Assemble);
	}

	//���в�������
	Run_Assemble_List(obj_Test_Assemble_List);

	return 0;
}
