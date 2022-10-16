


#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_KERNEL_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_KERNEL_UTIL_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

class ILogFactory;

class KernelUtil
{
public:
    // 框架初始化与销毁
    static Int32 Init(ILogFactory * logFactory, const Byte8 *logIniName, const Byte8 *iniPath);
    static void Start();
    static void Destroy();
};

KCP_CPP_END

extern KCP_CPP_NS::LibString g_LogIniName;
extern KCP_CPP_NS::LibString g_LogIniRootPath;

#endif