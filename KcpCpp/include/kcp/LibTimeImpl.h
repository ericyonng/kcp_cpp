
#ifdef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_TIME_H__

KCP_CPP_BEGIN

#pragma region Inline
inline LibTime LibTime::Now()
{
    return LibTime(TimeUtil::GetMicroTimestamp());
}

inline Int64 LibTime::NowTimestamp()
{
    return TimeUtil::GetMicroTimestamp() / TimeDefs::MICRO_SECOND_PER_SECOND;
    // return std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

inline Int64 LibTime::NowMilliTimestamp()
{
    return TimeUtil::GetMicroTimestamp() / TimeDefs::MICRO_SECOND_PER_MILLI_SECOND;
    // return (std::chrono::system_clock().now().time_since_epoch().count() / LibTime::_resolutionPerMicroSecond) / LibTime::_microSecondPerMilliSecond;
}

inline Int64 LibTime::NowMicroTimestamp()
{
    return TimeUtil::GetMicroTimestamp();
    // syscall(SYS_clock_gettime, CLOCK_MONOTONIC_RAW, &monotonic_time)
    // return (std::chrono::system_clock().now().time_since_epoch().count() / LibTime::_resolutionPerMicroSecond);
}

inline Int64 LibTime::GetSecTimestamp() const
{
    return _rawTime / TimeDefs::MICRO_SECOND_PER_SECOND;
}

inline Int64 LibTime::GetMilliTimestamp() const
{
    return _rawTime / TimeDefs::MICRO_SECOND_PER_MILLI_SECOND;
}

inline Int64 LibTime::GetMicroTimestamp() const
{
    return _rawTime;
}

inline const LibTime &LibTime::UpdateTime()
{
    _rawTime = TimeUtil::GetMicroTimestamp();
    // _rawTime = std::chrono::system_clock::now().time_since_epoch().count() / LibTime::_resolutionPerMicroSecond;
    _UpdateTimeStructs();
    return *this;
}

inline const LibTime &LibTime::UpdateTime(Int64 microSecTime)
{
    _rawTime = microSecTime;
    _UpdateTimeStructs();
    return *this;
}

inline bool LibTime::operator ==(const LibTime &time) const
{
    return _rawTime == time._rawTime;
}

inline bool LibTime::operator ==(const Int64 &microSecondTimestamp) const
{
    return _rawTime == microSecondTimestamp;
}

inline bool LibTime::operator !=(const LibTime &time) const
{
    return _rawTime != time._rawTime;
}

inline bool LibTime::operator <(const LibTime &time) const
{
    return _rawTime < time._rawTime;
}

inline bool LibTime::operator >(const LibTime &time) const
{
    return _rawTime > time._rawTime;
}

inline bool LibTime::operator >=(const LibTime &time) const
{
    return _rawTime >= time._rawTime;
}

inline bool LibTime::operator <=(const LibTime &time) const
{
    return _rawTime <= time._rawTime;
}

inline const tm &LibTime::GetGmtTime() const
{
    return _gmtTimeStruct;
}

inline void LibTime::GetGmtTime(tm &timeStruct) const
{
     memcpy(&timeStruct, &_gmtTimeStruct, sizeof(tm));
}

inline const tm &LibTime::GetLocalTime() const
{
    return _localTimeStruct;
}

inline void LibTime::GetLocalTime(tm &timeStruct) const
{
    memcpy(&timeStruct, &_localTimeStruct, sizeof(tm));
}

inline Int32 LibTime::GetLocalYear() const
{
    return _localTimeStruct.tm_year + 1900;     // since 1900
}

inline Int32 LibTime::GetLocalMonth() const
{
    return _localTimeStruct.tm_mon + 1;     // start by 1
}

inline Int32 LibTime::GetLocalDay() const
{
    return _localTimeStruct.tm_mday;
}

inline Int32 LibTime::GetLocalDayOfWeek() const
{
    return _localTimeStruct.tm_wday;        // start by 0
}

inline Int32 LibTime::GetLocalDayOfYear() const
{
    return _localTimeStruct.tm_yday + 1;    // start by 1
}

inline Int32 LibTime::GetLocalHour() const
{
    return _localTimeStruct.tm_hour;
}

inline Int32 LibTime::GetLocalMinute() const
{
    return _localTimeStruct.tm_min;
}

inline Int32 LibTime::GetLocalSecond() const
{
    return _localTimeStruct.tm_sec;
}

inline Int32 LibTime::GetLocalMilliSecond() const
{
    auto localTime = _rawTime - static_cast<Int64>(TimeUtil::GetTimeZone() * TimeDefs::MICRO_SECOND_PER_SECOND);
    return static_cast<Int32>((localTime % TimeDefs::MICRO_SECOND_PER_SECOND) / TimeDefs::MICRO_SECOND_PER_MILLI_SECOND);
}

inline Int32 LibTime::GetLocalMicroSecond() const
{
    auto microsecond = _rawTime - TimeUtil::GetTimeZone() * TimeDefs::MICRO_SECOND_PER_SECOND;
    return static_cast<Int32>(microsecond % TimeDefs::MICRO_SECOND_PER_SECOND);
}

inline LibString LibTime::Format(time_t timestamp, const Byte8 *outFmt)
{
    return FromSeconds(timestamp).Format(outFmt);
}

inline LibString LibTime::FormatAsGmt(time_t timestamp, const char *outFmt)
{
    return FromSeconds(timestamp).FormatAsGmt(outFmt);
}

KCP_CPP_END

#endif