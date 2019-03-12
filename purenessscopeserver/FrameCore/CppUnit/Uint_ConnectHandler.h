#ifndef UNIT_CONNECTHANDLER_H
#define UNIT_CONNECTHANDLER_H

#ifdef _CPPUNIT_TEST

#include "Unit_Common.h"
#include "ConnectHandler.h"

class CUnit_ConnectHandler : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUnit_ConnectHandler);
    CPPUNIT_TEST(Test_ConnectHandler_Stream);
    CPPUNIT_TEST(Test_ConnectHandler_CloseMessages);
    CPPUNIT_TEST(Test_ConnectHandler_Debug);
    CPPUNIT_TEST(Test_ConnectHandler_Close_Queue);
    CPPUNIT_TEST(Test_ConnectHandler_PostMessage);
    CPPUNIT_TEST_SUITE_END();

public:
    CUnit_ConnectHandler();

    virtual ~CUnit_ConnectHandler();

    CUnit_ConnectHandler(const CUnit_ConnectHandler& ar);

    CUnit_ConnectHandler& operator = (const CUnit_ConnectHandler& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    virtual void setUp(void);

    virtual void tearDown(void);

    void Test_ConnectHandler_Stream(void);

    void Test_ConnectHandler_CloseMessages(void);

    void Test_ConnectHandler_Debug(void);

    void Test_ConnectHandler_Close_Queue(void);

    void Test_ConnectHandler_PostMessage(void);

private:
    CConnectHandler* m_pConnectHandler;
};

#endif

#endif
