#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_KCP_CHANNEL_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_KCP_CHANNEL_H__

#pragma once

#include "kcp/kcp_cpp.h"
#include "KcpCommon/NetCommon/IChannel.h"
#include "KcpCommon/NetCommon/ikcp.h"

struct IKCPCB;
class IProtocolCoder;
struct KcpContext;
class IProtocolStack;

class KcpStateFlag
{
public:
    enum ENUMS
    {
        Open = (1 << 0),            // 开起状态
        HalfOpen = 1 << 1,          // 半开状态
        ReadScheduled = 1 << 2,     // 暂时没用
        WriteScheduled = 1 << 3,    // 暂时不用
        Active = 1 << 4,            // 激活状态
        HalfClosed = 1 << 5,        // 半关闭状态
    };

    /*
    * 对于服务端
         1. 创建Channel后状态是：Open | Active
         2. 
    */
};

class KcpFlags
{
public:
    enum ENUMS
    {
        BIT_SYN = 0,        // 第0位表示发起syn请求 一般用于连接请求,如果这一位不设值就表示数据传输
        BIT_ACK = 1,        // 第1位表示应答，一般用于应答,目前仅用于连接成功返回conv数据
        BIT_FIN = 2,        // 第2位表示关闭 暂时不用
        BIT_MSG = 3,        // 表示数据包 暂时不用
        BIT_FLAGS_SIZE = 1, // flag大小 1字节
        BIT_CONV_SIZE = 4,  // conv数据大小 4字节
    };
};

class KcpChannel : public IChannel
{
public:
    enum DIRTY_FLAG
    {
        READ = 0,       // 读脏
        WRITE = 1,      // 写脏
    };

    enum STREAM_CTRL_FLAG
    {
        LAZY_CLOSE = 0,         // 延迟关闭
        FORBID_SEND = 1,        // 禁用发送
        FORBID_RECV = 2,        // 禁用接收
    };

    enum CHANNEL_STATE
    {
        CHANNEL_CREATED = 0,    // 刚创建
        CHANNEL_ACTIVE = 1,     // 激活
    };

public:
    KcpChannel(){}
    ~KcpChannel();

    void Destroy();
    
    UInt64 GetSesssionId() const;
    void SetSessionId(UInt64 sessionId);

    void SetSocket(KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *sock);

    // 监听者
    virtual bool IsListener() const override;
    void SetIsListener(bool isListener);

    virtual SOCKET GetSocketRaw() const override;
    
    // 绑定地址
    void SetBindAddr(const KCP_CPP_NS::LibString &ip, UInt16 port);

    // 设置远端地址
    void SetRemoteAddr(const KCP_CPP_NS::LibString &ip, UInt16 port);

    // 设置接收到网络数据回调(没有经过解码的udp包 因为可能有其他channel的数据)
    void SetNetRawDataComeCallback(KCP_CPP_NS::IDelegate<void, UInt64, KcpContext &> *callback);
    // 设置接收回调
    void SetRcvMsgCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *, KCP_CPP_NS::LibStream &> *callback);
    // 新连接即将连入回调
    void SetWillConnectCallback(KCP_CPP_NS::IDelegate<bool, KcpChannel *, KCP_CPP_NS::EndPoint &> *callback);
    // 设置连接回调
    void SetConnectedCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *> *callback);
    // 设置断开连接回调
    void SetDisconnectedCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *> *callback);
    // 协议栈
    void SetProtocolStack(IProtocolStack *protocolStack);
    void SetKcpOutput(KCP_CPP_NS::IDelegate<int, const char*, int, struct IKCPCB*, KcpChannel*> *outputCallback);
    // 收到远端ACK应答包回调
    void SetSucAckBackCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *> *callback);

    // 协议族
    void SetFamily(Int32 af);
    Int32 GetFamily() const;

    // 缓冲大小
    void SetBufferCapicity(UInt64 bufferCapicity);

    // 启用udp包包头装饰
    void EnableUdpPacketDecorate(bool enable = true);

    // 状态: CHANNEL_STATE
    void Active();
    bool IsActived() const;
    UInt32 GetState() const;

    // 获取conv
    UInt64 GetConv() const;

    KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *GetSock();
    const KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *GetSock() const;

    // 脏标记
    bool HasDirty() const;
    void MaskReadDirty(bool set = true);
    void MaskWriteDirty(bool set = true);
    bool IsDirty(Int32 dirtyType) const;
    void ClearDirty();

    // 流控制
    void MaskClose();
    void ForbidSend();
    void ForbidRecv();
    bool WillClose() const;
    bool CanSend() const;
    bool CanRecv() const;

    const KCP_CPP_NS::LibString& GetRemoteIp() const;
    UInt16 GetRemotePort() const;
    const KCP_CPP_NS::EndPoint& GetRemoteEndpoint() const;
    KCP_CPP_NS::EndPoint& GetRemoteEndpoint();

    bool Send(UInt16 opcode, IProtocolCoder *coder, Int64 packetId = 0);
    int RawSend(KCP_CPP_NS::LibStream& stream);
    bool ShakeHandeWithRemote();
    int OnKcpOutput(const Byte8 *buf, int len);

    #pragma region // 网络操作
    // 监控
    void OnMonitor();
    // 可读事件
    void OnReadable();
    // 网络包数据
    void OnReadable(KCP_CPP_NS::LibStream &stream);
    
    // 可写事件
    void OnWritable();
    // 新连入
    void OnConnected(const KCP_CPP_NS::EndPoint &remoteEndpoint);
    // 断开
    void OnDisconnected();
    // 刷新
    void OnUpdate();
    #pragma endregion

    // 信息
    KCP_CPP_NS::LibString ToString() const;
    
    // 初始化
    Int32 Init(bool isNonBlocking = true);
    Int32 InitKcpCb(UInt64 conv);

    // 初始化始终
    void InitHeartbeatTimer(KCP_CPP_NS::LibTimer* timer);
    void UpdateHeatbeatTimer(Int64 milliseconds);
    bool IsAlive() const;

private:
    void _UpdateHeartbeatCount();
    Int32 _InitKcpCb(UInt64 conv = 0);
    void _ContinueSend();
    void _SendStream(KCP_CPP_NS::LibStream &stream);
    void _OnReadable(KcpContext &ctx);

private:
    Int32 _af = AF_INET;         // 协议族
    bool _isListener = false;   // 是否监听者
    IKCPCB *_cb = NULL;     // kcp控制块
    KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *_sock; // 共享套接字
    UInt64 _sessionId = 0;              // session唯一标识
    UInt32 _dirtyFlag = 0;              // 脏标记
    UInt32 _streamCtrlFlag = 0;         // 流控制
    UInt32 _state = CHANNEL_CREATED;    // 状态
    UInt64 _bufferCapicity = 2048;      // 缓冲大小 udp的缓冲应该大于mtu保证包不被截断
    KCP_CPP_NS::LibTimer* _heartbeatTimer = NULL;       // 心跳
    Int64 _sucHeartbeatCount = 0;
    IProtocolStack *_protocolStack = NULL;
    bool _enableUdpPacketDecorate = false;  // 启用udp包头装饰

    KCP_CPP_NS::LibString _bindIp;
    UInt16 _bindPort = 0;
    KCP_CPP_NS::LibString _remoteIp;
    UInt16 _remotePort = 0;
    KCP_CPP_NS::EndPoint _remoteEndpoint;   // 远程
    
    KCP_CPP_NS::IDelegate<void, UInt64, KcpContext &> *_netDataComeCallback = NULL;      // 接收到网络数据 带conv
    KCP_CPP_NS::IDelegate<void, KcpChannel *, KCP_CPP_NS::LibStream &> *_rcvCallback = NULL;        // 接收消息回调
    KCP_CPP_NS::IDelegate<bool, KcpChannel *, KCP_CPP_NS::EndPoint &> *_willConnectCallback = NULL;    // 新连接即将连入回调
    KCP_CPP_NS::IDelegate<void, KcpChannel *> *_sucGotAckFromRemoteKcp = NULL;   // 收到远端ACK包应答
    KCP_CPP_NS::IDelegate<void, KcpChannel *> *_connectedCallback = NULL;    // 连入回调
    KCP_CPP_NS::IDelegate<void, KcpChannel *> *_disconnectedCallback = NULL;    // 连入回调
    KCP_CPP_NS::IDelegate<int, const char*, int, struct IKCPCB*, KcpChannel*> *_kcpOutputCallback = NULL; // kcp输出回调

    KCP_CPP_NS::LibStream _leftMsgStream;
    KCP_CPP_NS::LibStream _waitingSendMsgStream;
};

inline void KcpChannel::SetSocket(KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *sock)
{
    _sock = sock;
}

inline UInt64 KcpChannel::GetSesssionId() const
{
    return _sessionId;
}

inline void KcpChannel::SetSessionId(UInt64 sessionId)
{
    _sessionId = sessionId;
}


inline bool KcpChannel::IsListener() const
{
    return _isListener;
}

inline void KcpChannel::SetIsListener(bool isListener)
{
    _isListener = isListener;
}

inline SOCKET KcpChannel::GetSocketRaw() const
{
    return _sock->GetSock();
}

inline void KcpChannel::SetBindAddr(const KCP_CPP_NS::LibString &ip, UInt16 port)
{
    _bindIp = ip;
    _bindPort = port;
}

inline void KcpChannel::SetRemoteAddr(const KCP_CPP_NS::LibString &ip, UInt16 port)
{
    _remoteIp = ip;
    _remotePort = port;
}

inline void KcpChannel::SetNetRawDataComeCallback(KCP_CPP_NS::IDelegate<void, UInt64, KcpContext &> *callback)
{
    if(_netDataComeCallback)
        _netDataComeCallback->Release();

    _netDataComeCallback = callback;
}

inline void KcpChannel::SetRcvMsgCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *, KCP_CPP_NS::LibStream &> *callback)
{
    if(_rcvCallback)
        _rcvCallback->Release();

    _rcvCallback = callback;
}

inline void KcpChannel::SetWillConnectCallback(KCP_CPP_NS::IDelegate<bool, KcpChannel *, KCP_CPP_NS::EndPoint &> *callback)
{
    if(_willConnectCallback)
        _willConnectCallback->Release();
    _willConnectCallback = callback;
}

inline void KcpChannel::SetConnectedCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *> *callback)
{
    if(_connectedCallback)
        _connectedCallback->Release();

    _connectedCallback = callback;
}

inline void KcpChannel::SetDisconnectedCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *> *callback)
{
    if(_disconnectedCallback)
        _disconnectedCallback->Release();
    _disconnectedCallback = callback;
}

inline void KcpChannel::SetProtocolStack(IProtocolStack *protocolStack)
{
    _protocolStack = protocolStack;
}

inline void KcpChannel::SetKcpOutput(KCP_CPP_NS::IDelegate<int, const char*, int, struct IKCPCB*, KcpChannel*>* outputCallback)
{
    if(_kcpOutputCallback)
        _kcpOutputCallback->Release();

    _kcpOutputCallback = outputCallback;
}

inline void KcpChannel::SetSucAckBackCallback(KCP_CPP_NS::IDelegate<void, KcpChannel *> *callback)
{
    if(_sucGotAckFromRemoteKcp)
        _sucGotAckFromRemoteKcp->Release();

    _sucGotAckFromRemoteKcp = callback;
}

inline void KcpChannel::SetFamily(Int32 af)
{
    _af = af;
}

inline Int32 KcpChannel::GetFamily() const
{
    return _af;
}

inline void KcpChannel::SetBufferCapicity(UInt64 bufferCapicity)
{
    _bufferCapicity = bufferCapicity;
}

inline void KcpChannel::EnableUdpPacketDecorate(bool enable)
{
    _enableUdpPacketDecorate = enable;
}

inline void KcpChannel::Active()
{
    _state = CHANNEL_ACTIVE;
}

inline bool KcpChannel::IsActived() const
{
    return _state == CHANNEL_ACTIVE;
}

inline UInt32 KcpChannel::GetState() const
{
    return _state;
}

inline UInt64 KcpChannel::GetConv() const
{
    return _cb ? ikcp_getconv(_cb) : 0;
}

inline KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *KcpChannel::GetSock()
{
    return _sock;
}

inline const KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *KcpChannel::GetSock() const
{
    return _sock;
}

inline bool KcpChannel::HasDirty() const
{
    return _dirtyFlag != 0;
}

inline void KcpChannel::MaskReadDirty(bool set)
{
    if(set)
        _dirtyFlag = KCP_CPP_NS::BitUtil::Set(_dirtyFlag, KcpChannel::READ);
    else
        _dirtyFlag = KCP_CPP_NS::BitUtil::Clear(_dirtyFlag, KcpChannel::READ);
}

inline void KcpChannel::MaskWriteDirty(bool set)
{
    if(set)
        _dirtyFlag = KCP_CPP_NS::BitUtil::Set(_dirtyFlag, KcpChannel::WRITE);
    else
        _dirtyFlag = KCP_CPP_NS::BitUtil::Clear(_dirtyFlag, KcpChannel::READ);
}

inline bool KcpChannel::IsDirty(Int32 dirtyType) const
{
    return KCP_CPP_NS::BitUtil::IsSet(_dirtyFlag, dirtyType);
}

inline void KcpChannel::ClearDirty()
{
    _dirtyFlag = 0;
}

inline void KcpChannel::MaskClose()
{
    _streamCtrlFlag = KCP_CPP_NS::BitUtil::Set(_streamCtrlFlag, LAZY_CLOSE);
}

inline void KcpChannel::ForbidSend()
{
    _streamCtrlFlag = KCP_CPP_NS::BitUtil::Set(_streamCtrlFlag, FORBID_SEND);
}

inline void KcpChannel::ForbidRecv()
{
    _streamCtrlFlag = KCP_CPP_NS::BitUtil::Set(_streamCtrlFlag, FORBID_RECV);
}

inline bool KcpChannel::WillClose() const
{
    return KCP_CPP_NS::BitUtil::IsSet(_streamCtrlFlag, LAZY_CLOSE);
}

inline bool KcpChannel::CanSend() const
{
    return !KCP_CPP_NS::BitUtil::IsSet(_streamCtrlFlag, FORBID_SEND);
}

inline bool KcpChannel::CanRecv() const
{
    return !KCP_CPP_NS::BitUtil::IsSet(_streamCtrlFlag, FORBID_RECV);
}

inline const KCP_CPP_NS::LibString &KcpChannel::GetRemoteIp() const
{
    return _remoteIp;
}

inline UInt16 KcpChannel::GetRemotePort() const
{
    return _remotePort;
}

inline const KCP_CPP_NS::EndPoint& KcpChannel::GetRemoteEndpoint() const
{
    return _remoteEndpoint;
}

inline KCP_CPP_NS::EndPoint& KcpChannel::GetRemoteEndpoint()
{
    return _remoteEndpoint;
}

inline bool KcpChannel::IsAlive() const
{
    return _sucHeartbeatCount > 1;
}

inline void KcpChannel::_UpdateHeartbeatCount()
{
    ++_sucHeartbeatCount;
}

#endif
