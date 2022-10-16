#include "pch.h"
#include "KcpClient.h"
#include "IMessageHandler.h"
#include "Handler/Handler.h"
#include "KcpCommon/MessageDefines/MessageDefines.h"

KcpClient *g_Client = NULL;

Int32 KcpClient::Init(Int32 channelCount, const KCP_CPP_NS::LibString &localIp, UInt16 localPort, Int32 family)
{
    g_Client = this;
    // 协议栈
    _RegisterProtocolStack();

    for(Int32 idx = 0; idx < channelCount; ++idx)
    {
        auto newChannel = _CreateChannel(localIp, localPort, _targetIp, _targetPort, family, false);
        if(!newChannel)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("create channel fail localIp:%s, localPort:%hu."), localIp.c_str(), localPort);
            return Status::Failed;
        }
        newChannel->EnableUdpPacketDecorate();
        _sessionIdRefChannel.insert(std::make_pair(newChannel->GetSesssionId(), newChannel));
    }

    _worker = new KCP_CPP_NS::LibThread;
    _worker->AddTask(this, &KcpClient::_OnWork);
    _timerMgr = new KCP_CPP_NS::TimerMgr();

    g_Log->Info(LOGFMT_OBJ_TAG("client init suc."));
    return Status::Success;
}

void KcpClient::ConnectTo(const KCP_CPP_NS::LibString &targetIp, UInt16 targetPort)
{
    _targetIp = targetIp;
    _targetPort = targetPort;
}

Int32 KcpClient::Start()
{
    _worker->Start();
    return Status::Success;
}

void KcpClient::Close()
{
    if(!_worker)
        return;

    if(_worker->HalfClose())
        _worker->FinishClose();

    delete _worker;
    _worker = NULL;

    _sessionIdRefChannel.clear();

    delete _serverClientStack;
    delete _timerMgr;

    _timerMgr = NULL;
}

KcpChannel *KcpClient::GetChannel(UInt64 sessionId)
{
    auto iter = _sessionIdRefChannel.find(sessionId);
    return iter == _sessionIdRefChannel.end() ? NULL : iter->second;
}

KcpChannel *KcpClient::GetChannelByConv(UInt64 conv)
{
    auto iter = _convRefChannel.find(conv);
    return iter == _convRefChannel.end() ? NULL : iter->second;
}

void KcpClient::_OnWork(KCP_CPP_NS::LibThread *t)
{
    // 激活
    for(auto iter:_sessionIdRefChannel)
        iter.second->Active();

    auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_WakeupWorker);
    _timerMgr->Launch(delg);

    // 发送握手消息
    // for (auto iter : _sessionIdRefChannel)
    // {
    //     auto req = new ConnectReq;
    //     iter.second->Send(Opcodes::ConnectReq, req, GetNewPacketId());
    // }

    // 心跳包
    if (_heartbeatMilliseconds)
    {
        auto __heartbeat = [this](KCP_CPP_NS::LibTimer* t)
        {
            for (auto iter : _sessionIdRefChannel)
            {
                auto channel = iter.second;
                // g_Log->Debug(LOGFMT_OBJ_TAG("start send channel heartbeat sessionId:%llu"), iter.first);
                auto req = new HeartbeatReq;
                channel->Send(Opcodes::HeartbeatReq, req, GetNewPacketId());
            }
        };
        auto delg = CREATE_CLOSURE_DELEGATE(__heartbeat, void, KCP_CPP_NS::LibTimer *);
        auto timer = new KCP_CPP_NS::LibTimer(_timerMgr);
        timer->SetTimeOutHandler(delg);
        timer->Schedule(_heartbeatMilliseconds);
    }

    // 发起连接
    auto linkinTimer = [this](KCP_CPP_NS::LibTimer* t)
    {
        Int32 sucConnectNum = 0;
        for (auto iter : _sessionIdRefChannel)
        {
            auto channel = iter.second;
            if(channel->GetConv() == 0)
            {
                auto packetId = GetNewPacketId();
                g_Log->Info(LOGFMT_OBJ_TAG("ShakeHandeWithRemote session id:%llu, channel:%s"), iter.first, channel->ToString().c_str());
                channel->ShakeHandeWithRemote();
            }
            else
            {
                ++sucConnectNum;
            }
        }

        // g_Log->Info(LOGFMT_OBJ_TAG("suc connect num:%d"), sucConnectNum);
        if(sucConnectNum == _sessionIdRefChannel.size())
            delete t;
    };
    {// 尝试每隔两秒连一次
        auto timer = new KCP_CPP_NS::LibTimer(_timerMgr);
        timer->SetTimeOutHandler(CREATE_CLOSURE_DELEGATE(linkinTimer, void, KCP_CPP_NS::LibTimer*));
        timer->Schedule(2000);
    }
    
    auto testTimer = [this](KCP_CPP_NS::LibTimer* t)
    {
        for (auto iter : _sessionIdRefChannel)
        {
            auto channel = iter.second;
            if(channel->GetConv() == 0)
                continue;
                
            auto packetId = GetNewPacketId();
            g_Log->Info(LOGFMT_OBJ_TAG("start send HelloWorldReq sessionId:%llu"), iter.first);
            auto req = new HelloWorldReq;
            channel->Send(Opcodes::HelloWorldReq, req, packetId);
        }
    };

        // for (auto iter : _sessionIdRefChannel)
        // {
        //     auto channel = iter.second;
        //     KCP_CPP_NS::LibStream stream;
        //     Byte8 flag = 1;
        //     stream.InitBuffer(64);
        //     stream.Write(&flag, 1);
        //     channel->RawSend(stream);
        // }

        // auto testTimer = [this](KCP_CPP_NS::LibTimer* t)
        // {
        //     for (auto iter : _sessionIdRefChannel)
        //     {
        //         auto channel = iter.second;
        //         KCP_CPP_NS::LibStream stream;
        //         Byte8 flag = 0;
        //         stream.InitBuffer(64);
        //         stream.Write(&flag, 1);
        //         Int32 conv = 1;
        //         stream.Write(&conv, 4);
        //         BUFFER32 data = {0};
        //         sprintf(data, "hello world");
        //         stream.Write(data, sizeof(data));
        //         channel->RawSend(stream);
        //     }
        // };

        auto timer = new KCP_CPP_NS::LibTimer(_timerMgr);
        timer->SetTimeOutHandler(CREATE_CLOSURE_DELEGATE(testTimer, void, KCP_CPP_NS::LibTimer*));
        timer->Schedule(1000);


    while(!t->IsDestroy())
    {
        if(!_sessionIdRefChannel.empty())
        {
            _DoSelector();
        }
        else
        {
            KCP_CPP_NS::SystemUtil::ThreadSleep(_frameInterval);
        }

        // 关闭channel
        for(auto  iterChannel = _sessionIdRefChannel.begin(); iterChannel != _sessionIdRefChannel.end();)
        {
            auto channel = iterChannel->second;
            if(!channel->WillClose())
            {
                ++iterChannel;
                continue; 
            }

            channel->OnDisconnected();
            channel->Destroy();

            iterChannel = _sessionIdRefChannel.erase(iterChannel);
        }
    
        // 处理定时器事件
        _timerMgr->Drive();

        for (auto iterChannel : _sessionIdRefChannel)
            iterChannel.second->OnUpdate();
    }

    for(auto iter : _sessionIdRefChannel)
        iter.second->Destroy();
}

void KcpClient::_OnNetPacket(UInt64 conv, KcpContext &ctx)
{
    auto channel = GetChannelByConv(conv);
    if(!channel)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("have no channel with conv:%llu"), conv);
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

void KcpClient::_OnRcvMsg(KcpChannel *channel, KCP_CPP_NS::LibStream &msgStream)
{
    std::vector<LibPacket *> packets;
    bool hasBadMsg = false;
    _serverClientStack->Decode(channel->GetSesssionId(), msgStream, packets, hasBadMsg);
    
    for(auto packet : packets)
    {
        auto handler = _GetMsgHandler(packet->GetOpcode());
        if(!handler)
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("have no msg handler packet:%s"), packet->ToString().c_str());
        }
        else
        {
            handler->ProcessMessage(channel, packet);
        }

        delete packet;
    }

    if(msgStream.GetReadableBytes())
    {// 还有没解码完成的数据包，先缓存在channel里
        // g_Log->Debug(LOGFMT_OBJ_TAG("channel has %llu unhandled data channel:%s"), msgStream.GetReadableBytes(), channel->ToString().c_str());
    }

    if (hasBadMsg)
    {
        channel->MaskClose();
        channel->ForbidRecv();
        channel->ForbidSend();
        g_Log->Warn(LOGFMT_OBJ_TAG("has bad msg channel will close channel:%s"), channel->ToString().c_str());
    }
}

bool KcpClient::_OnWillConnect(KcpChannel *monitorChannel, KCP_CPP_NS::EndPoint &endpoint)
{
    g_Log->Warn(LOGFMT_OBJ_TAG("client cant support accepting a link in, remote :%s, monitor channel:%s"), endpoint.ToString().c_str(), monitorChannel->ToString().c_str());

    return false;
}

void KcpClient::_OnConnected(KcpChannel *channel)
{
    // TODO: logic
    // 激活并发送一个握手成功的消息
    channel->Active();

    auto &endpoint = channel->GetSock()->GetRemoteEndPoint();
    const auto &addrPort = KCP_CPP_NS::EndPoint::BuildAddrPortString(endpoint);
    _existChannelAddrPort.insert(addrPort);
    g_Log->Info(LOGFMT_OBJ_TAG("channel suc active and send a connect suc response to remote channel:%s."), channel->ToString().c_str());
}

void KcpClient::_OnDisonnected(KcpChannel *channel)
{
    auto &endpoint = channel->GetSock()->GetRemoteEndPoint();
    const auto &addrPort = KCP_CPP_NS::EndPoint::BuildAddrPortString(endpoint);
    _existChannelAddrPort.erase(addrPort);

    auto sock = channel->GetSock();
    sock->Close();

    _sessionIdRefSock.erase(channel->GetSesssionId());
    _convRefChannel.erase(channel->GetConv());
    g_Log->Info(LOGFMT_OBJ_TAG("channel disconnected:%s, addrPort:%s"), channel->ToString().c_str(), addrPort.c_str());
}

void KcpClient::_OnConnectAckBack(KcpChannel *channel)
{
    // g_Log->Info(LOGFMT_OBJ_TAG("suc got a ack from remote channel:%s"), channel->ToString().c_str());

    auto iter = _convRefChannel.find(channel->GetConv());
    if(iter == _convRefChannel.end())
        _convRefChannel.insert(std::make_pair(channel->GetConv(), channel));
}

void KcpClient::_SendConnectSucToRemote(KcpChannel *channel)
{
    auto res = new ConnectSucRes;
    channel->Send(Opcodes::ConnectSucRes, res, GetNewPacketId());
    // g_Log->Info(LOGFMT_OBJ_TAG("_SendConnectSucToRemote channel:%s"), channel->ToString().c_str());
}

void KcpClient::_DoSelector()
{
    struct timeval tv;
    tv.tv_sec = static_cast<long>(_frameInterval / 1000);
    tv.tv_usec = 1000 * _frameInterval % 1000;

    fd_set fd_read, fd_write, fd_except;
    FD_ZERO(&fd_read);
    FD_ZERO(&fd_write);
    FD_ZERO(&fd_except);

    for(auto iter:_sessionIdRefChannel)
    {
        FD_SET(iter.second->GetSock()->GetSock(), &fd_read);
        FD_SET(iter.second->GetSock()->GetSock(), &fd_write);
        FD_SET(iter.second->GetSock()->GetSock(), &fd_except);
    }

    // 监控sock集合
    auto ret = ::select(0, &fd_read, &fd_write, &fd_except, &tv);
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

    // 有事件触发
    for(auto iterChannel : _sessionIdRefChannel)
    {
        auto channel = iterChannel.second;
        auto sock = channel->GetSocketRaw();

        // 异常先处理
        if(FD_ISSET(sock, &fd_except))
        {
            channel->MaskClose();
            channel->ForbidRecv();
            channel->ForbidSend();
            g_Log->Warn(LOGFMT_OBJ_TAG("channel exepte and will close later channel:%s"), channel->ToString().c_str());
            continue;
        }
        
        if(FD_ISSET(sock, &fd_read))
        {// 可读事件
            channel->OnMonitor();
        }

        if(FD_ISSET(sock, &fd_write))
        {// 可写事件
            channel->OnWritable();
        }
    }
}

void KcpClient::_WakeupWorker()
{
    // TODO:
}

void KcpClient::_RegisterProtocolStack()
{
    // 客户端服务器协议
    _serverClientStack = ProtocolStackFactory::Create(ProtocolStackType::SERVER_CLIENT);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::ConnectReq, new ConnectReqFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::ConnectSucRes, new ConnectSucResFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::HelloWorldReq, new HelloWorldReqFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::HelloWorldRes, new HelloWorldResFactory);
    _serverClientStack->RegisterOpcodeCoderFactory(Opcodes::HeartbeatReq, new HeartbeatReqFactory);

    _opcodeRefHandler.insert(std::make_pair(Opcodes::ConnectSucRes, new ConnectSucResHandler));
    _opcodeRefHandler.insert(std::make_pair(Opcodes::HelloWorldReq, new HelloWorldReqHandler));
    _opcodeRefHandler.insert(std::make_pair(Opcodes::HelloWorldRes, new HelloWorldResHandler));
}


KcpChannel *KcpClient::_CreateChannel(const KCP_CPP_NS::LibString &localIp, UInt16 localPort, 
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

    {// 设置接收到网络数据回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_OnNetPacket);
        newChannel->SetNetRawDataComeCallback(delg);
    }
    if(!newChannel->IsListener())
    {// 设置接收消息回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_OnRcvMsg);
        newChannel->SetRcvMsgCallback(delg);
    }
    {// 即将连入回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_OnWillConnect);
        newChannel->SetWillConnectCallback(delg);
    }

    {// 连接回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_OnConnected);
        newChannel->SetConnectedCallback(delg);
    }

    {// 断开连接回调
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_OnDisonnected);
        newChannel->SetDisconnectedCallback(delg);
    }
    {// ack包到来需要映射
        auto delg = KCP_CPP_NS::DelegateFactory::Create(this, &KcpClient::_OnConnectAckBack);
        newChannel->SetSucAckBackCallback(delg);
    }

    newChannel->SetProtocolStack(_serverClientStack);

    auto sock = new KCP_CPP_NS::LibSocket<KCP_CPP_NS::SockProtocolType::UDP>;
    _sessionIdRefSock.insert(std::make_pair(newChannel->GetSesssionId(), sock));
    auto err = sock->Create(family);
    if (err != Status::Success)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("create sock fail err:%d"), err);
        return NULL;
    }

    sock->SetNonBlocking();
    sock->SetReuseAddr();

    sock->Bind(family, localIp, 0);

    if (!remoteIp.empty())
        sock->Connect(remoteIp, remotePort);

    newChannel->SetSocket(sock);
    err = newChannel->Init();
    if(err != Status::Success)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("create listen channel fail."));
        delete newChannel;
        return NULL;
    }

//     g_Log->Info(LOGFMT_OBJ_TAG("new channel init suc local ip:%s, local port:%hu, remote ip:%s, remote port:%hu new channel:%s.")
//     , localIp.c_str(), localPort, remoteIp.c_str(), remotePort, newChannel->ToString().c_str());
//     
    return newChannel;
}

IMessageHandler *KcpClient::_GetMsgHandler(UInt16 opcode)
{
    auto iter = _opcodeRefHandler.find(opcode);
    return iter == _opcodeRefHandler.end() ? NULL : iter->second;
}


