

#include "pch.h"
#include "KcpServer.h"
#include "DevelopmentKit/DevelopmentKit.h"

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

int main()
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

    do
    {
        auto kcpSvr = new KcpServer;
        kcpSvr->SetFramInterval(1);
        kcpSvr->SetHeartbeatMilliseconds(5000);
        // kcpSvr->SetHeartbeatMilliseconds(0);
        err = kcpSvr->Init("127.0.0.1", 9001);
        if(err != Status::Success)
        {
            g_Log->Error(LOGFMT_NON_OBJ_TAG(KcpServer, "kcp svr init fail err:%d"), err);
            break;
        }

        err = kcpSvr->Start();

        if(err != Status::Success)
        {
            g_Log->Error(LOGFMT_NON_OBJ_TAG(KcpServer, "kcp svr start fail err:%d"), err);
            break;
        }

        while(true)
        {
            auto cmd = getchar();
            if(cmd == 'q')
            {
                break;
            }
        }

        kcpSvr->Close();

    } while (false);
    
    g_Log->Info(LOGFMT_NON_OBJ_TAG(KcpServer,"-------------------- KCP SVR FINISH -----------------------------"));

    #ifdef _WIN32
     system("pause");
    #endif

    KCP_CPP_NS::KernelUtil::Destroy();

    return 0;
}