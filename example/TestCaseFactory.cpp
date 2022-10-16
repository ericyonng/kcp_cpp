#include "pch.h"
#include <kcp/kcp_cpp.h>

#pragma region // 所有的测试用例

// 测试kcp
#include "TestKcpCase.h"

#pragma endregion 

#include "TestCaseFactory.h"

ITestCase *TestCaseFactory::Create()
{
    return new TestKcpCase();
}