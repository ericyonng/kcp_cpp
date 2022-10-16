#include "pch.h"
#include "TestKcpCase.h"
#include "kcp/kcp_cpp.h"
#include "kcp_core/kissnet.hpp"

void TestKcpCase::Release()
{
    delete this;
}

void TestKcpCase::Run()
{
    g_Log->Info(LOGFMT_OBJ_TAG("Hello TestKcpCase."));
}