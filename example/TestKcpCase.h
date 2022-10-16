#pragma once

#include "ITestCase.h"

class TestKcpCase : public ITestCase
{
public:
    virtual void Run() override;
    virtual void Release() override;
};
