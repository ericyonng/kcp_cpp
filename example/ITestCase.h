#ifndef __KCP_CPP_EXAMPLE_ITEST_CASE_H__
#define __KCP_CPP_EXAMPLE_ITEST_CASE_H__

#pragma once

class ITestCase
{
public:
    virtual void Run() = 0;
    virtual void Release() = 0;
};

#endif

