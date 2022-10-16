
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_END_POINT_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_END_POINT_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>
#include "kcp/net/NetDefs.h"

KCP_CPP_BEGIN

union EndPointAddrIn
{
    sockaddr_in _v4;
    sockaddr_in6 _v6;
};

struct EndPoint
{
    EndPoint(const std::string &addr, UInt16 port) :_addr(addr), _port(port)
    {
        _isIpv6 = IsIpv6(_addr);
        UpdateAddrIn();
    }
    EndPoint(const LibString &addr, UInt16 port) :_addr(addr), _port(port)
    {
        _isIpv6 = IsIpv6(_addr);
        UpdateAddrIn();
    }

    // addrPort:xxxx:port格式
    EndPoint(const LibString &addrPort);
    EndPoint(SOCKADDR * addr);
    EndPoint()
    {
        _port = 0;
        _isIpv6 = false;
        ClearAddrInfo();
    }

    // 检测n是16位以内
    static bool is_valid_port_number(unsigned long n)
    {
        return n < 1 << 16;
    }

    static bool IsIpv6(const LibString &ip)
    {
        // 普通ipv6格式:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx
        if(UNLIKELY(ip.empty()))
            return false;

        struct sockaddr_in6 in6; 
        return ::inet_pton(AF_INET6, ip.c_str(), &in6.sin6_addr) == 1;
    }

    static LibString BuildAddrPortString(const EndPoint &endporit);

    void ClearAddrInfo()
    {
        ::memset(&_addrInfo, 0, sizeof(_addrInfo));
    }

    void UpdateAddr(const LibString &ip, UInt16 port)
    {
        _addr = ip;
        _port = port;
        _isIpv6 = IsIpv6(_addr);
    }

    void UpdateRemoteAddrIn(Int32 family, SOCKET sock);
    void UpdateAddrIn();
    void UpdateAddrIn(bool isIpv6, SOCKET sock);
    bool UpdateAddrByAddrIn(bool isIpV6);

    bool operator==(const EndPoint &other) const
    {
        if(this == &other)
            return true;

        if(_addr != other._addr)
            return false;

        if(_port != other._port)
            return false;

        return true;
    }

    bool operator!=(const EndPoint &other) const
    {
        if(*this == other)
            return false;

        return true;
    }

    LibString ToString() const;

    LibString _addr;
    UInt16 _port;
    bool _isIpv6;

    EndPointAddrIn _addrInfo;
};

KCP_CPP_END

#endif
