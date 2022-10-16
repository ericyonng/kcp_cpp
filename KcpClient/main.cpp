

#include "pch.h"
#include "KcpClient.h"
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

    //g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel init suc."));
    KCP_CPP_NS::KernelUtil::Start();
    //g_Log->Info(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel start suc."));

    do
    {
        auto kcpClient = new KcpClient;
        kcpClient->SetFramInterval(10);
        kcpClient->SetHeartbeat(3000);
        // kcpClient->ConnectTo("127.0.0.1", 7500);
        kcpClient->ConnectTo("127.0.0.1", 9001);
        err = kcpClient->Init(1, "127.0.0.1");
        if(err != Status::Success)
        {
            g_Log->Error(LOGFMT_NON_OBJ_TAG(KcpClient, "kcp client init fail err:%d"), err);
            break;
        }

        err = kcpClient->Start();

        if(err != Status::Success)
        {
            g_Log->Error(LOGFMT_NON_OBJ_TAG(KcpClient, "kcp client start fail err:%d"), err);
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

        g_Log->Info(LOGFMT_NON_OBJ_TAG(KcpClient, "will close client."));
        kcpClient->Close();

    } while (false);
    
    g_Log->Info(LOGFMT_NON_OBJ_TAG(KcpClient,"-------------------- KCP CLIENT FINISH -----------------------------"));

    #ifdef _WIN32
     system("pause");
    #endif

    KCP_CPP_NS::KernelUtil::Destroy();

    return 0;
}