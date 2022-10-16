#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_SOCKET_STATUS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_SOCKET_STATUS_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

///Represent the status of a socket as returned by a socket operation (send, received). Implicitly convertible to bool
struct SocketStatus
{
    ///Enumeration of socket status, with a 1 byte footprint
    enum VALUES : Byte8 
    {
        errored							= 0x0,
        valid							= 0x1,
        cleanly_disconnected			= 0x2,
        non_blocking_would_have_blocked = 0x3,
        timed_out						= 0x4
        /* ... any other info on a "still valid socket" goes here ... */
    };

    SocketStatus():_value(errored){ }
    explicit SocketStatus(bool state) :_value(state ? valid : errored) { }
    SocketStatus(VALUES value) :_value(value) { }
    SocketStatus(const SocketStatus &) = default;
    SocketStatus(SocketStatus &&) = default;

    ///implicitly convert this object to const bool (as the status should not change)
    operator bool() const { return _value > 0; }
    bool operator==(VALUES v){ return v == _value; }
    int8_t GetValue(){ return _value; }

    ///Actual value of the socket_status.
    const VALUES _value;
};

KCP_CPP_END

#endif

