
#include <pch.h>
#include <kcp/util/KernelUtil.h>
#include <kcp/LibThread.h>
#include <kcp/util/SystemUtil.h>
#include <kcp/ILogFactory.h>
#include <kcp/util/DirectoryUtil.h>
#include <kcp/util/TimeUtil.h>
#include <kcp/util/CrashUtil.h>
#include <kcp/LibLog.h>

KCP_CPP_NS::LibString g_LogIniName;
KCP_CPP_NS::LibString g_LogIniRootPath;

KCP_CPP_BEGIN

Int32 KernelUtil::Init(ILogFactory *logFactory, const Byte8 *logIniName, const Byte8 *iniPath)
{
    // 转入后台
    LibString rootDir = SystemUtil::GetCurProgRootPath();

    // 标准输出,标准错误输出重定向到文件 TODO:版本发布情况下在外部开启,内核初始化不需要
    // auto stdiolog = rootDir + "stdio.log";
    // KERNEL_NS::SystemUtil::TurnDaemon(stdiolog, rootDir);

    // ini 文件路径
    LibString iniRoot;
    if(!iniPath)
    {
        iniRoot = rootDir + ROOT_DIR_INI_SUB_DIR;
    }
    else
    {
        iniRoot = iniPath;
    }

    // 创建路径
    DirectoryUtil::CreateDir(iniRoot);

    // 初始化时区
    TimeUtil::SetTimeZone();

    // 主线程初始化
    const auto mainThreadId = SystemUtil::GetCurrentThreadId();

    // 日志初始化与启动
    g_Log = logFactory->Create();
    if(!g_Log->Init(logIniName, iniRoot.c_str()))
    {
        return false;
    }

    g_LogIniName = logIniName;
    g_LogIniRootPath = iniRoot;

    // 堆栈
    auto destroyDelg = KCP_CPP_NS::DelegateFactory::Create(&KernelUtil::Destroy);
    auto err = KCP_CPP_NS::CrashUtil::InitCrashHandleParams(g_Log, destroyDelg);
    if(err != Status::Success)
    {
        g_Log->Error(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "InitCrashHandleParams fail err=[%d]."), err);
        return err;
    }

    // 网络环境初始化
    WSADATA wsaData;
    memset(&wsaData, 0, sizeof(WSADATA));
    auto result = ::WSAStartup(MAKEWORD(2, 2), &wsaData);
    if(LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
        g_Log->Error(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "WSAStartup fail LOBYTE wVersion not 2 or HIBYTE wVersion not 2  result[%d]"), result);
        return Status::Error;
    }

    if(result != NO_ERROR)
    {
        g_Log->Error(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "WSAStartup fail result[%d]"), result);
        return Status::Error;
    }

    g_Log->Sys(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel inited."));

    return Status::Success;
}

void KernelUtil::Start()
{
    // 日志启动
    g_Log->Start();

    g_Log->Sys(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel started."));
}

void KernelUtil::Destroy()
{
    g_Log->Sys(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "kernel will destroy."));

    if(LIKELY(g_Log))
        g_Log->Sys(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "comp will destroy."));
    
    int result = NO_ERROR;
    result = ::WSACleanup();
    if(result != NO_ERROR)
        g_Log->Warn(LOGFMT_NON_OBJ_TAG(KCP_CPP_NS::KernelUtil, "WSACleanup fail."));

    // 日志关闭
    if(LIKELY(g_Log))
        g_Log->Close();
    g_Log = NULL;
}

KCP_CPP_END