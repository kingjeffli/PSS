#ifndef _UNIT_LOGGING_H
#define _UNIT_LOGGING_H

#ifdef _CPPUNIT_TEST

#include "Unit_Common.h"
#include "define.h"
#include "Frame_Logging_Strategy.h"

class CUnit_Frame_Logging : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(CUnit_Frame_Logging);
    CPPUNIT_TEST(Test_Debug_Log);
    CPPUNIT_TEST_SUITE_END();

public:
    CUnit_Frame_Logging();

    virtual ~CUnit_Frame_Logging();

    CUnit_Frame_Logging(const CUnit_Frame_Logging& ar);

    CUnit_Frame_Logging& operator = (const CUnit_Frame_Logging& ar)
    {
        if (this != &ar)
        {
            ACE_UNUSED_ARG(ar);
        }

        return *this;
    }

    virtual void setUp(void);

    virtual void tearDown(void);

    void Test_Debug_Log(void);

private:
    Frame_Logging_Strategy* m_pFrameLoggingStrategy;
};

#endif

#endif
