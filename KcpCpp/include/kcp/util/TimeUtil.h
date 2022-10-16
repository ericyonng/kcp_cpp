
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_TIME_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_TIME_UTIL_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

class TimeUtil
{
public:
    // 设置成服务器所在时区 TODO:需要在不同平台下验证
    static void SetTimeZone();
    // 获取服务器时区单位(s)且有符号，如+8区需要输出 +28800s，时区是相对于格林尼治时间的偏移秒数
    static Int32 GetTimeZone();
    static bool IsLeapYear(Int32 year);

    /**
     * Get specific month max days.
     * @param[in] year  - the year.
     * @param[in] month - the month.
     * @return int - the specific month max days, if failed, return 0.
     */
    static Int32 GetMonthMaxDays(Int32 year, Int32 month);

    // 获取不随调时变化的单调递增硬件时钟(系统启动运行的时间),产生系统调用额外开销 只支持linux版本,windows下会直接调用GetMicroTimestamp 单位微妙
    static Int64 GetHandwareSysRunTime();
    // clocktime 会随用户调时而变化,系统调用开销
    static Int64 GetClockRealTime();
    // clocktime 随用户调时而变化 更快精度相对较低的时钟 高性能时钟 千万次调用只话费85ms 精度最差 毫秒级精度
    static Int64 GetClockRealTimeCoarse();
    // clocktime 不会随用户调时而变化的单调递增时钟,有系统调用开销,但会受ntp影响 系统启动运行的时间 单调递增
    static Int64 GetClockMonotonicSysRunTime();
    // 通用时间 除GetClockRealTimeCoarse外最高性能 但调用一次仍然将近400+ns
    static Int64 GetMicroTimestamp();
    // chrono时间
    static Int64 GetChronoMicroTimestamp();

    // 限定只用于测试性能
    static Int64 GetSystemElapseNanoseconds();
    static Int64 GetProcessElapseNanoseconds();
    static Int64 GetThreadElapseNanoseconds();
private:
};

inline bool TimeUtil::IsLeapYear(Int32 year)
{
    return (year % 4 == 0 && year % 100 != 0 || year % 400 == 0);
}

inline Int64 TimeUtil::GetHandwareSysRunTime()
{
#ifdef _WIN32
    return GetMicroTimestamp();
#else
    struct timespec tp;
    ::syscall(SYS_clock_gettime, CLOCK_MONOTONIC_RAW, &tp);

    return (Int64)tp.tv_sec * TimeDefs::MICRO_SECOND_PER_SECOND + tp.tv_nsec / TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
#endif
}

inline Int64 TimeUtil::GetClockRealTime()
{
#ifdef _WIN32
    return GetMicroTimestamp();
#else
    struct timespec tp;
    ::clock_gettime(CLOCK_REALTIME, &tp);

    return (Int64)tp.tv_sec * TimeDefs::MICRO_SECOND_PER_SECOND + tp.tv_nsec / TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
#endif
}

inline Int64 TimeUtil::GetClockRealTimeCoarse()
{
#ifdef _WIN32
    return GetMicroTimestamp();
#else
    struct timespec tp;
    ::clock_gettime(CLOCK_REALTIME_COARSE, &tp);

    return (Int64)tp.tv_sec * TimeDefs::MICRO_SECOND_PER_SECOND + tp.tv_nsec / TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
#endif
}

inline Int64 TimeUtil::GetClockMonotonicSysRunTime()
{
#ifdef _WIN32
    return GetMicroTimestamp();
#else
    struct timespec tp;
    ::clock_gettime(CLOCK_MONOTONIC, &tp);

    return (Int64)tp.tv_sec * TimeDefs::MICRO_SECOND_PER_SECOND + tp.tv_nsec / TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
#endif
}

// 通用时间,高性能
inline Int64 TimeUtil::GetMicroTimestamp()
{
#ifndef _WIN32
    // 第二个参数返回时区
    struct timeval timeVal;
    gettimeofday(&timeVal, NULL);

    return (Int64)timeVal.tv_sec * TimeDefs::MICRO_SECOND_PER_SECOND + timeVal.tv_usec;

#else
    // Get time.
    FILETIME ft;
    ::GetSystemTimeAsFileTime(&ft);

    Int64 timeInMicroSec = ((Int64)ft.dwHighDateTime) << 32;
    timeInMicroSec |= ft.dwLowDateTime;
    timeInMicroSec /= 10;

    return timeInMicroSec - CRYSTAL_EPOCH_IN_USEC;
#endif
}

inline Int64 TimeUtil::GetChronoMicroTimestamp()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock().now().time_since_epoch()).count() / TimeDefs::RESOLUTION_PER_MICROSECOND;
}

inline Int64 TimeUtil::GetSystemElapseNanoseconds()
{
    #ifndef _WIN32
        struct timespec tp;
        ::clock_gettime(CLOCK_MONOTONIC, &tp);

        return (Int64)tp.tv_sec * TimeDefs::NANO_SECOND_PER_SECOND + tp.tv_nsec;   
    #endif

    #ifdef _WIN32
		return GetMicroTimestamp() * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
        // static_assert(false, "windows not support GetSystemElapseNanoseconds");
        // return GetMicroTimestamp() * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
    #endif
}

inline Int64 TimeUtil::GetProcessElapseNanoseconds()
{
    #ifndef _WIN32
        struct timespec tp;
        ::clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp);

        return (Int64)tp.tv_sec * TimeDefs::NANO_SECOND_PER_SECOND + tp.tv_nsec;
    #endif

    #ifdef _WIN32
		return GetMicroTimestamp() * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
		// static_assert(false, "windows not support GetProcessElapseNanoseconds");
        // return GetMicroTimestamp() * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
    #endif
}

inline Int64 TimeUtil::GetThreadElapseNanoseconds()
{
    #ifndef _WIN32
        struct timespec tp;
        ::clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp);

        return (Int64)tp.tv_sec * TimeDefs::NANO_SECOND_PER_SECOND + tp.tv_nsec;
    #endif

    #ifdef _WIN32
		return GetMicroTimestamp() * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
		// static_assert(false, "windows not support GetThreadElapseNanoseconds");
        // return GetMicroTimestamp() * TimeDefs::NANO_SECOND_PER_MICRO_SECOND;
    #endif
}

KCP_CPP_END

#endif
