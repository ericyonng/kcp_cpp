#include "pch.h"
#include <kcp/net/EndPoint.h>
#include <kcp/log.h>
#include <kcp/util/StringUtil.h>

KCP_CPP_BEGIN

EndPoint::EndPoint(const LibString &addrPort)
{
    const auto &sepStrs = addrPort.Split(":");
    if(sepStrs.size() < 2)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("addr port format error:%s"), addrPort.c_str());
        return;
    }

    _addr = sepStrs[0];
    auto port = StringUtil::StringToUInt16(sepStrs[1].c_str());
    if(!is_valid_port_number(port))
    {
        g_Log->Error(LOGFMT_OBJ_TAG("error port in addr prot format string:%s"), addrPort.c_str());
        return;
    }

    _port = port;
    _isIpv6 = IsIpv6(_addr);

    UpdateAddrIn(); 
}

EndPoint::EndPoint(SOCKADDR * addr)
{
    switch (addr->sa_family)
    {
        case AF_INET: 
        {
            auto ip_addr = (SOCKADDR_IN*)(addr);
            _addr		 = ::inet_ntoa(ip_addr->sin_addr);
            _port		 = ::ntohs(ip_addr->sin_port);
        }
        break;
        case AF_INET6: 
        {
            auto ip_addr = (sockaddr_in6*)(addr);
            char buffer[INET6_ADDRSTRLEN];
            _addr = ::inet_ntop(AF_INET6, &(ip_addr->sin6_addr), buffer, INET6_ADDRSTRLEN);
            _port	= ::ntohs(ip_addr->sin6_port);
        }
            break;

        default: 
        {
            g_Log->Error(LOGFMT_OBJ_TAG("Trying to construct an endpoint for a protocol familly that is neither AF_INET or AF_INET6"));
        }
    }

    if (_addr.empty())
        g_Log->Error(LOGFMT_OBJ_TAG("Couldn't construct endpoint from sockaddr(_storage) struct"));

    _isIpv6 = addr->sa_family == AF_INET6;

    UpdateAddrIn();
}

bool EndPoint::UpdateAddrByAddrIn(bool isIpV6)
{
    Byte8 ip[256] = {0};
    UInt16 port = 0;
    if(isIpV6)
    {
        if(::inet_ntop(AF_INET6, &(_addrInfo._v6.sin6_addr), ip, sizeof(ip)) == NULL)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("parse ip from sockaddr_in6 fail."));
            return false;
        }

        port = ::ntohs(_addrInfo._v6.sin6_port);
    }
    else
    {
        if(::inet_ntop(AF_INET, &(_addrInfo._v4.sin_addr.s_addr), ip, sizeof(ip)) == NULL)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("parse ip from sockaddr_in fail."));
            return false;
        }

        port = ::ntohs(_addrInfo._v4.sin_port);
    }

    _addr = ip;
    _port = port;
    _isIpv6 = isIpV6;

    return true;
}

void EndPoint::UpdateAddrIn()
{
    if(!_isIpv6)
    {
        sockaddr_in sin;
        sin.sin_family = AF_INET;
        sin.sin_port = ::htons(_port);

        #ifdef _WIN32
            if(_addr.empty())
            {
                sin.sin_addr.s_addr = INADDR_ANY;
            }
            else
            {
                // 比较新的函数对比 inet_addr
                ::inet_pton(sin.sin_family, _addr.c_str(), &(sin.sin_addr));
            }
        #else
            if(_addr.empty()) 
            {
                sin.sin_addr.s_addr = INADDR_ANY;
            }
            else 
            {
                ::inet_pton(sin.sin_family, _addr.c_str(), &(sin.sin_addr.s_addr));
            }
        #endif

        ClearAddrInfo();
        _addrInfo._v4 = sin;
    }
    else
    {
        sockaddr_in6 sin;
        sin.sin6_family = AF_INET6;
        sin.sin6_port = ::htons(_port);
        if(_addr.empty()) 
        {
            sin.sin6_addr = in6addr_any;
        }
        else 
        {
            ::inet_pton(sin.sin6_family, _addr.c_str(), &(sin.sin6_addr));
        }

        ClearAddrInfo();
        _addrInfo._v6 = sin;
    }
}

template<typename SockAddrInType>
static inline Int32 GetSockName(SOCKET sock, SockAddrInType &sockAddr, Int32 &lastError)
{
    if(!sock || sock == INVALID_SOCKET)
        return Status::Socket_ParamError;
    
    Int32 addrSize = static_cast<Int32>(sizeof(sockAddr));
    if(::getsockname(sock, (sockaddr *)(&sockAddr), reinterpret_cast<socklen_t *>(&addrSize)) != 0)
    {
#ifdef _WIN32
        lastError = WSAGetLastError();
#else
        lastError = errno;
#endif
        return Status::IPUtil_GetSockNameFailed;
    }

    return Status::Success;
}

static inline bool GetAddrInfoFromNetInfo(sockaddr_in &addrObj, UInt64 szip, Byte8 *&ip, UInt16 &port)
{
    if(::inet_ntop(addrObj.sin_family, &addrObj.sin_addr.s_addr, ip, szip) == NULL)
        return false;

    port = ntohs(addrObj.sin_port);

    return true;
}


static inline bool GetAddrInfoFromNetInfo(sockaddr_in6 &addrObj, UInt64 szip, Byte8 *&ip, UInt16 &port)
{
    if(::inet_ntop(addrObj.sin6_family, &addrObj.sin6_addr, ip, szip) == NULL)
        return false;

    port = ntohs(addrObj.sin6_port);

    return true;
}


static Int32 GetPeerAddr(UInt16 family, UInt64 sSocket, Int32 sizeIp, Byte8 *&ip, UInt16 &port, Int32 &lastError)
{
    if(!sSocket || sSocket == INVALID_SOCKET)
        return Status::Socket_ParamError;

    //定义
    const bool isIpv4 = family == AF_INET;
    EndPointAddrIn addr_in;
    ::memset(&addr_in, 0, sizeof(addr_in));
    socklen_t  len = 0;
    if(isIpv4)
    {
        len = sizeof(addr_in._v4);
    }
    else
    {
        len = sizeof(addr_in._v6);
    }

    // 获取客户端地址
    if(getpeername(sSocket, (struct sockaddr*)&addr_in, &len) != 0)
    {
#ifdef _WIN32
        lastError = WSAGetLastError();
#else
        lastError = errno;
#endif
        return Status::IPUtil_GetPeerNameFailed;
    }

    if(isIpv4)
    {
        auto &sinv4 = addr_in._v4;
        if(::inet_ntop(family, &sinv4.sin_addr.s_addr, ip, sizeIp) == NULL)
        {
    #ifdef _WIN32
            lastError = WSAGetLastError();
    #else
            lastError = errno;
    #endif
            return Status::Error;
        }

        port = ::ntohs(sinv4.sin_port);
    }
    else
    {
        auto &sinv6 = addr_in._v6;
        if(::inet_ntop(family, &addr_in._v6.sin6_addr, ip, sizeIp) == NULL)
        {
    #ifdef _WIN32
            lastError = WSAGetLastError();
    #else
            lastError = errno;
    #endif
            return Status::Error;
        }
        port = ::ntohs(sinv6.sin6_port);
    }

    return Status::Success;
}

void EndPoint::UpdateRemoteAddrIn(Int32 family, SOCKET sock)
{
    // 远程地址
    BUFFER256 ip = {0};
    Byte8 *ptr = ip;
    Int32 lastErr = 0;
    Int32 errCode = GetPeerAddr(family, sock, sizeof(ip), ptr, _port, lastErr);
    if(errCode != Status::Success)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("sock[%d] get peer addr fail!"), sock);
        return;
    }

    UInt64 ipNetByteOrder[2] = {0};
    _addr = ip;
    lastErr = ::inet_pton(family, ip, &ipNetByteOrder);
    if(lastErr != 1)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG( "inet_pton fail lastErr[%d], family[%hu], ip[%s]"), lastErr, family, ip);
        return;
    }
    if(family == AF_INET)
    {
        auto &sinv4 = _addrInfo._v4;
        sinv4.sin_family = family;
        sinv4.sin_addr.s_addr = static_cast<ULONG>(ipNetByteOrder[0]);
        sinv4.sin_port = ::htons(_port);
    }
    else
    {
        auto &sinv6 = _addrInfo._v6;
        sinv6.sin6_family = family;
        ::memcpy(sinv6.sin6_addr.u.Byte, ipNetByteOrder, sizeof(sinv6.sin6_addr.u.Byte));
        sinv6.sin6_port = ::htons(_port);
    }

    _isIpv6 = family == AF_INET6;
    // g_Log->Debug(LOGFMT_OBJ_TAG("sock[%d] _family[%hu] remote addr info [%s]")
    // ,sock, family, ToString().c_str());    
}

void EndPoint::UpdateAddrIn(bool isIpv6, SOCKET sock)
{
    // 本地地址
    Int32 lastErr = 0;
    Int32 errCode = Status::Success;
    if(!isIpv6)
    {
        errCode = GetSockName(sock, _addrInfo._v4, lastErr);
    }
    else
    {
        errCode = GetSockName(sock, _addrInfo._v6, lastErr);
    }
    if(errCode != Status::Success)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("_sock[%d] GetSockName fail errCode = [%d]!")
                                , sock, errCode);
        return;
    }

    BUFFER256 ip = {0};
    Byte8 *ptr = ip;
    if(!isIpv6)
    {
        if(!GetAddrInfoFromNetInfo(_addrInfo._v4, sizeof(ip), ptr, _port))
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("_sock[%d], GetAddrInfoFromNetInfo fail"), sock);
            return;
        }
    }
    else
    {
        if(!GetAddrInfoFromNetInfo(_addrInfo._v6, sizeof(ip), ptr, _port))
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("_sock[%d], GetAddrInfoFromNetInfo fail"), sock);
            return;
        }
    }

    _addr = ip;
    _isIpv6 = isIpv6;

    // g_Log->Debug(LOGFMT_OBJ_TAG("sock[%d] local addr info[%s]"),sock ,_addr.ToString().c_str());
}

LibString EndPoint::ToString() const
{
    LibString info;
    info.AppendFormat("%s:%hu, is ipv6:%d", _addr.c_str(), _port, _isIpv6);
    return info;
}

LibString EndPoint::BuildAddrPortString(const EndPoint &endporit)
{
    LibString info;
    info.AppendFormat("%s:%hu", endporit._addr.c_str(), endporit._port);
    return info;
}


KCP_CPP_END
