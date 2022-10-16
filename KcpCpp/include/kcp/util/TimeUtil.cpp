
#include <pch.h>
#include <kcp/util/TimeUtil.h>

KCP_CPP_BEGIN


std::atomic<Int32> __g_timezone{0};

void TimeUtil::SetTimeZone()
{
    // 设置成服务器所在时区
#ifndef _WIN32
    tzset();
    time_t now = time(NULL);
    __g_timezone = -1 * static_cast<Int32>(localtime(&now)->tm_gmtoff);         // linux下localtime(&now)->tm_gmtoff是个整数，但是本框架需要取得相反的数，如+8时区，这里需要输出-28800
#else // WIN32
    _tzset();
    __g_timezone = _timezone;
#endif // Non-WIN32
}

// TODO需要验证接口
Int32 TimeUtil::GetTimeZone()
{
    return __g_timezone;
}

Int32 TimeUtil::GetMonthMaxDays(Int32 year, Int32 month)
{
    if(month >= 1 && month <= 7)
    {
        if(month % 2 == 1)
            return 31;
        else if(month == 2)
            return IsLeapYear(year) ? 29 : 28;
        else
            return 30;
    }
    else if(month >= 8 && month <= 12)
    {
        return month % 2 == 0 ? 31 : 30;
    }
    else
    {
        printf("error param");
        return -1;
    }
}

KCP_CPP_END
