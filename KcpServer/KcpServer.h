
#ifndef __KCP_CPP_KCP_SERVER_KCP_SERVER_H__
#define __KCP_CPP_KCP_SERVER_KCP_SERVER_H__

#pragma once

#include "DevelopmentKit/DevelopmentKit.h"

class KcpChannel;
class IMessageHandler;

class KcpServer
{
public:
    Int32 Init(const KCP_CPP_NS::LibString &ip, UInt16 port, Int32 family = AF_INET);
    Int32 Start();
    void Close();

    void SetFramInterval(UInt64 intevalMilliseconds);
    void SetHeartbeatMilliseconds(Int64 milliseconds);

    KCP_CPP_NS::TimerMgr *GetTimerMgr() { return _timerMgr; } 

    KcpChannel *GetChannel(UInt64 sessionId);
    KcpChannel *GetChannelByConv(UInt64 conv);

private:
    void _OnWork(KCP_CPP_NS::LibThread *t);

    #pragma region // 事件
    void _OnNetPacket(UInt64 conv, KcpContext &ctx);
    void _OnRcvMsg(KcpChannel *channel, KCP_CPP_NS::LibStream &msgStream);
    bool _OnWillConnect(KcpChannel *monitorChannel, KCP_CPP_NS::EndPoint &endpoint);
    void _OnConnected(KcpChannel *channel);
    void _OnDisonnected(KcpChannel *channel);
    #pragma endregion

    void _SendConnectSucToRemote(KcpChannel *channel);

    void _GatherSocks();
    void _DoSelector();
    
    // 唤醒worker
    void _WakeupWorker();

    // 注册协议栈
    void _RegisterProtocolStack();

    // 创建通道
    KcpChannel *_CreateChannel(KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *sock,
                                const KCP_CPP_NS::LibString &localIp, UInt16 localPort, 
                                const KCP_CPP_NS::LibString &remoteIp, UInt16 remotePort,  
                                Int32 family, bool isListener);

    IMessageHandler *_GetMsgHandler(UInt16 opcode);

    KcpChannel *_GetChannel(const KCP_CPP_NS::LibString &addr);

    void _MakeDict(KcpChannel *channel);

private:
    std::atomic<UInt64> _maxSessionId{0};
    std::unordered_map<UInt64, KcpChannel *> _sessionIdRefChannel;
    std::map<KCP_CPP_NS::LibString, KcpChannel *> _addrRefKcpChannel;    // ip:port=>channel
    std::map<UInt64, KcpChannel *> _convRefChannel;    // conv=>channel
    Int64 _heartbeatTime = 0;   // 0表示没有心跳
    UInt32 _maxConv = 0;

    KCP_CPP_NS::LibThread *_worker = NULL;
    UInt64 _frameInterval = 1;              // 默认1ms

    KcpChannel *_monitorChannel = NULL;     // 监控
    KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *_sock; // 监控套接字
    
    KCP_CPP_NS::TimerMgr *_timerMgr = NULL; // 定时器

    IProtocolStack *_serverClientStack = NULL;
    std::unordered_map<UInt16, IMessageHandler *> _opcodeRefHandler;

    Int64 _curRecvMaxPacketId = 0;
};

inline void KcpServer::SetFramInterval(UInt64 intevalMilliseconds)
{
    _frameInterval = intevalMilliseconds;
}

inline void KcpServer::SetHeartbeatMilliseconds(Int64 milliseconds)
{
    _heartbeatTime = milliseconds;
}

inline KcpChannel *KcpServer::_GetChannel(const KCP_CPP_NS::LibString &addr)
{
    auto iter = _addrRefKcpChannel.find(addr);
    return iter == _addrRefKcpChannel.end() ? NULL : iter->second;
}


extern KcpServer *g_Server;

#endif
