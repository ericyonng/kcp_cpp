#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_TIME_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_TIME_H__

#pragma once

#include "kcp/common/common.h"
#include <kcp/LibString.h>
#include <kcp/util/TimeUtil.h>

KCP_CPP_BEGIN

class TimeSlice;

class LibTime
{ 
    
public:
    LibTime();
    LibTime(const LibTime &other);
    virtual ~LibTime();

    #pragma region 
    static LibTime Now();
    // 耗时在400ns+
    static Int64 NowTimestamp();            // 
    static Int64 NowMilliTimestamp();       // 
    static Int64 NowMicroTimestamp();       // 
    
    static LibTime FromSeconds(Int64 seconds);
    static LibTime FromMilliSeconds(Int64 milliSeconds);
    static LibTime FromMicroSeconds(Int64 microSeconds);
    static LibTime FromFmtString(const LibString &fmt);    // fmt：1970-07-01 12:12:12.000000055 
    static LibTime FromTimeMoment(int year, int month, int day, int hour, int minute, int second, int milliSecond = 0, int microSecond = 0);
    static LibTime FromTimeStruct(const tm &timeStruct, int milliSecond = 0, int microSecond = 0);
    #pragma endregion

    #pragma region 
    Int64 GetSecTimestamp() const;
    Int64 GetMilliTimestamp() const;
    Int64 GetMicroTimestamp() const;
    #pragma endregion

    #pragma region 
    const LibTime &UpdateTime();
    const LibTime &UpdateTime(Int64 microSecTime);
    const LibTime &UpdateAppendTime(const TimeSlice &addSliceBaseOnNowTime);
    const LibTime &UpdateAppendTime(Int64 addMicroSecBaseOnNowTime);

    bool operator ==(const LibTime &time) const;
    bool operator ==(const Int64 &microSecondTimestamp) const;
    bool operator !=(const LibTime &time) const;
    bool operator <(const LibTime &time) const;
    bool operator >(const LibTime &time) const;
    bool operator <=(const LibTime &time) const;
    bool operator >=(const LibTime &time) const;

    LibTime &operator =(const LibTime &other);
    #pragma endregion

    #pragma region 
    /**
     * Get current time of day.
     * @return TimeSlice - the current time of day.
     */
    TimeSlice GetTimeOfDay() const;
    /**
     * Get remaining seconds to nearest day special monent.
     * @param[in] slice        - slice value.
     * @param[in] hour        - hour.
     * @param[in] minute      - minute.
     * @param[in] second      - second.
     * @param[in] milliSecond - milli-second.
     * @param[in] microSecond - micro-second.
     * @param[in] from        - from time.
     * @return TimeSlice - timeslice value.
     */
    TimeSlice GetIntervalTo(const TimeSlice &slice) const;    // slice是当天的时刻如：10:10:10.100000的微妙数
    TimeSlice GetIntervalTo(int hour, int minute, int second, int milliSecond = 0, int microSecond = 0) const;
    static TimeSlice GetIntervalTo(const LibTime &from, const TimeSlice &slice);
    static TimeSlice GetIntervalTo(const LibTime &from, int hour, int minute, int second, int milliSecond = 0, int microSecond = 0);

    /**
     * Time slice operations.
     */
    TimeSlice operator -(const LibTime &time) const;

    LibTime operator +(const TimeSlice &slice) const;
    LibTime operator -(const TimeSlice &slice) const;
    #pragma endregion

    #pragma region 
    /**
     * Add specified time parts values.
     * Notes: These operations are thread-safe, all add parts added to new LLBC_Time object.
     * @param[in] <time parts> - the all time parts(year, month, day, ...).
     * @return LLBC_Time - the new time object.
     */
    LibTime AddYears(int years) const;
    LibTime AddMonths(int months) const;
    LibTime AddDays(int days) const;
    LibTime AddHours(int hours) const;
    LibTime AddMinutes(int minutes) const;
    LibTime AddSeconds(int seconds) const;
    LibTime AddMilliSeconds(int milliSeconds) const;
    LibTime AddMicroSeconds(int microSeconds) const;
    
    /**
     * Get GMT time struct.
     * @param[out] timeStruct - time struct object reference.
     * @return const tm & - time struct object.
     */
    const tm &GetGmtTime() const;
    void GetGmtTime(tm &timeStruct) const;

    /**
     * Get local time struct.
     * @param[out] timeStruct - time struct reference.
     * @return const tm & - time struct object.
     */
    const tm &GetLocalTime() const;
    void GetLocalTime(tm &timeStruct) const;

    Int32 GetLocalYear() const; // since 1900
    Int32 GetLocalMonth() const;    // start by 1
    Int32 GetLocalDay() const;
    Int32 GetLocalDayOfWeek() const;    // start by 0
    Int32 GetLocalDayOfYear() const;    // start by 1
    Int32 GetLocalHour() const;
    Int32 GetLocalMinute() const;
    Int32 GetLocalSecond() const;
    Int32 GetLocalMilliSecond() const;
    Int32 GetLocalMicroSecond() const;

    LibTime GetZeroTime() const;

    LibString Format(const Byte8 *outFmt = NULL) const;
    static LibString Format(time_t timestamp, const Byte8 *outFmt);

    LibString FormatAsGmt(const char *outFmt = NULL) const;
    static LibString FormatAsGmt(time_t timestamp, const char *outFmt);
    
    /**
     * Get the time object string representation.
     * @return FS_String - the object string representation.
     */
    LibString ToString() const;
    LibString ToStringOfMillSecondPrecision() const;
    #pragma endregion

    #pragma region 
private:
    explicit LibTime(Int64 microSecTimestamp);
    // explicit LibTime(const std::chrono::system_clock::time_point &now);
    void _UpdateTimeStructs();
    #pragma endregion

private:
    Int64 _rawTime{0};          // microsecond ()
    tm _gmtTimeStruct{0};       // 
    tm _localTimeStruct{0};     // 
};

KCP_CPP_END

#include <kcp/LibTimeImpl.h>

#endif
