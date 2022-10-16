#ifndef __KCP_CPP_EXAMPLE_TEST_CASE_FACTORY_H__
#define __KCP_CPP_EXAMPLE_TEST_CASE_FACTORY_H__

#pragma once

#include "ITestCase.h"

class TestCaseFactory
{
public:
    static ITestCase *Create();
};

#endif

