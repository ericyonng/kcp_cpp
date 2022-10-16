


#ifndef __KCP_CPP_KCP_CLIENT_KCP_CLIENT_H__
#define __KCP_CPP_KCP_CLIENT_KCP_CLIENT_H__

#pragma once

#include "DevelopmentKit/DevelopmentKit.h"

class KcpChannel;
class IMessageHandler;

class KcpClient
{
public:
    Int32 Init(Int32 channelCount, const KCP_CPP_NS::LibString &localIp, UInt16 localPort = 0, Int32 family = AF_INET);
    Int32 Start();
    void Close();
    void ConnectTo(const KCP_CPP_NS::LibString &targetIp, UInt16 targetPort);

    void SetFramInterval(UInt64 intevalMilliseconds);
    void SetHeartbeat(Int64 millisecconds);

    KCP_CPP_NS::TimerMgr *GetTimerMgr() { return _timerMgr; } 

    KcpChannel *GetChannel(UInt64 sessionId);
    KcpChannel *GetChannelByConv(UInt64 conv);
    bool IsChannelExistsByAddrPort(const KCP_CPP_NS::LibString &addrPort) const;
    Int64 GetNewPacketId();

private:
    void _OnWork(KCP_CPP_NS::LibThread *t);

    #pragma region // 事件
    void _OnNetPacket(UInt64 conv, KcpContext &ctx);
    void _OnRcvMsg(KcpChannel *channel, KCP_CPP_NS::LibStream &msgStream);
    bool _OnWillConnect(KcpChannel *monitorChannel, KCP_CPP_NS::EndPoint &endpoint);
    void _OnConnected(KcpChannel *channel);
    void _OnDisonnected(KcpChannel *channel);
    void _OnConnectAckBack(KcpChannel *channel);
    #pragma endregion

    void _SendConnectSucToRemote(KcpChannel *channel);

    void _GatherSocks();
    void _DoSelector();
    
    // 唤醒worker
    void _WakeupWorker();

    // 注册协议栈
    void _RegisterProtocolStack();

    // 创建通道
    KcpChannel *_CreateChannel(const KCP_CPP_NS::LibString &localIp, UInt16 localPort, 
                                const KCP_CPP_NS::LibString &remoteIp, UInt16 remotePort,  
                                Int32 family, bool isListener);

    IMessageHandler *_GetMsgHandler(UInt16 opcode);

private:
    std::atomic<UInt64> _maxSessionId{0};
    std::unordered_map<UInt64, KcpChannel *> _sessionIdRefChannel;
    std::unordered_map<UInt64, KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *> _sessionIdRefSock;
    std::unordered_map<UInt64, KcpChannel *> _convRefChannel;
    std::set<KCP_CPP_NS::LibString> _existChannelAddrPort;

    KCP_CPP_NS::LibThread *_worker = NULL;
    UInt64 _frameInterval = 1;              // 默认1ms
    Int64 _heartbeatMilliseconds = 0;       // 默认没有心跳

    KCP_CPP_NS::TimerMgr *_timerMgr = NULL; // 定时器

    IProtocolStack *_serverClientStack = NULL;
    std::unordered_map<UInt16, IMessageHandler *> _opcodeRefHandler;

    KCP_CPP_NS::LibString _targetIp;
    UInt16 _targetPort = 0;

    Int64 _maxPacketId = 0;
};

inline void KcpClient::SetFramInterval(UInt64 intevalMilliseconds)
{
    _frameInterval = intevalMilliseconds;
}

inline void KcpClient::SetHeartbeat(Int64 millisecconds)
{
    _heartbeatMilliseconds = millisecconds;
}

inline bool KcpClient::IsChannelExistsByAddrPort(const KCP_CPP_NS::LibString &addrPort) const
{
    return _existChannelAddrPort.find(addrPort) != _existChannelAddrPort.end();
}

inline Int64 KcpClient::GetNewPacketId()
{
    return ++_maxPacketId;
}

extern KcpClient *g_Client;

#endif
