#ifndef UNIT_BASEHANDLER_H
#define UNIT_BASEHANDLER_H

#ifdef _CPPUNIT_TEST

#include "Unit_Common.h"
#include "BaseHander.h"

class CUnit_Basehandler : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUnit_Basehandler);
    CPPUNIT_TEST(Test_Tcp_Common_File_Message);
    CPPUNIT_TEST(Test_Tcp_Common_ClientInfo);
    CPPUNIT_TEST(Test_Tcp_Common_Send_Input_To_Cache);
    CPPUNIT_TEST(Test_Tcp_Common_Manager_Timeout_CheckInfo);
    CPPUNIT_TEST(Test_Tcp_Common_ClientNameInfo);
    CPPUNIT_TEST(Test_Udp_Common_Recv_Stream);
    CPPUNIT_TEST(Test_Udp_Common_Send_Message);
    CPPUNIT_TEST(Test_Tcp_Common_Make_Send_Packet);
    CPPUNIT_TEST_SUITE_END();
public:
    CUnit_Basehandler();

    virtual ~CUnit_Basehandler();

    CUnit_Basehandler(const CUnit_Basehandler& ar);

    CUnit_Basehandler& operator = (const CUnit_Basehandler& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    virtual void setUp(void);

    virtual void tearDown(void);

    void Test_Tcp_Common_File_Message(void);
    void Test_Tcp_Common_ClientInfo(void);
    void Test_Tcp_Common_Send_Input_To_Cache(void);
    void Test_Tcp_Common_Manager_Timeout_CheckInfo(void);
    void Test_Tcp_Common_ClientNameInfo(void);
    void Test_Udp_Common_Recv_Stream(void);
    void Test_Udp_Common_Send_Message(void);
    void Test_Tcp_Common_Make_Send_Packet(void);
private:
    int m_nBaseHandlerID;

};

#endif

#endif
