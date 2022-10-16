
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIME_SLICE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_TIME_SLICE_H__

#pragma once

#include "kcp/common/common.h"

KCP_CPP_BEGIN

class LibString;

class TimeSlice
{
public:
    TimeSlice();

    /**
     * Construct by slice, in seconds.
     * @param[in] seconds      - the slice seconds part.
     * @param[in] milliSeconds - the slice milli-seconds part.
     * @param[in] microSeconds - the slice micro-seconds part.
     */
    explicit TimeSlice(int seconds, Int64 milliSeconds = 0, Int64 microSeconds = 0);

    /**
     * Construct by slice string representation(fmt: 00:00:00.xxxxxx).
     * @param[in] slice - the slice value string representation, fmt: 00:00:00.xxxxx, the micro-seconds is optional.
     */
    TimeSlice(const LibString &fmtSlice);

    /**
     * Copy constructor.
     */
    TimeSlice(const TimeSlice &slice);

    /**
     * Time slice parts constructor.
     * @param[in] days         - the days part.
     * @param[in] hours        - the hours part.
     * @param[in] minutes      - the minutes part.
     * @param[in] seconds      - the seconds part.
     * @param[in] milliSeconds - the milli-seconds part, default is 0.
     * @param[in] microSeconds - the micro-seconds part, default is 0.
     */
    TimeSlice(int days, int hours, int minutes, Int64 seconds, Int64 milliSeconds = 0, Int64 microSeconds = 0);

    /**
     * Destructor.
     */
    ~TimeSlice();

    /**
     * Get days/hours/minutes/seconds/milli-seconds/micro-seconds.
     * @return int - the time slice parts value. slice 的一部分
     */
    int GetDays() const;
    int GetHours() const;
    int GetMinutes() const;
    int GetSeconds() const;
    int GetMilliSeconds() const;
    int GetMicroSeconds() const;

    int GetTotalDays() const;
    int GetTotalHours() const;
    int GetTotalMinutes() const;
    int GetTotalSeconds() const;
    Int64 GetTotalMilliSeconds() const;
    const Int64 &GetTotalMicroSeconds() const;

    TimeSlice operator +(const TimeSlice &slice) const;
    TimeSlice operator -(const TimeSlice &slice) const;

    TimeSlice &operator +=(const TimeSlice &slice);
    TimeSlice &operator -=(const TimeSlice &slice);

    bool operator ==(const TimeSlice &slice) const;
    bool operator !=(const TimeSlice &slice) const;
    bool operator <(const TimeSlice &slice)const;
    bool operator >(const TimeSlice &slice) const;
    bool operator <=(const TimeSlice &slice) const;
    bool operator >=(const TimeSlice &slice) const;

    TimeSlice &operator =(const TimeSlice &slice);
    TimeSlice &operator =(Int64 microSecSlice);

    LibString ToString() const;

private:
    friend class LibTime;

    TimeSlice(const Int64 &slice);

private:
    Int64 _slice;
};

KCP_CPP_END

#include <kcp/TimeSliceImpl.h>

#endif
