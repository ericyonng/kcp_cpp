#include "pch.h"
#include "KcpCommon/NetCommon/KcpChannel.h"
#include "KcpCommon/NetCommon/ikcp.h"
#include "KcpCommon/NetCommon/ProtocolCoder.h"
#include "KcpCommon/NetCommon/KcpContext.h"
#include "KcpCommon/NetCommon/Protocol/Protocol.h"
#include "KcpCommon/NetCommon/LibPacket.h"

static int KcpChannelOutput(const char *buf, int len, struct IKCPCB *kcp, void *channel)
{
    auto kcpChannel = LibCast<KcpChannel>(channel);
    return kcpChannel->OnKcpOutput(buf, len);

}

KcpChannel::~KcpChannel()
{
    Destroy();
}

void KcpChannel::Destroy()
{
    if(_cb)
        ikcp_release(_cb);
    _cb = NULL;

    if(_rcvCallback)
        _rcvCallback->Release();
    _rcvCallback = NULL;

    if(_connectedCallback)
        _connectedCallback->Release();
    _connectedCallback = NULL;

    if(_disconnectedCallback)
        _disconnectedCallback->Release();
    _disconnectedCallback = NULL;

    if (_kcpOutputCallback)
        _kcpOutputCallback->Release();
    _kcpOutputCallback = NULL;

    if (_heartbeatTimer)
        delete _heartbeatTimer;

    _heartbeatTimer = NULL;
}

void KcpChannel::OnMonitor()
{
    OnReadable();
}

bool KcpChannel::Send(UInt16 opcode, IProtocolCoder *coder, Int64 packetId)
{
    if(!CanSend() || !IsActived())
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("cant send msg or channel not actived opcode:%hu, channel:%s")
                , opcode, ToString().c_str());
        return false;
    }

    if(!_cb)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no kcp cb packetId:%lld channel:%s"), packetId, ToString().c_str());
        coder->Release();
        return false;
    }

    MaskWriteDirty(false);

    // 1.构建packet
    auto newPacket = new LibPacket;
    newPacket->SetCoder(coder);
    newPacket->SetOpcode(opcode);
    newPacket->SetPacketId(packetId);
    newPacket->SetSessionId(_sessionId);

    // 2.等待发送的需要追加到发送 :todo可以使用链表替换 这里简单的做法
    KCP_CPP_NS::LibStream stream;
    if(_waitingSendMsgStream.GetReadableBytes())
    {
        stream.InitBuffer(_waitingSendMsgStream.GetReadableBytes());
        stream.Write(_waitingSendMsgStream.GetReadBegin(), _waitingSendMsgStream.GetReadableBytes());
        _waitingSendMsgStream.ShiftReadPos(_waitingSendMsgStream.GetReadableBytes());
        _waitingSendMsgStream.Destroy();
    }
    const auto oldLen = stream.GetWritePos();
    _protocolStack->Encode(newPacket, stream);
    const auto diffBytes = stream.GetWritePos() - oldLen;

    // KCP_CPP_NS::LibStream decodeStream;
    // decodeStream.AttachBuffer(stream.GetReadBegin(), stream.GetReadableBytes(), 0, stream.GetReadableBytes());
    // std::vector<LibPacket*> packets;
    // bool hasBadMsg = false;
    // _protocolStack->Decode(this->GetSesssionId(), decodeStream, packets, hasBadMsg);
    // if (hasBadMsg)
    // {
    //     g_Log->Error(LOGFMT_OBJ_TAG("channel send bad msg opcode:%hu, packet id:%lld channel:%s"), opcode, packetId, ToString().c_str());
    // }

    KCP_CPP_NS::LibString hex;
    KCP_CPP_NS::StringUtil::ToHexStringView(stream.GetReadBegin(), stream.GetReadableBytes(), hex);
//     g_Log->NetInfo(LOGFMT_OBJ_TAG("channel send data session id:%llu, opcode:%hu, packet id:%lld hex data:%s")
//             , GetSesssionId(), opcode, packetId, hex.c_str());

//     g_Log->Info(LOGFMT_OBJ_TAG("channel {%s}\nsend msg opcode:%d, %s, packetId:%lld, will send bytes:%llu")
//                         , ToString().c_str(), coder->GetOpcode(), coder->GetOpcodeName().c_str(), packetId, diffBytes);

    // 3.判断是否超过两边发送窗口
    // auto waitsnd = ikcp_waitsnd(_cb);
    // const auto doubleSndWnd = 2 * _cb->snd_wnd;
    // if(waitsnd > static_cast<int>(doubleSndWnd))
    // {
    //     g_Log->Warn(LOGFMT_OBJ_TAG("net busy waitsnd:%d channel:%s"), waitsnd, ToString().c_str());
    //     MaskWriteDirty();

    //     if(_waitingSendMsgStream.GetCapicity() == 0)
    //         _waitingSendMsgStream.InitBuffer(stream.GetReadableBytes());

    //     _waitingSendMsgStream.Write(stream.GetReadBegin(), stream.GetReadableBytes());
    //     return true;
    // }

    MaskWriteDirty(false);

    // 4.超过发送长度的分多批次发送
    _SendStream(stream);

    // 5.剩余的数据缓存
    if(stream.GetReadableBytes())
    {
        if(_waitingSendMsgStream.GetCapicity() == 0)
            _waitingSendMsgStream.InitBuffer(stream.GetReadableBytes());
        _waitingSendMsgStream.Write(stream.GetReadBegin(), stream.GetReadableBytes());
    }
    _waitingSendMsgStream.Compress();

    if(_waitingSendMsgStream.GetReadableBytes())
        MaskWriteDirty();

//     g_Log->Debug(LOGFMT_OBJ_TAG("send msg suc opcode:%hu, packetId:%llu, next frame send bytes:%llu  channel:%s")
//                 , opcode, packetId, _waitingSendMsgStream.GetReadableBytes(), ToString().c_str());

    delete newPacket;
    return true;
}

bool KcpChannel::ShakeHandeWithRemote()
{
    if(!_sock->IsValid())
    {
        g_Log->Error(LOGFMT_OBJ_TAG("have no valid socket channel:%s"), ToString().c_str());
        return false;
    }

    // 是否已经创建了kcp
    if(_cb)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("had conneted remote before channel:%s"), ToString().c_str());
        return false;
    }

    // 发送syn包
    U8 flag = 0;
    flag = 1 << KcpFlags::BIT_SYN;
    KCP_CPP_NS::LibStream stream;
    stream.Write(&flag, 1);
    RawSend(stream);

    return true;
}

int KcpChannel::RawSend(KCP_CPP_NS::LibStream& stream)
{
    auto sock = GetSock();
    Int32 errCode = KCP_CPP_NS::SocketStatus::valid;

    KCP_CPP_NS::EndPoint *endpoint = NULL;
    if (IsActived())
        endpoint = &GetRemoteEndpoint();

    // 打印网络包
    const auto toSendBytes = stream.GetReadableBytes();
    if(toSendBytes == 0)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no stream to send channel:%s"), ToString().c_str());
        return 0;
    }

    if (toSendBytes)
    {
        KCP_CPP_NS::LibString hex;
        KCP_CPP_NS::StringUtil::ToHexStringView(stream.GetReadBegin(), toSendBytes, hex);
        // g_Log->NetInfo(LOGFMT_OBJ_TAG("raw send net data hex:\n%s"), hex.c_str());
    }

    // 发送
    auto sendBytes = sock->Send(stream, endpoint, errCode);
    if (errCode != KCP_CPP_NS::SocketStatus::valid)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("sock send fail errCode:%d, channel:%s"), errCode, ToString().c_str());
        if(errCode == KCP_CPP_NS::SocketStatus::non_blocking_would_have_blocked)
        {// 不需要处理,丢包会重传
            return 0;
        }

        if(!IsListener())
        {// 发包失败如果不是监听者就关掉
            MaskClose();
            g_Log->Warn(LOGFMT_OBJ_TAG("send stream fail errCode:%d will close later channel:%s"), errCode, ToString().c_str());
        }
        
        return 0;
    }

    // 发送结果
    if (sendBytes != toSendBytes)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("send not completion data to remote sendBytes:%llu, toSendBytes:%llu channel:%s."), sendBytes, toSendBytes, ToString().c_str());
    }

    return 0;
}

int KcpChannel::OnKcpOutput(const Byte8 *buf, int len)
{
    if(!_cb)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no kcp cb channel:%s"), ToString().c_str());
        return 0;
    }

    if(_kcpOutputCallback)
        return _kcpOutputCallback->Invoke(buf, len, _cb, this);

//     g_Log->Debug(LOGFMT_OBJ_TAG("have no output callback and will use default."));

    auto sock = GetSock();
    KCP_CPP_NS::LibStream stream;

    if(_enableUdpPacketDecorate)
    {// 需要装饰数据所以需要拷贝
        // g_Log->Debug(LOGFMT_OBJ_TAG("will decorate buf before send."));
        stream.InitBuffer(static_cast<UInt64>(len));
        _protocolStack->DecorateKcpRawSend(this, buf, len, stream);
    }
    else
    {// 不需要装饰只要Attach即可
        stream.AttachBuffer(const_cast<Byte8 *>(buf), len, 0, len);
    }

    return RawSend(stream);
}


KCP_CPP_NS::LibString KcpChannel::ToString() const
{
    KCP_CPP_NS::LibString info, dirtyFlagInfo, streamCtrlFlagInfo, stateInfo;

    // 脏标记
    // dirtyFlagInfo.AppendFormat("dirty flag:");
    // if(KCP_CPP_NS::BitUtil::IsSet(_dirtyFlag, KcpChannel::READ))
    //     dirtyFlagInfo.AppendFormat("READ:true, ");
    // else
    //     dirtyFlagInfo.AppendFormat("READ:false, ");
    // if(KCP_CPP_NS::BitUtil::IsSet(_dirtyFlag, KcpChannel::WRITE))
    //     dirtyFlagInfo.AppendFormat("WRITE:true, ");
    // else
    //     dirtyFlagInfo.AppendFormat("WRITE:false, ");

    // 流控
    streamCtrlFlagInfo.AppendFormat("stream ctrl flag:");
    if(KCP_CPP_NS::BitUtil::IsSet(_streamCtrlFlag, KcpChannel::LAZY_CLOSE))
        streamCtrlFlagInfo.AppendFormat("LAZY_CLOSE:true, ");
    else
        streamCtrlFlagInfo.AppendFormat("LAZY_CLOSE:false, ");
    if(KCP_CPP_NS::BitUtil::IsSet(_streamCtrlFlag, KcpChannel::FORBID_SEND))
        streamCtrlFlagInfo.AppendFormat("FORBID_SEND:true, ");
    else
        streamCtrlFlagInfo.AppendFormat("FORBID_SEND:false, ");
    if(KCP_CPP_NS::BitUtil::IsSet(_streamCtrlFlag, KcpChannel::FORBID_RECV))
        streamCtrlFlagInfo.AppendFormat("FORBID_RECV:true, ");
    else
        streamCtrlFlagInfo.AppendFormat("FORBID_RECV:false, ");

    // 状态
    if(_state == CHANNEL_ACTIVE)
        stateInfo.AppendFormat("channel state:CHANNEL_ACTIVE, ");
    else if(_state == CHANNEL_CREATED)
        stateInfo.AppendFormat("channel state:CHANNEL_CREATED, ");
    else
        stateInfo.AppendFormat("channel state:UNKNOWN_STATE, ");
    
    info.AppendFormat("sid:%llu, kcp conv:%d isListener:%u, remote:%s, heartbeat count:%lld, sock:%s,"
                    , _sessionId, _cb ? _cb->conv : 0, _isListener, _remoteEndpoint.ToString().c_str()
                    , _sucHeartbeatCount, _sock->ToString().c_str());

    info.AppendFormat("%s; %s; %s", stateInfo.c_str(), dirtyFlagInfo.c_str(), streamCtrlFlagInfo.c_str());

    return info;
}

Int32 KcpChannel::Init(bool isNonBlocking)
{    
    if (IsListener())
    {
        // 监控者会创建cb,非监控者需要在连接成功后的ack包获得conv后再创建cb
        auto errCode = _InitKcpCb();
        if (errCode != Status::Success)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("init kcp cb fail errCode:%d"), errCode);
            return errCode;
        }
    }

    g_Log->Info(LOGFMT_OBJ_TAG("new channel init %s."), ToString().c_str());
    return Status::Success;
}

Int32 KcpChannel::InitKcpCb(UInt64 conv)
{
    return _InitKcpCb(conv);
}

void KcpChannel::InitHeartbeatTimer(KCP_CPP_NS::LibTimer* timer)
{
    if (_heartbeatTimer)
        delete _heartbeatTimer;

    _heartbeatTimer = timer;
}

void KcpChannel::UpdateHeatbeatTimer(Int64 milliseconds)
{
    _UpdateHeartbeatCount();
    if (!_heartbeatTimer)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no heart beat timer channel:%s"), ToString().c_str());
        return;
    }

    _heartbeatTimer->Schedule(milliseconds);
}

Int32 KcpChannel::_InitKcpCb(UInt64 conv)
{
    if(_cb)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("kcp cb already created old conv:%u, new conv:%llu"), ikcp_getconv(_cb), conv);
        return Status::Repeat;
    }

    _cb = ikcp_create(static_cast<IUINT32>(conv), this);
    if(_cb == NULL)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("kcp create fail."));
        return Status::Error;
    }

    // 2.设置发送接口
    ikcp_setoutput(_cb, &KcpChannelOutput);

    // 3.配置参数(快速模式)
    ikcp_nodelay(_cb, 1, 10, 2, 1);

    // 4.设置最小rto
    _cb->rx_minrto = 10;

    return Status::Success;
}

void KcpChannel::_ContinueSend()
{
    if(!_cb)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no kcp cb channel:%s"), ToString().c_str());
        return;
    }

    MaskWriteDirty(false);
    // auto waitsnd = ikcp_waitsnd(_cb);
    // const auto doubleSnd = 2 * _cb->snd_wnd;

    // g_Log->Debug(LOGFMT_OBJ_TAG("continue send channel:%s _waitingSendMsgStream bytes:%llu, waitsnd:%d, double snd:%d")
    //             , ToString().c_str(), _waitingSendMsgStream.GetReadableBytes(), waitsnd, doubleSnd);

    // if(waitsnd >= doubleSnd)
    // {
    //     MaskWriteDirty();
    //     return;
    // }

    _SendStream(_waitingSendMsgStream);

    if(_waitingSendMsgStream.GetReadableBytes())
    {
        MaskWriteDirty();
        // g_Log->NetInfo(LOGFMT_OBJ_TAG("will send next frame bytes:%llu, channel:%s"), _waitingSendMsgStream.GetReadableBytes(), ToString().c_str());
    }

    _waitingSendMsgStream.Compress();
}

void KcpChannel::_SendStream(KCP_CPP_NS::LibStream &stream)
{
    if(!_cb)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no kcp cb channel:%s"), ToString().c_str());
        return;
    }
    
    auto bytes = stream.GetReadableBytes();
    const auto totalBytes = stream.GetReadableBytes();
    const auto maxLenPerTimes = ikcp_getmaxsendlen_pertimes(_cb);
    const auto doubleSndWnd = 2 * _cb->snd_wnd;

    // 4.超过发送长度的分多批次发送
    while(bytes)
    {
        // 大于两倍窗口停止发送
        auto curWaitSnd = ikcp_waitsnd(_cb);
        if (curWaitSnd >= static_cast<int>(doubleSndWnd))
        {
            g_Log->NetWarn(LOGFMT_OBJ_TAG("kcp net busy curWaitSnd:%d channel:%s, doubleSndWnd:%d")
                , curWaitSnd, ToString().c_str(), doubleSndWnd);
            break;
        }

        const auto bytesToSend = (bytes >= maxLenPerTimes) ? (maxLenPerTimes) : bytes; 
        auto err = ikcp_send(_cb, stream.GetReadBegin(), static_cast<int>(bytesToSend));
        if(err != 0)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("ikcp_send fail err:%d, channel:%s, opcode:%hu")
            , err, ToString().c_str());
        }

        stream.ShiftReadPos(bytesToSend);
        bytes -= bytesToSend;
    }

   //  g_Log->NetInfo(LOGFMT_OBJ_TAG("_SendStream total bytes:%llu, left bytes:%llu, real send:%llu, channel:%s"), totalBytes, bytes, totalBytes - bytes, ToString().c_str());
}

void KcpChannel::_OnReadable(KcpContext &ctx)
{
    MaskReadDirty(false);
    if(!CanRecv() || !IsActived())
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("cant recv msg or not actived channel:%s, stream data:%llu"), 
                ToString().c_str(), ctx._data.GetReadableBytes());
        return;
    }

//     KCP_CPP_NS::LibString hex;
//     KCP_CPP_NS::StringUtil::ToHexStringView(ctx._data, hex);
//     g_Log->NetInfo(LOGFMT_OBJ_TAG("on readable channel:%s,  ctx.endporint:%s data:%llu, hex:\n%s")
//                 , ToString().c_str(), ctx._endpoint.ToString().c_str(), ctx._data.GetReadableBytes(), hex.c_str());

    // flags
    auto &data = ctx._data;

    U8 flags = data.ReadUInt8();
    UInt64 conv = 0;
    if (data.GetReadableBytes() >= KcpFlags::BIT_CONV_SIZE)
        data.Read(&conv, KcpFlags::BIT_CONV_SIZE);

    if ((flags & (1 << KcpFlags::BIT_SYN)) != 0)
    {// 远程连接请求包
        if (_willConnectCallback)
        {
            if(_willConnectCallback->Invoke(this, ctx._endpoint))
            {
                g_Log->Info(LOGFMT_OBJ_TAG("new channel link in suc endpoint:%s"), ctx._endpoint.ToString().c_str());
            }
        }
        else
            g_Log->Warn(LOGFMT_OBJ_TAG("have no will connect callback channel:%s"), ToString().c_str());
    }
    else if ((flags & (1 << KcpFlags::BIT_ACK)) != 0)
    {// 连接请求被确认包 此时可以创建 kcpcb结构并传入conv
        if (IsListener())
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("listenr cant got a ack packet from remote channel:%s, conv:%llu, remote endpoint:%s.")
                , ToString().c_str(), conv, ctx._endpoint.ToString().c_str());

            return;
        }

        {// 只有非监控者才会接受应答确认连接
            if (conv == 0)
            {
                g_Log->Warn(LOGFMT_OBJ_TAG("error conv from kcp ack packet channel:%s"), ToString().c_str());
                return;
            }

            auto err = _InitKcpCb(conv);
            if (err != Status::Success)
            {
                g_Log->Error(LOGFMT_OBJ_TAG("channel init kcp cb fail err:%d, conv:%llu, channel:%s"), err, conv, ToString().c_str());
                return;
            }
        }

       // g_Log->Debug(LOGFMT_OBJ_TAG("success got a ack packt from remote conv:%llu, channel:%s"), conv, ToString().c_str());

        if (_sucGotAckFromRemoteKcp)
            _sucGotAckFromRemoteKcp->Invoke(this);
        else
            g_Log->Warn(LOGFMT_OBJ_TAG("have no suc got ack callback channel:%s"), ToString().c_str());
    }
    else
    {// 数据传输
        // 校验是否连接,校验是否创建 cb
        if (conv == 0)
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("bad conv:%llu channel:%s"), conv, ToString().c_str());
            return;
        }

        if (_netDataComeCallback)
            _netDataComeCallback->Invoke(conv, ctx);
        else
            g_Log->Warn(LOGFMT_OBJ_TAG("have no net data handler please check channel:%s"), ToString().c_str());
    }
    
}

void KcpChannel::OnReadable()
{
    std::vector<KcpContext *> ctxVector;
    do
    {
        KCP_CPP_NS::LibStream stream;
        stream.InitBuffer(_bufferCapicity);

        auto availableSize = _sock->GetAvailableReadBytesInsideSocket();
        if(availableSize == 0)
        {
            // g_Log->Debug(LOGFMT_OBJ_TAG("have no available data channel:%s"), ToString().c_str());
            break;
        }

        if(stream.GetWritableBytes() < availableSize)
            stream.AppendCapacity(availableSize);
        
        KCP_CPP_NS::EndPoint endpoint;
        Int32 errCode = KCP_CPP_NS::SocketStatus::valid;
        auto recvBytes = _sock->Recv(stream, &endpoint, errCode);
        if(errCode != KCP_CPP_NS::SocketStatus::valid)
        {
            if(errCode != KCP_CPP_NS::SocketStatus::non_blocking_would_have_blocked)
            {
                if(!IsListener())
                {
                    MaskClose();
                    ForbidRecv();
                    ForbidSend();
                }

                g_Log->Error(LOGFMT_OBJ_TAG("sock:%s errCode:%d"), _sock->ToString().c_str(), errCode);
            }
            
            break;
        }

        // 至少要一个字节数据
        if(stream.GetReadableBytes() == 0)
        {
            g_Log->Warn(LOGFMT_OBJ_TAG("have no data recv endpoint:%s"), endpoint.ToString().c_str());
            continue;
        }

        stream.Compress();
        auto newCtx = new KcpContext;
        newCtx->_data.Swap(stream);
        newCtx->_endpoint = endpoint;
        ctxVector.push_back(newCtx);
    } while (true);

    for(auto ctx :ctxVector)
    {
        _OnReadable(*ctx);
        delete ctx;
    }
}

void KcpChannel::OnReadable(KCP_CPP_NS::LibStream &stream)
{
    if(!_cb)
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("have no kcp cb channel:%s"), ToString().c_str());
        return;
    }

   MaskReadDirty(false);
    if(!CanRecv() || !IsActived())
    {
        g_Log->Warn(LOGFMT_OBJ_TAG("cant recv msg or not actived channel:%s, stream data:%llu"), ToString().c_str(), stream.GetReadableBytes());
        return;
    }

//     g_Log->NetDebug(LOGFMT_OBJ_TAG("on readable channel:%s, data:%llu")
//                 , ToString().c_str(), stream.GetReadableBytes());

    if(IsListener())
    {
        g_Log->Error(LOGFMT_OBJ_TAG("listener channel cant recv client data channel:%s"), ToString().c_str());
        return;
    }

    KCP_CPP_NS::LibString hex;
    KCP_CPP_NS::StringUtil::ToHexStringView(stream.GetReadBegin(), stream.GetReadableBytes(), hex);
    // g_Log->NetInfo(LOGFMT_OBJ_TAG("OnReadable recv raw data\n:%s"), hex.c_str());

    // 给kcp喂数据
    auto err = ikcp_input(_cb, stream.GetReadBegin(), static_cast<long>(stream.GetReadableBytes()));
    if(err != 0)
        g_Log->Warn(LOGFMT_OBJ_TAG("ikcp_input fail err:%d, channel:%s"), err, ToString().c_str());
    stream.ShiftReadPos(stream.GetReadableBytes());
    
    // kcp收到的数据总量
    auto recvBufferSize = ikcp_peeksize(_cb);
    if (recvBufferSize > 0)
    {
        if (_leftMsgStream.GetCapicity() == 0)
            _leftMsgStream.InitBuffer(recvBufferSize);
        if (_leftMsgStream.GetWritableBytes() < recvBufferSize)
            _leftMsgStream.AppendCapacity(recvBufferSize - _leftMsgStream.GetWritableBytes());

        // 接受到的数据
        auto unhandledRecvBytes = ikcp_recv(_cb, _leftMsgStream.GetWriteBegin(), static_cast<int>(_leftMsgStream.GetWritableBytes()));
        if (unhandledRecvBytes > 0)
        {
            _leftMsgStream.ShiftWritePos(unhandledRecvBytes);
            // g_Log->Debug(LOGFMT_OBJ_TAG("recv a unhandled data:%llu, channel:%s, user will handle recv data later."), unhandledRecvBytes, ToString().c_str());
        }
    }

    // 处理数据：必须激活了的channel才可以接收数据
    if(_leftMsgStream.GetReadableBytes())
    {
        // g_Log->Debug(LOGFMT_OBJ_TAG("recv total data bytes:%llu, and will handle it now channel:%s."), _leftMsgStream.GetReadableBytes(), ToString().c_str());

        // channel必须激活了才能处理消息
        if(_rcvCallback && IsActived())
            _rcvCallback->Invoke(this, _leftMsgStream);

        _leftMsgStream.Compress();
    }
}

void KcpChannel::OnWritable()
{
    // g_Log->Info(LOGFMT_OBJ_TAG("writable channel:%s"), ToString().c_str());
}

void KcpChannel::OnConnected(const KCP_CPP_NS::EndPoint& remoteEndpoint)
{
    _remoteEndpoint = remoteEndpoint;
    _UpdateHeartbeatCount();
    if(!IsListener())
    {
        auto err = _InitKcpCb();
    }
    g_Log->Info(LOGFMT_OBJ_TAG("OnConnected channel:%s"), ToString().c_str());
    if(_connectedCallback)
        _connectedCallback->Invoke(this);
}

void KcpChannel::OnDisconnected()
{
    g_Log->Info(LOGFMT_OBJ_TAG("OnDisconnected channel:%s"), ToString().c_str());
    if(_disconnectedCallback)
        _disconnectedCallback->Invoke(this);
}

void KcpChannel::OnUpdate()
{
    if(IsActived())
    {
        if(!_cb)
        {
            g_Log->NetWarn(LOGFMT_OBJ_TAG("have no kcp cb channel:%s"), ToString().c_str());
            return;
        }

        auto nowMill = KCP_CPP_NS::LibTime::NowMilliTimestamp();
        auto updateTime = ikcp_check(_cb, static_cast<IUINT32>(nowMill));
        if(updateTime <= nowMill)
        {
            // g_Log->Debug(LOGFMT_OBJ_TAG("channel invoke kcp update channel:%s"), ToString().c_str());
            ikcp_update(_cb, static_cast<IUINT32>(nowMill));

            if (IsDirty(KcpChannel::WRITE))
            {
                _ContinueSend();
            }
        }
    }
}
