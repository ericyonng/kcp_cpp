
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_LEVEL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_LEVEL_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

// 这里仅仅保留系统默认提供的日志等级,其他扩展请在业务层面继承并在配置中配置相应的等级id即可,且系统
// 且系统默认提供的等级id配置中不允许更改
class LogLevel
{
public:
    enum ENUMS
    {
        None = -1,
        Begin = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,

        Crash = 5,
        Net = 6,
        NetDebug = 7,
        NetInfo = 8,
        NetWarn = 9,
        NetError = 10,
        NetTrace = 11,
        Custom = 12,

        Sys = 13,            // 系统初始化情况等,系统信息打印
        MemMonitor = 14,     // 内存监视器日志

        Trace = 15,         // 追踪
        End,
    };
};

// 由配置生成
struct LogLevelInfo
{
    Int32 _levelId;                 // 配置表
    LibString _levelName;           // "Warn, Info"等
};

// 等级配置各个值枚举
class LogLevelValue
{
public:
    enum
    {
        ENALBLE = 0,        // 是否开启该等级日志
        FILE_ID = 1,        // 该等级日志对应的日志文件id（配置中定义）
        ENABLE_CONSOLE = 2, // 是否允许控制台显示
        FG_BG_COLOR = 3,    // 配置控制台前景色|背景色
        LEVEL_ID = 4,       // 配置日志等级的等级id
        ENABLE_REAL_TIME = 5,   // 开启实时着盘
        PRINT_STACK_TRACE_BACK = 6,   // 打印堆栈
    };
};

KCP_CPP_END

#endif