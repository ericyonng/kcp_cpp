
#include "pch.h"
#include "kcp/kcp_cpp.h"
#include "TestCaseFactory.h"


class LibTestLog : public KCP_CPP_NS::LibLog
{
public:
    LibTestLog() {}
    ~LibTestLog() {}
};

class LogFactory : public KCP_CPP_NS::ILogFactory
{
public:
    virtual KCP_CPP_NS::ILog *Create()
    {
        return new LibTestLog();
    }
};

int main(int argv, char const *argc[])
{
    LogFactory logFactory;
    auto err = KCP_CPP_NS::KernelUtil::Init(&logFactory, "LogCfg.ini", "./ini/");   
    if(err != Status::Success)
    {
        printf("kernel init fail.\n");
        return 0;
    }

    g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel init suc."));
    KCP_CPP_NS::KernelUtil::Start();
    g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel start suc."));

    auto testCase = TestCaseFactory::Create();
    g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "-------------------- TEST START -----------------------------"));
    testCase->Run();
    testCase->Release();

    g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil,"-------------------- TEST FINISH -----------------------------"));
    g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "test finish.")); 

    #ifdef _WIN32
     system("pause");
    #endif

    KCP_CPP_NS::KernelUtil::Destroy();
    return 0;
}
