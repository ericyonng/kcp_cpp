#include "pch.h"
#include "KcpServer.h"
#include "IMessageHandler.h"
#include "Handler/HelloWorldReqHandler.h"
#include "Handler/HelloWorldResHandler.h"
#include "Handler/ConnectReqHandler.h"
#include "Handler/ConnectSucResHandler.h"
#include "Handler/HeartbeatReqHandler.h"

KcpServer *g_Server = NULL;

Int32 KcpServer::Init(const KCP_CPP_NS::LibString &ip, UInt16 port, Int32 family)
{
    g_Server = this;
    
    // 协议栈
    _RegisterProtocolStack();

    _sock = new KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP>;
    auto errCode = _sock->Create(family);
    if(errCode != Status::Success)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("create sock fail errCode:%d"), errCode);
        return errCode;
    }

    _sock->SetNonBlocking();
    _sock->SetReuseAddr();
    if(!ip.empty())
        _sock->Bind(family, ip, port);

    _monitorChannel = _CreateChannel(_sock, ip, port, "", 0, family, true);
    if(!_monitorChannel)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("create listen channel fail ip:%s, port:%hu."), ip.c_str(), port);
        return Status::Failed;
    }

    _sessionIdRefChannel.insert(std::make_pair(_monitorChannel->GetSesssionId(), _monitorChannel));
    _worker = new KCP_CPP_NS::LibThread;
    _worker->AddTask(this, &KcpServer::_OnWork);
    _timerMgr = new KCP_CPP_NS::TimerMgr();

    g_Log->Info(LOGFMT_OBJ_TAG("server init suc."));
    return Status::Success;
}

Int32 KcpServer::Start()
{
    _worker->Start();
    return Status::Success;
}

void KcpServer::Close()
{
    if(!_worker)
        return;

    if(_worker->HalfClose())
        _worker->FinishClose();

    delete _worker;
    _worker = NULL;

    for(auto iter: _sessionIdRefChannel)
        delete iter.second;
    _sessionIdRefChannel.clear();

    _sock->Close();
    delete _sock;
    delete _serverClientStack;
    delete _timerMgr;
    for(auto iter : _opcodeRefHandler)
        iter.second->Release();
    _opcodeRefHandler.clear();

    _monitorChannel = NULL;
    _sock = NULL;
    _timerMgr = NULL;
    _serverClientStack = NULL;
}

KcpChannel *KcpServer::GetChannel(UInt64 sessionId)
{
    auto iter = _sessionIdRefChannel.find(sessionId);
    return iter == _sessionIdRefChannel.end() ? NULL : iter->second;
}

KcpChannel *KcpServer::GetChannelByConv(UInt64 conv)
{
    auto iter = _convRefChannel.find(conv);
    return iter == _convRefChannel.end() ? NULL : iter->second;
}

void KcpServer::_OnWork(KCP_CPP_NS::LibThread *t)
{
    // 激活
    _monitorChannel->Active();
    auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpServer::_WakeupWorker);
    _timerMgr->Launch(delg);

    g_Log->Info(LOGFMT_OBJ_TAG("kcp server working monitor channel:%s."), _monitorChannel->ToString().c_str());

    while(!t->IsDestroy())
    {
        // 设置监控集合
        _DoSelector();

        // 关闭channel
        for(auto  iterChannel = _sessionIdRefChannel.begin(); iterChannel != _sessionIdRefChannel.end();)
        {
            auto channel = iterChannel->second;
            if(!channel->WillClose())
            {
                ++iterChannel;
                continue; 
            }

            auto endpoint = channel->GetRemoteEndpoint();
            auto isListener = channel->IsListener();
            auto conv = channel->GetConv();
            channel->OnDisconnected();
            channel->Destroy();

            if(!isListener)
            {
                _convRefChannel.erase(conv);
                _addrRefKcpChannel.erase(KCP_CPP_NS::EndPoint::BuildAddrPortString(endpoint));
            }

            delete channel;
            iterChannel = _sessionIdRefChannel.erase(iterChannel);
        }
    
        // 处理定时器事件
        _timerMgr->Drive();

        // 刷新
        for(auto iterChannel : _sessionIdRefChannel)
            iterChannel.second->OnUpdate();
    }

    for(auto iter : _sessionIdRefChannel)
        iter.second->Destroy();
}

void KcpServer::_OnNetPacket(UInt64 conv, KcpContext &ctx)
{
    auto channel = GetChannelByConv(conv);
    if(!channel)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no channel with conv:%llu"), conv);
        return;
    }

    // 校验上下文
    auto &remoterEndpoint = channel->GetRemoteEndpoint();
    if(remoterEndpoint != ctx._endpoint)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("bad remote endpoint:%s, conv:%llu, channel:%s")
        , ctx._endpoint.ToString().c_str(), conv, channel->ToString().c_str());
        return;
    }

    // g_Log->Debug(LOGFMT_OBJ_TAG("channel got a net data channel:%s"), channel->ToString().c_str());
    channel->OnReadable(ctx._data);
}

void KcpServer::_OnRcvMsg(KcpChannel *channel, KCP_CPP_NS::LibStream &msgStream)
{
    KCP_CPP_NS::LibString hex;
    KCP_CPP_NS::StringUtil::ToHexStringView(msgStream.GetReadBegin(), msgStream.GetReadableBytes(), hex);
//     g_Log->NetInfo(LOGFMT_OBJ_TAG("kcp server will decode data session id:%llu, hex data:%s")
//             , channel->GetSesssionId(), hex.c_str());

    std::vector<LibPacket *> packets;
    bool hasBadMsg = false;
    _serverClientStack->Decode(channel->GetSesssionId(), msgStream, packets, hasBadMsg);
    
    for(auto packet : packets)
    {
//         g_Log->NetInfo(LOGFMT_OBJ_TAG("kcp server will handle packet data session id:%llu, opcode:%hu, packet id:%lld, _curRecvMaxPacketId:%lld")
//                 , channel->GetSesssionId(), packet->GetOpcode(), packet->GetPacketId(), _curRecvMaxPacketId);

        if (_curRecvMaxPacketId < packet->GetPacketId())
            _curRecvMaxPacketId = packet->GetPacketId();

        auto handler = _GetMsgHandler(packet->GetOpcode());
        if(!handler)
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("have no msg handler packet:%s"), packet->ToString().c_str());
        }
        else
        {
            handler->ProcessMessage(channel, packet);
            channel->UpdateHeatbeatTimer(_heartbeatTime);
        }

        delete packet;
    }

    if (hasBadMsg)
    {
        channel->MaskClose();
        channel->ForbidRecv();
        channel->ForbidSend();

        if(channel->IsListener())
        {
            g_Log->Error(LOGFMT_OBJ_TAG("has bad msg channel will close channel:%s"), channel->ToString().c_str());
        }
        else
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("has bad msg channel will close channel:%s"), channel->ToString().c_str());
        }
    }
}

bool KcpServer::_OnWillConnect(KcpChannel *monitorChannel, KCP_CPP_NS::EndPoint &endpoint)
{
    g_Log->Info(LOGFMT_OBJ_TAG("will accept a link in, remote :%s, monitor channel:%s"), endpoint.ToString().c_str(), monitorChannel->ToString().c_str());
 
    const auto &addr = KCP_CPP_NS::EndPoint::BuildAddrPortString(endpoint);
    auto channel = _GetChannel(addr);
    if(channel)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("endpoint %s, has already link in channel:%s"), endpoint.ToString().c_str(), channel->ToString().c_str());
        return false;
    }

    const auto &localEndpoint = monitorChannel->GetSock()->GetLocalEndPoint();
    channel = _CreateChannel(_sock, localEndpoint._addr, localEndpoint._port, endpoint._addr, endpoint._port, monitorChannel->GetFamily(), false);
    auto err = channel->InitKcpCb(++_maxConv);
    if(err != Status::Success)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("init kcp cb fail channel:%s, err:%d"), channel->ToString().c_str(), err);
        delete channel;
        return false;
    }

    _MakeDict(channel);

    channel->OnConnected(endpoint);
    g_Log->Info(LOGFMT_OBJ_TAG("new channel link in suc channel:%s"), channel->ToString().c_str());
    return true;
}

void KcpServer::_OnConnected(KcpChannel *channel)
{
    // TODO: logic
    // 激活
    channel->Active();

    KCP_CPP_NS::LibStream stream;
    stream.InitBuffer(5);
    stream.WriteInt8(1 << KcpFlags::BIT_ACK);
    auto conv = channel->GetConv();
    stream.Write(&conv, 4);
    channel->RawSend(stream);

    // 每秒发送一次确认直到客户端alive TODO:可以使用广播的方式，将客户端放到广播列表，alive后移除
    auto sessionId = channel->GetSesssionId();
    auto timer = new KCP_CPP_NS::LibTimer(_timerMgr);
    auto connectAckSend = [this, sessionId](KCP_CPP_NS::LibTimer* t)
    {
        auto channel = GetChannel(sessionId);
        if(!channel || channel->WillClose())
        {
            delete t;
            return;
        }
        
        if(channel->IsAlive())
        {
            g_Log->Info(LOGFMT_OBJ_TAG("channel alive and will stop send connect suc ack channel:%s"), channel->ToString().c_str());
            delete t;
            return;
        }

        KCP_CPP_NS::LibStream stream;
        stream.InitBuffer(5);
        stream.WriteInt8(1 << KcpFlags::BIT_ACK);
        auto conv = channel->GetConv();
        stream.Write(&conv, 4);
        channel->RawSend(stream);

        // g_Log->Info(LOGFMT_OBJ_TAG("will send connect suc back ack channel:%s"), channel->ToString().c_str());
    };
    auto delg = CREATE_CLOSURE_DELEGATE(connectAckSend, void, KCP_CPP_NS::LibTimer *);
    timer->SetTimeOutHandler(delg);
    timer->Schedule(1000);

    const auto &endpoint = channel->GetSock()->GetRemoteEndPoint();
    const auto &addrPort = KCP_CPP_NS::EndPoint::BuildAddrPortString(endpoint);
    g_Log->Info(LOGFMT_OBJ_TAG("channel suc active and send a connect suc response to remote channel:%s."), channel->ToString().c_str());
}

void KcpServer::_OnDisonnected(KcpChannel *channel)
{
    auto &endpoint = channel->GetSock()->GetRemoteEndPoint();
    const auto &addrPort = KCP_CPP_NS::EndPoint::BuildAddrPortString(endpoint);

    // g_Log->Info(LOGFMT_OBJ_TAG("channel disconnected:%s, addrPort:%s"), channel->ToString().c_str(), addrPort.c_str());
}

void KcpServer::_SendConnectSucToRemote(KcpChannel *channel)
{
    auto res = new ConnectSucRes;
    channel->Send(Opcodes::ConnectSucRes, res);
    g_Log->Info(LOGFMT_OBJ_TAG("_SendConnectSucToRemote channel:%s"), channel->ToString().c_str());
}

void KcpServer::_DoSelector()
{
    struct timeval tv;
    tv.tv_sec = static_cast<long>(_frameInterval / 1000);
    tv.tv_usec = 1000 * _frameInterval % 1000;

    fd_set fd_read, fd_except;
    FD_ZERO(&fd_read);
    FD_ZERO(&fd_except);
    FD_SET(_monitorChannel->GetSocketRaw(), &fd_read);
    FD_SET(_monitorChannel->GetSocketRaw(), &fd_except);

    // 监控sock集合
    auto ret = ::select(0, &fd_read, NULL, &fd_except, &tv);
    if(ret == -1)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("select fail err:%d"), KCP_CPP_NS::GetErrorCode());
        return;
    }

    // 超时
    if(ret == 0)
    {
        g_Log->Debug(LOGFMT_OBJ_TAG("select time out."));
        return;
    }

    std::vector<KcpChannel *> newChannels;
    if(FD_ISSET(_monitorChannel->GetSocketRaw(), &fd_read))
    {// 监控
        _monitorChannel->OnMonitor();
    }
}

void KcpServer::_WakeupWorker()
{
    // TODO:
}

void KcpServer::_RegisterProtocolStack()
{
    // 客户端服务器协议
    _serverClientStack = ProtocolStackFactory::Create(ProtocolStackType::SERVER_CLIENT);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::ConnectReq, new ConnectReqFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::ConnectSucRes, new ConnectSucResFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::HelloWorldReq, new HelloWorldReqFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::HelloWorldRes, new HelloWorldResFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::HeartbeatReq, new HeartbeatReqFactory);

    _opcodeRefHandler.insert(std::make_pair(Opcodes::ConnectReq, new ConnectReqHandler));
    _opcodeRefHandler.insert(std::make_pair(Opcodes::ConnectSucRes, new ConnectSucResHandler));
    _opcodeRefHandler.insert(std::make_pair(Opcodes::HelloWorldReq, new HelloWorldReqHandler));
    _opcodeRefHandler.insert(std::make_pair(Opcodes::HelloWorldRes, new HelloWorldResHandler));
    _opcodeRefHandler.insert(std::make_pair(Opcodes::HeartbeatReq, new HeartbeatReqHandler));
}


KcpChannel *KcpServer::_CreateChannel(KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP> *sock, const KCP_CPP_NS::LibString &localIp, UInt16 localPort, 
                            const KCP_CPP_NS::LibString &remoteIp, UInt16 remotePort,  
                            Int32 family, bool isListener)
{
    auto newChannel = new KcpChannel();
    newChannel->SetSessionId(++_maxSessionId);
    newChannel->SetIsListener(isListener);
    newChannel->SetBindAddr(localIp, localPort);

    // 远程地址
    if(!remoteIp.empty())
        newChannel->SetRemoteAddr(remoteIp, remotePort);

    newChannel->SetFamily(family);

    {// 设置接收消息回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpServer::_OnNetPacket);
        newChannel->SetNetRawDataComeCallback(delg);
    }

    if(!newChannel->IsListener())
    {// 设置接收消息回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpServer::_OnRcvMsg);
        newChannel->SetRcvMsgCallback(delg);
    }
    {// 即将连入回调
        if(newChannel->IsListener())
        {
            auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpServer::_OnWillConnect);
            newChannel->SetWillConnectCallback(delg);
        }
    }

    {// 连接回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpServer::_OnConnected);
        newChannel->SetConnectedCallback(delg);
    }

    {// 断开连接回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpServer::_OnDisonnected);
        newChannel->SetDisconnectedCallback(delg);
    }

    {// 需要装饰
        if(!newChannel->IsListener())
            newChannel->EnableUdpPacketDecorate();
    }

    newChannel->SetSocket(sock);

    if(!newChannel->IsListener())
        newChannel->SetProtocolStack(_serverClientStack);

    auto err = newChannel->Init();
    if(err != Status::Success)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("create listen channel fail."));
        delete newChannel;
        return NULL;
    }

    if (!newChannel->IsListener() && _heartbeatTime)
    {
        auto timer = new KCP_CPP_NS::LibTimer(_timerMgr);
        auto __heartbeatTimeout = [this, newChannel](KCP_CPP_NS::LibTimer* t)
        {
            g_Log->Info(LOGFMT_OBJ_TAG("channel heartbeat time out channel:%s"), newChannel->ToString().c_str());
            if (!newChannel->WillClose())
            {
                newChannel->ForbidRecv();
                newChannel->ForbidSend();
                newChannel->MaskClose();
            }
        };
        auto delg = CREATE_CLOSURE_DELEGATE(__heartbeatTimeout, void, KCP_CPP_NS::LibTimer *);
        timer->SetTimeOutHandler(delg);
        newChannel->InitHeartbeatTimer(timer);
        newChannel->UpdateHeatbeatTimer(_heartbeatTime);
    }

    g_Log->Info(LOGFMT_OBJ_TAG("new channel init suc local ip:%s, local port:%hu, remote ip:%s, remote port:%hu _heartbeatTime:%lld new channel:%s.")
    , localIp.c_str(), localPort, remoteIp.c_str(), remotePort, _heartbeatTime, newChannel->ToString().c_str());
    return newChannel;
}

IMessageHandler *KcpServer::_GetMsgHandler(UInt16 opcode)
{
    auto iter = _opcodeRefHandler.find(opcode);
    return iter == _opcodeRefHandler.end() ? NULL : iter->second;
}

void KcpServer::_MakeDict(KcpChannel *channel)
{
    _sessionIdRefChannel.insert(std::make_pair(channel->GetSesssionId(), channel));

    const auto& ip = channel->GetRemoteIp();
    const auto port = channel->GetRemotePort();
    const auto &addr = KCP_CPP_NS::EndPoint::BuildAddrPortString(KCP_CPP_NS::EndPoint(ip, port));

    if(channel->GetConv())
        _convRefChannel.insert(std::make_pair(channel->GetConv(), channel));
    _addrRefKcpChannel.insert(std::make_pair(addr, channel));
}

