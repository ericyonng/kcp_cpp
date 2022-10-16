#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_LIB_SOCKET_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_LIB_SOCKET_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/net/EndPoint.h>
#include <kcp/net/SocketStatus.h>
#include <kcp/log.h>
#include "kcp/net/NetFuncs.h"
#include "kcp/LibStream.h"

KCP_CPP_BEGIN

class LibStream;

enum class SockProtocolType
{
    TCP,
    UDP,
};

template<SockProtocolType sockProtocol>
class LibSocket
{
public:
    ///Represent a number of bytes with a status information. Some of the methods of this class returns this.
    using bytes_with_status = std::tuple<size_t, SocketStatus>;

public:
    LibSocket(){}

    ///socket<> isn't copyable
    LibSocket(const LibSocket&) = delete;
    ///socket<> isn't copyable
    LibSocket& operator=(const LibSocket&) = delete;
    ~LibSocket();

    bool operator==(const LibSocket& other) const;
    bool IsValid() const;
    inline operator bool() const { return IsValid(); }

    #pragma region // socket 设置
    Int32 Create(Int32 af = AF_INET, Int32 type = SOCK_DGRAM, Int32 protocol = (sockProtocol == SockProtocolType::UDP) ? IPPROTO_UDP : IPPROTO_TCP);
    void SetNonBlocking(bool state = true) const;
    void SetBroadcast(bool state = true) const;
    void SetTcpNodelay(bool state = true) const;
    void SetReuseAddr(bool state = true) const;
    #pragma endregion

    #pragma region // socket 通信相关操作
    ///Bind socket locally using the address and port of the endpoint
    void Bind(Int32 af, const LibString &ip, UInt16 port);
    void Bind(Int32 af, sockaddr *addr);
    ///Join a multicast group
    void Join(const EndPoint &multiCastEndPoint, const LibString &interfaceStr = "");
    ///(For TCP) connect to the endpoint as client
    SocketStatus Connect(const LibString &ip, UInt16 port, Int64 timeout = 0);
    ///(for TCP= setup socket to listen to connection. Need to be called on binded socket, before being able to accept()
    void Listen();
    ///(for TCP) Wait for incoming connection, return socket connect to the client. Blocking.
    LibSocket Accept();
    UInt64 Send(LibStream &stream, KCP_CPP_NS::EndPoint *endpoint, Int32 &errCode);
    UInt64 Recv(LibStream &stream, EndPoint *addrCollection, Int32 &errCode);
    void Close();
    void Shutdown();
    #pragma endregion

    #pragma region // 获取相关属性
    SocketStatus GetStatus() const;
    EndPoint GetLocalEndPoint() const { return _localEndPoint;}
    EndPoint GetRemoteEndPoint() const { return _remoteEndPoint; }
    UInt64 GetAvailableReadBytesInsideSocket() const;
    SockProtocolType GetProtocolType()const;
    SOCKET GetSock() const;
    LibString ToString() const;
    #pragma endregion

private:
    Int32 _af = AF_INET;
    ///operatic-system type for a socket object
    SOCKET _sock = INVALID_SOCKET;

    ///Location where this socket is bound
    EndPoint _localEndPoint;
    EndPoint _remoteEndPoint;
    bool _connected = false;
};

template<SockProtocolType sockProtocol>
inline LibSocket<sockProtocol>::~LibSocket()
{
    Close();
}

template<SockProtocolType sockProtocol>
inline bool LibSocket<sockProtocol>::operator==(const LibSocket<sockProtocol>& other) const
{
    return _sock == other._sock;
}

template<SockProtocolType sockProtocol>
inline bool LibSocket<sockProtocol>::IsValid() const
{
    return _sock != INVALID_SOCKET;
}

template<SockProtocolType sockProtocol>
inline Int32 LibSocket<sockProtocol>::Create(Int32 af, Int32 type, Int32 protocol)
{
    if(_sock != INVALID_SOCKET)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("sock is created before."));
        return Status::Error;
    }

    _sock = ::socket(af, type, protocol);
    if(_sock == INVALID_SOCKET)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("create sock fail ."));
        return Status::Error;
    }

    _af = af;

    return Status::Success;
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::SetNonBlocking(bool state) const
{
#ifdef _WIN32
            u_long set = state ? 1 : 0;
			if (::ioctlsocket(_sock, FIONBIO, &set) < 0)
#else
            const auto flags	= ::fcntl(_sock, F_GETFL, 0);
            const auto newflags = state ? flags | O_NONBLOCK : flags ^ O_NONBLOCK;
            if (fcntl(_sock, F_SETFL, newflags) < 0)
#endif
        g_Log->Error(LOGFMT_OBJ_TAG("setting socket to nonblock returned an error"));
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::SetBroadcast(bool state) const
{
    const int broadcast = state ? 1 : 0;
    if (::setsockopt(_sock, SOL_SOCKET, SO_BROADCAST, reinterpret_cast<const char*>(&broadcast), sizeof(broadcast)) != 0)
        g_Log->Error(LOGFMT_OBJ_TAG("setsockopt broadcast fail"));
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::SetTcpNodelay(bool state) const
{
    if (sockProtocol == SockProtocolType::TCP)
    {
        const int tcpnodelay = state ? 1 : 0;
        if (::setsockopt(_sock, SOL_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&tcpnodelay), sizeof(tcpnodelay)) != 0)
            g_Log->Error(LOGFMT_OBJ_TAG("setting socket tcpnodelay mode returned an error"));
    }
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::SetReuseAddr(bool state) const
{
    int flag = state ? 1 : 0;
    Int32 len = sizeof(Int32);

#ifndef _WIN32
    if (::setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, 
        reinterpret_cast<const char *>(&flag), len) != 0)
    {
        Int32 err = errno;
        g_Log->Warn(LOGFMT_OBJ_TAG(KCP_CPP_NS::LibSocket<sockProtocol>, "setsockopt fail handle:%d, level:%d, optname:%d, err:%d")
                    , _sock, SOL_SOCKET, SO_REUSEADDR, err);
        return;
    }

    return;
#else
    if (::setsockopt(_sock, SOL_SOCKET, SO_REUSEADDR, 
        reinterpret_cast<const char *>(&flag), len) == SOCKET_ERROR)
    {
        Int32 err = ::WSAGetLastError();
        g_Log->Warn(LOGFMT_OBJ_TAG("setsockopt fail handle:%d, level:%d, optname:%d, err:%d")
                    , _sock, SOL_SOCKET, SO_REUSEADDR, err);
        return;
    }
#endif
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::Bind(Int32 af, const LibString &ip, UInt16 port)
{
    if(af == AF_INET)
    {
        sockaddr_in sin;
        sin.sin_family = af;
        sin.sin_port = ::htons(port);
        #ifdef _WIN32
            if(ip.empty())
            {
                sin.sin_addr.s_addr = INADDR_ANY;
            }
            else
            {
                // 比较新的函数对比 inet_addr
                ::inet_pton(sin.sin_family, ip.c_str(), &(sin.sin_addr));
            }
        #else
            if(ip.empty()) 
            {
                sin.sin_addr.s_addr = INADDR_ANY;
            }
            else 
            {
                ::inet_pton(sin.sin_family, ip.c_str(), &(sin.sin_addr.s_addr));
            }
        #endif

        Int32 ret = ::bind(_sock, (sockaddr *)(&sin), sizeof(sin));
        #ifdef _WIN32
            if(ret == SOCKET_ERROR)
        #else
            if(-1 == ret)
        #endif
            {
                auto lastErr = GetErrorCode();
                g_Log->Error(LOGFMT_OBJ_TAG("bind fail sock:%d ip:%s, port:%hu, lastErr:%d"), _sock, ip.c_str(), port, lastErr);
                return;
            }

        _localEndPoint.ClearAddrInfo();
        _localEndPoint.UpdateAddrIn(false, _sock);
    }
    else
    {
        sockaddr_in6 sin;
        sin.sin6_family = af;
        sin.sin6_port = ::htons(port);
        if(ip.empty()) 
        {
            sin.sin6_addr = in6addr_any;
        }
        else 
        {
            ::inet_pton(sin.sin6_family, ip.c_str(), &(sin.sin6_addr));
        }

        Int32 ret = ::bind(_sock, (sockaddr *)(&sin), sizeof(sin));
        #ifdef _WIN32
            if(ret == SOCKET_ERROR)
        #else
            if(-1 == ret)
        #endif
            {
                auto lastErr = GetErrorCode();
                g_Log->Error(LOGFMT_OBJ_TAG("bind fail sock:%d ip:%s, port:%hu, lastErr:%d"), _sock, ip.c_str(), port, lastErr);
                return;
            }

        _localEndPoint.ClearAddrInfo();
        _localEndPoint.UpdateAddrIn(true, _sock);
    }

    // _localEndPoint.UpdateAddr(ip, port);
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::Bind(Int32 af, sockaddr *addr)
{
    // 解析
    Byte8 ip[256] = {0};
    UInt16 port = 0;
    if(af == AF_INET)
    {
        sockaddr_in *sin = LibCast<sockaddr_in>(addr);
        if(::inet_ntop(af, &(sin->sin_addr.s_addr), ip, sizeof(ip)) == NULL)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("parse ip from sockaddr_in fail."));
            return;
        }

        port = ::ntohs(sin->sin_port);
        
        Int32 ret = ::bind(_sock, addr, sizeof(*sin));
        #ifdef _WIN32
            if(ret == SOCKET_ERROR)
        #else
            if(-1 == ret)
        #endif
            {
                auto lastErr = GetErrorCode();
                g_Log->Error(LOGFMT_OBJ_TAG("bind fail sock:%d ip:%s, port:%hu, lastErr:%d"), _sock, ip.c_str(), port, lastErr);
                return Status::Error;
            }
    }
    else
    {
        sockaddr_in6 *sin = LibCast<sockaddr_in6>(addr);
        if(::inet_ntop(af, &(sin->sin6_addr), ip, sizeof(ip)) == NULL)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("parse ip from sockaddr_in6 fail."));
            return;
        }

        port = ::ntohs(sin.sin6_port);
    }

    // 绑定
    Int32 ret = ::bind(_sock, (sockaddr *)(&sin), sizeof(sin));
    #ifdef _WIN32
        if(ret == SOCKET_ERROR)
    #else
        if(-1 == ret)
    #endif
        {
            auto lastErr = GetErrorCode();
            g_Log->Error(LOGFMT_OBJ_TAG("bind fail sock:%d ip:%s, port:%hu, lastErr:%d"), _sock, ip.c_str(), port, lastErr);
            return Status::Error;
        }

    // 更新本地缓存
    _localEndPoint.ClearAddrInfo();
    if(af == AF_INET)
    {
        _localEndPoint.UpdateAddrIn(false, _sock);
        // _localEndPoint._addrInfo._v4 = *LibCast<sockaddr_in>(addr);
    }
    else
    {
        // _localEndPoint._addrInfo._v6 = *LibCast<sockaddr_in6>(addr);
        _localEndPoint.UpdateAddrIn(true, _sock);
    }
   //  _localEndPoint.UpdateAddr(LibString(ip), port);
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::Join(const EndPoint &multiCastEndPoint, const LibString &interfaceStr)
{
    if (sockProtocol != SockProtocolType::UDP)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("joining a multicast is only possible in UDP mode"));
        return;
    }

    addrinfo *multicastAddr;
    addrinfo *localAddr;
    addrinfo  hints = {0};
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags  = AI_NUMERICHOST;
    if (::getaddrinfo(multiCastEndPoint.address.c_str(), nullptr, &hints, &multicastAddr) != 0)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("getaddrinfo FAILED."));
        return;
    }

    hints.ai_family   = multicastAddr->ai_family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE;
    if (::getaddrinfo(nullptr, std::to_string(multiCastEndPoint.port).c_str(), &hints, &localAddr) != 0)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("getaddrinfo FAILED."));
        return;
    }

    _sock = ::socket(localAddr->ai_family, localAddr->ai_socktype, localAddr->ai_protocol);
    if (_sock != INVALID_SOCKET)
    {
        // _socketAddrInfo = localAddr;
    } else {
        g_Log->Error(LOGFMT_OBJ_TAG("syscall_socket() failed"));
        ::freeaddrinfo(multicastAddr);
        return;
    }

    Bind(localAddr->ai_family, localAddr->ai_addr);

    //IPv4
    if (multicastAddr->ai_family  == PF_INET && multicastAddr->ai_addrlen == sizeof(struct sockaddr_in))
    {
        struct ip_mreq multicastRequest = {0};
        ::memcpy(&multicastRequest.imr_multiaddr,
                &((struct sockaddr_in*)(multicastAddr->ai_addr))->sin_addr,
                sizeof(multicastRequest.imr_multiaddr));

        if (interfaceStr.length()) {
            multicastRequest.imr_interface.s_addr = ::inet_addr(interfaceStr.c_str());;
        } else {
            multicastRequest.imr_interface.s_addr = ::htonl(INADDR_ANY);
        }

        if (::setsockopt(_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &multicastRequest, sizeof(multicastRequest)) != 0)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("setsockopt() failed"));
        }
    }

        //IPv6
    else if (multicastAddr->ai_family  == PF_INET6 && multicastAddr->ai_addrlen == sizeof(struct sockaddr_in6))
    {
        struct ipv6_mreq multicastRequest = {0};
        ::memcpy(&multicastRequest.ipv6mr_multiaddr,
                &((struct sockaddr_in6*)(multicastAddr->ai_addr))->sin6_addr,
                sizeof(multicastRequest.ipv6mr_multiaddr));

        if (interfaceStr.length()) {
            struct addrinfo *reslocal;
            if (getaddrinfo(interfaceStr.c_str(), nullptr, nullptr, &reslocal)){
                kissnet_fatal_error("getaddrinfo() failed\n");
            }
            multicastRequest.ipv6mr_interface = ((sockaddr_in6 *)reslocal->ai_addr)->sin6_scope_id;
            ::freeaddrinfo(reslocal);
        } else {
            multicastRequest.ipv6mr_interface = 0;
        }

        if (::setsockopt(_sock, IPPROTO_IPV6, IPV6_JOIN_GROUP, (char*) &multicastRequest, sizeof(multicastRequest)) != 0)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("setsockopt() failed"));
        }
    }
    else
    {
        g_Log->Error(LOGFMT_OBJ_TAG("unknown AI family."));
    }

    ::freeaddrinfo(multicastAddr);
}

template<SockProtocolType sockProtocol>
inline SocketStatus LibSocket<sockProtocol>::Connect(const LibString &ip, UInt16 port, Int64 timeout)
{
    if(_connected)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("already connected, please check or try to close it then."));
        return SocketStatus::errored;
    }
    
    if(sockProtocol == SockProtocolType::TCP)
    {
        if(timeout > 0)
            SetNonBlocking(true);
    }    

    // 构建地址结构
    union
    {
        sockaddr_in v4;
        sockaddr_in6 v6;
    }cacheAddrIn;
    ::memset(&cacheAddrIn, 0, sizeof(cacheAddrIn));
    auto addrPtr = LibCast<sockaddr>(&cacheAddrIn);

    auto build_sockaddr = [](Int32 af, const LibString &ip, UInt16 port, sockaddr *&finalAddr)
    {
        if (af == AF_INET)
        {
            sockaddr_in sin;
            sin.sin_family = af;
            sin.sin_port = ::htons(port);

#ifdef _WIN32
            if (ip.empty())
            {
                sin.sin_addr.s_addr = INADDR_ANY;
            }
            else
            {
                // 比较新的函数对比 inet_addr
                ::inet_pton(sin.sin_family, ip.c_str(), &(sin.sin_addr));
            }
#else
            if (ip.empty())
            {
                sin.sin_addr.s_addr = INADDR_ANY;
            }
            else
            {
                ::inet_pton(sin.sin_family, ip.c_str(), &(sin.sin_addr.s_addr));
            }
#endif

            *LibCast<sockaddr_in>(finalAddr) = sin;
        }
        else
        {
            sockaddr_in6 sin;
            sin.sin6_family = af;
            sin.sin6_port = ::htons(port);
            if (ip.empty())
            {
                sin.sin6_addr = in6addr_any;
            }
            else
            {
                ::inet_pton(sin.sin6_family, ip.c_str(), &(sin.sin6_addr));
            }

            *LibCast<sockaddr_in6>(finalAddr) = sin;
        }
    };

    build_sockaddr(_af, ip, port, addrPtr);
    
    UInt64 len = (_af == AF_INET) ? sizeof(cacheAddrIn.v4) : sizeof(cacheAddrIn.v6);
    auto error = ::connect(_sock, addrPtr, static_cast<int>(len));
    if(sockProtocol == SockProtocolType::TCP)
    {// tcp才有三次握手
        if(error == SOCKET_ERROR)
        {
            error = GetErrorCode();
            if (error == EWOULDBLOCK || error == EAGAIN || error == EINPROGRESS)
            {
                struct timeval tv;
                tv.tv_sec  = static_cast<long>(timeout / 1000);
                tv.tv_usec = 1000 * static_cast<long>(timeout % 1000);

                fd_set fd_write, fd_except;
                ;
                FD_ZERO(&fd_write);
                FD_SET(_sock, &fd_write);
                FD_ZERO(&fd_except);
                FD_SET(_sock, &fd_except);

                int ret = ::select(static_cast<int>(_sock) + 1, NULL, &fd_write, &fd_except, &tv);
                if (ret == -1)
                    error = GetErrorCode();
                else if (ret == 0)
                    error = ETIMEDOUT;
                else
                {
                    socklen_t errlen = sizeof(error);
                    if (::getsockopt(_sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &errlen) != 0)
                        g_Log->Error(LOGFMT_OBJ_TAG("getting socket error returned an error"));
                }
            }
        }
    }

    if(sockProtocol == SockProtocolType::TCP)
    {
        if (timeout > 0)
            SetNonBlocking(false);
    }

    if (error == 0)
    {
        _remoteEndPoint.ClearAddrInfo();
        // ::memcpy(&_remoteEndPoint._addrInfo, &cacheAddrIn, sizeof(cacheAddrIn));

        _remoteEndPoint.UpdateRemoteAddrIn(_af, _sock);
        // _remoteEndPoint.UpdateAddr(ip, port);
        _connected = true;
        return SocketStatus::valid;
    }
    else
    {
        _connected = false;
        Close();
        g_Log->Error(LOGFMT_OBJ_TAG("connect fail error:%d"), error);
        return SocketStatus::errored;
    }
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::Listen()
{
    if (sockProtocol == SockProtocolType::TCP)
    {
        if (::listen(_sock, SOMAXCONN) == SOCKET_ERROR)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("listen failed"));
        }
    }
}

template<SockProtocolType sockProtocol>
inline LibSocket<sockProtocol> LibSocket<sockProtocol>::Accept()
{
    if (sockProtocol != SockProtocolType::TCP)
        return { INVALID_SOCKET, {} };

    sockaddr_storage socketAddress;
    SOCKET s;
    socklen_t size = sizeof socketAddress;

    if ((s = ::accept(_sock, reinterpret_cast<SOCKADDR*>(&socketAddress), &size)) == INVALID_SOCKET)
    {
        const auto error = GetErrorCode();
        switch (error)
        {
            case EWOULDBLOCK: //if socket "would have blocked" from the call, ignore
            case EINTR:		  //if blocking call got interrupted, ignore;
                return {};
        }

        g_Log->Error(LOGFMT_OBJ_TAG("accept() returned an invalid socket\n"));
    }

    return { s, EndPoint(reinterpret_cast<SOCKADDR*>(&socketAddress)) };
}

template<SockProtocolType sockProtocol>
inline UInt64 LibSocket<sockProtocol>::Send(LibStream &stream, KCP_CPP_NS::EndPoint *endpoint, Int32 &errCode)
{
    Int64 receivedBytes = 0;
    if ((sockProtocol == SockProtocolType::TCP) || (_connected && (sockProtocol == SockProtocolType::UDP)))
    {// tcp 或者已建立连接的udp
        receivedBytes = ::send(_sock, reinterpret_cast<const char*>(stream.GetReadBegin()), static_cast<int>(stream.GetReadableBytes()), 0);
    }
    else if(sockProtocol == SockProtocolType::UDP)
    {
        if (endpoint) {
            auto len = endpoint->_isIpv6 ? sizeof(endpoint->_addrInfo._v6) : sizeof(endpoint->_addrInfo._v4);
            sockaddr* addr_in = endpoint->_isIpv6 ? LibCast<sockaddr>(&endpoint->_addrInfo._v6) : LibCast<sockaddr>(&endpoint->_addrInfo._v4);
            receivedBytes = ::sendto(_sock, reinterpret_cast<const char*>(stream.GetReadBegin()), static_cast<int>(stream.GetReadableBytes()), 0, addr_in, static_cast<int>(len));
        } else {
            auto len = _remoteEndPoint._isIpv6 ? sizeof(_remoteEndPoint._addrInfo._v6) : sizeof(_remoteEndPoint._addrInfo._v4);
            sockaddr *addr_in = _remoteEndPoint._isIpv6 ? LibCast<sockaddr>(&_remoteEndPoint._addrInfo._v6) : LibCast<sockaddr>(&_remoteEndPoint._addrInfo._v4);
            receivedBytes = ::sendto(_sock, reinterpret_cast<const char*>(stream.GetReadBegin()), static_cast<int>(stream.GetReadableBytes()), 0, addr_in, socklen_t(len));
        }
    }

    if (receivedBytes < 0)
    {
        if (GetErrorCode() == EWOULDBLOCK)
        {
            errCode = SocketStatus::non_blocking_would_have_blocked;
            return 0;
        }

        errCode = SocketStatus::errored;
        return 0;
    }

    stream.ShiftReadPos(receivedBytes);
    errCode = SocketStatus::valid;
    return receivedBytes;

}

template<SockProtocolType sockProtocol>
inline UInt64 LibSocket<sockProtocol>::Recv(LibStream &stream, EndPoint *addrCollection, Int32 &errCode)
{
    Int64 receivedBytes = 0;
    if ((sockProtocol == SockProtocolType::TCP) ||(_connected && (sockProtocol == SockProtocolType::UDP)))
    {
        receivedBytes = ::recv(_sock, stream.GetWriteBegin(), static_cast<int>(stream.GetWritableBytes()), 0);
    }
    else if (sockProtocol == SockProtocolType::UDP)
    {// udp接收数据会带地址
        EndPointAddrIn cacheSockIn;

        socklen_t len = _localEndPoint._isIpv6 ? sizeof(_localEndPoint._addrInfo._v6) : sizeof(_localEndPoint._addrInfo._v4);
        sockaddr *addr_in = _localEndPoint._isIpv6 ? LibCast<sockaddr>(&cacheSockIn._v6) : LibCast<sockaddr>(&cacheSockIn._v4);
        receivedBytes = ::recvfrom(_sock, stream.GetWriteBegin(), static_cast<int>(stream.GetWritableBytes()), 0, addr_in, &len); // 注意recvfrom只会填充一个udp包，且大小取缓冲和udp包大小的最小值，发送方udp包应该小于缓冲大小
        if (addrCollection) 
        {
            addrCollection->ClearAddrInfo();
            addrCollection->_addrInfo = cacheSockIn;
            addrCollection->UpdateAddrByAddrIn(_localEndPoint._isIpv6);
        }
    }

    if (receivedBytes < 0)
    {
        const auto error = GetErrorCode();
        if (error == EWOULDBLOCK)
        {
            errCode = SocketStatus::non_blocking_would_have_blocked;
            return 0;
        }
        if (error == EAGAIN)
        {
            errCode = SocketStatus::non_blocking_would_have_blocked;
            return 0;
        }

        errCode = SocketStatus::errored;
        return 0;
    }

    if (receivedBytes == 0)
    {
        errCode = SocketStatus::cleanly_disconnected;
        return receivedBytes;
    }

    stream.ShiftWritePos(receivedBytes);
    errCode = SocketStatus::valid;
    return size_t(receivedBytes);
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::Close()
{
    if(sockProtocol == SockProtocolType::UDP && _connected)
    {// udp连接情况下断开连接需要再次调用connect
        auto remoteEndPoint = _remoteEndPoint;
        if(remoteEndPoint._isIpv6)
        {
            remoteEndPoint._addrInfo._v6.sin6_family = AF_UNSPEC;
        }
        else
        {
            remoteEndPoint._addrInfo._v4.sin_family = AF_UNSPEC;
        }

        auto len = _remoteEndPoint._isIpv6 ? sizeof(_remoteEndPoint._addrInfo._v6) : sizeof(_remoteEndPoint._addrInfo._v4);
        sockaddr *addr_in = _remoteEndPoint._isIpv6 ? LibCast<sockaddr>(&_remoteEndPoint._addrInfo._v6) : LibCast<sockaddr>(&_remoteEndPoint._addrInfo._v4);
        ::connect(_sock, addr_in, static_cast<int>(len));
    }

    _remoteEndPoint = EndPoint();
    _localEndPoint = EndPoint();
    _af = 0;

    if (_sock != INVALID_SOCKET)
        closesocket(_sock);

    _connected = false;
    _sock = INVALID_SOCKET;
}

template<SockProtocolType sockProtocol>
inline void LibSocket<sockProtocol>::Shutdown()
{
    if (_sock != INVALID_SOCKET)
        ::shutdown(_sock, SHUT_RDWR);
}

template<SockProtocolType sockProtocol>
inline SocketStatus LibSocket<sockProtocol>::GetStatus() const
{
    int sockerror	 = 0;
    socklen_t errlen = sizeof(sockerror);
    if (::getsockopt(_sock, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&sockerror), &errlen) != 0)
        g_Log->Error(LOGFMT_OBJ_TAG("getting socket error returned an error"));

    return sockerror == SOCKET_ERROR ? SocketStatus::errored : SocketStatus::valid;
}

template<SockProtocolType sockProtocol>
inline UInt64 LibSocket<sockProtocol>::GetAvailableReadBytesInsideSocket() const
{
    u_long size = 0;
    const auto status		  = ::ioctlsocket(_sock, FIONREAD, &size);

    if (status < 0)
        g_Log->Error(LOGFMT_OBJ_TAG("ioctlsocket status is negative when getting FIONREAD"));

    return size > 0 ? size : 0;
}

template<SockProtocolType sockProtocol>
inline SockProtocolType LibSocket<sockProtocol>::GetProtocolType() const
{
    return sockProtocol;
}

template<SockProtocolType sockProtocol>
inline SOCKET LibSocket<sockProtocol>::GetSock() const
{
    return _sock;
}

template<SockProtocolType sockProtocol>
inline LibString LibSocket<sockProtocol>::ToString() const
{
    LibString info;
    info.AppendFormat("family:%d, sock:%d, connected:%d, local end point:[%s], remote end point:[%s]"
    , _af, _sock, _connected, _localEndPoint.ToString().c_str(), _remoteEndPoint.ToString().c_str());

    return info;
}


using UdpSocket = LibSocket<SockProtocolType::UDP>;
using TcpSocket = LibSocket<SockProtocolType::TCP>;

KCP_CPP_END

#endif