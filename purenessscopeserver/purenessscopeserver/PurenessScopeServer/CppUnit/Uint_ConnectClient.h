#ifndef UNIT_CONNECTCLIENT_H
#define UNIT_CONNECTCLIENT_H

#ifdef _CPPUNIT_TEST

#include "Unit_Common.h"
#include "ConnectClient.h"

class CUnit_ConnectClient : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUnit_ConnectClient);
    CPPUNIT_TEST(Test_handle_input);
    CPPUNIT_TEST(Test_GetTimeout);
    CPPUNIT_TEST_SUITE_END();

public:
    virtual ~CUnit_ConnectClient();

    virtual void setUp(void);

    virtual void tearDown(void);

    void Test_handle_input(void);

    void Test_GetTimeout(void);

private:
    CConnectClient* m_pConnectClient;
};

#endif

#endif
