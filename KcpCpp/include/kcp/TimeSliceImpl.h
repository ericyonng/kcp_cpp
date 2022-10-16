
#ifdef __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIME_SLICE_H__

#include <kcp/LibTime.h>

KCP_CPP_BEGIN

inline TimeSlice::TimeSlice()
    :_slice(0)
{

}

inline TimeSlice::TimeSlice(int seconds, Int64 milliSeconds /*= 0*/, Int64 microSeconds /*= 0*/)
{
    _slice = seconds * TimeDefs::MICRO_SECOND_PER_SECOND +
        milliSeconds * TimeDefs::MICRO_SECOND_PER_MILLI_SECOND + microSeconds;
}

inline TimeSlice::TimeSlice(const TimeSlice &slice)
    :_slice(slice._slice)
{
}

inline TimeSlice::TimeSlice(int days, int hours, int minutes, Int64 seconds, Int64 milliSeconds /* = 0 */, Int64 microSeconds /* = 0 */)
{
    _slice = static_cast<Int64>((((((days * 24) + hours) * 60) + minutes) * 60) + seconds) *
        TimeDefs::MICRO_SECOND_PER_SECOND + milliSeconds * TimeDefs::MICRO_SECOND_PER_MILLI_SECOND + microSeconds;
}

inline TimeSlice::~TimeSlice()
{
}

inline int TimeSlice::GetDays() const
{
    return static_cast<int>(_slice / TimeDefs::MICRO_SECOND_PER_DAY);
}

inline int TimeSlice::GetHours() const
{
    return static_cast<int>((_slice % TimeDefs::MICRO_SECOND_PER_DAY) /
                            TimeDefs::MICRO_SECOND_PER_HOUR);
}

inline int TimeSlice::GetMinutes() const
{
    return static_cast<int>((_slice % TimeDefs::MICRO_SECOND_PER_HOUR) /
                            TimeDefs::MICRO_SECOND_PER_MINUTE);
}

inline int TimeSlice::GetSeconds() const
{
    return static_cast<int>((_slice % TimeDefs::MICRO_SECOND_PER_MINUTE) /
                            TimeDefs::MICRO_SECOND_PER_SECOND);
}

inline int TimeSlice::GetMilliSeconds() const
{
    return static_cast<int>((_slice % TimeDefs::MICRO_SECOND_PER_SECOND) /
                            TimeDefs::MICRO_SECOND_PER_MILLI_SECOND);
}

inline int TimeSlice::GetMicroSeconds() const
{
    return static_cast<int>(_slice % TimeDefs::MICRO_SECOND_PER_MILLI_SECOND);
}

inline int TimeSlice::GetTotalDays() const
{
    return static_cast<int>(_slice / TimeDefs::MICRO_SECOND_PER_DAY);
}

inline int TimeSlice::GetTotalHours() const
{
    return static_cast<int>(_slice / TimeDefs::MICRO_SECOND_PER_HOUR);
}

inline int TimeSlice::GetTotalMinutes() const
{
    return static_cast<int>(_slice / TimeDefs::MICRO_SECOND_PER_MINUTE);
}

inline int TimeSlice::GetTotalSeconds() const
{
    return static_cast<int>(_slice / TimeDefs::MICRO_SECOND_PER_SECOND);
}

inline Int64 TimeSlice::GetTotalMilliSeconds() const
{
    return static_cast<Int64>(_slice / TimeDefs::MICRO_SECOND_PER_MILLI_SECOND);
}

inline const Int64 &TimeSlice::GetTotalMicroSeconds() const
{
    return _slice;
}

inline TimeSlice TimeSlice::operator +(const TimeSlice &slice) const
{
    return TimeSlice(_slice + slice._slice);
}

inline TimeSlice TimeSlice::operator -(const TimeSlice &slice) const
{
    return TimeSlice(_slice - slice._slice);
}

inline TimeSlice &TimeSlice::operator +=(const TimeSlice &slice)
{
    _slice += slice._slice;
    return *this;
}

inline TimeSlice &TimeSlice::operator -=(const TimeSlice &slice)
{
    _slice -= slice._slice;
    return *this;
}

inline bool TimeSlice::operator ==(const TimeSlice &slice) const
{
    return _slice == slice._slice;
}

inline bool TimeSlice::operator !=(const TimeSlice &slice) const
{
    return !(*this == slice);
}

inline bool TimeSlice::operator <(const TimeSlice &slice) const
{
    return _slice < slice._slice;
}

inline bool TimeSlice::operator >(const TimeSlice &slice) const
{
    return _slice > slice._slice;
}

inline bool TimeSlice::operator <=(const TimeSlice &slice) const
{
    return _slice <= slice._slice;
}

inline bool TimeSlice::operator >=(const TimeSlice &slice) const
{
    return _slice >= slice._slice;
}

inline TimeSlice &TimeSlice::operator =(const TimeSlice &slice)
{
    _slice = slice._slice;
    return *this;
}

inline TimeSlice &TimeSlice::operator =(Int64 microSecSlice)
{
    _slice = microSecSlice;
    return *this;
}

inline LibString TimeSlice::ToString() const
{
    int days = GetDays();
    if(days != 0)
        return LibString().AppendFormat("%d %02d:%02d:%02d.%06d",
                                    days, GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
    else
        return LibString().AppendFormat("%02d:%02d:%02d.%06d",
                                    GetHours(), GetMinutes(), GetSeconds(), GetMilliSeconds() * 1000 + GetMicroSeconds());
}

inline TimeSlice::TimeSlice(const Int64 &slice)
:_slice(slice)
{

}

KCP_CPP_END

#endif
