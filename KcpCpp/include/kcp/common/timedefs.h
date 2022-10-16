
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_TIMEDEFS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_TIMEDEFS_H__

#pragma once

#include <kcp/common/macro.h>

KCP_CPP_BEGIN

// 解决时间中静态变量在程序startup阶段的初始化依赖问题
class TimeDefs
{
public:
    enum ENUMS : Int64
    {
        // 每天
        HOUR_PER_DAY = 24,
        MINUTE_PER_DAY = 1440,
        SECOND_PER_DAY = 86400,
        MILLI_SECOND_PER_DAY = 86400000,
        MICRO_SECOND_PER_DAY = 86400000000,
        NANO_SECOND_PER_DAY  = 86400000000000,

        // 每小时
        MINUTE_PER_HOUR = 60,
        SECOND_PER_HOUR = 3600,
        MILLI_SECOND_PER_HOUR = 3600000,
        MICRO_SECOND_PER_HOUR = 3600000000,
        NANO_SECOND_PER_HOUR  = 3600000000000,

        // 每分钟
        SECOND_PER_MINUTE = 60,
        MILLI_SECOND_PER_MINUTE = 60000,
        MICRO_SECOND_PER_MINUTE = 60000000,
        NANO_SECOND_PER_MINUTE  = 60000000000,

        // 每秒
        MILLI_SECOND_PER_SECOND = 1000,
        MICRO_SECOND_PER_SECOND = 1000000,
        NANO_SECOND_PER_SECOND  = 1000000000,

        // 每毫秒
        MICRO_SECOND_PER_MILLI_SECOND = 1000,
        NANO_SECOND_PER_MILLI_SECOND  = 1000000,

        // 每微秒
        NANO_SECOND_PER_MICRO_SECOND = 1000,

        // 平台下的精度
        // 不同平台时钟精度 std::chrono::system_clock::now().time_since_epoch().count() 精度问题
        #ifdef _WIN32

        // 在windows下系统时间最小分辨率只能到微妙以下10分位
        RESOLUTION_PER_MICROSECOND = 10,
        
        #else
        
        // linux最小分辨率可以到微妙下千分位即可以精确到纳秒
        RESOLUTION_PER_MICROSECOND = 1000,
        
        #endif

    };
};

KCP_CPP_END

#endif

