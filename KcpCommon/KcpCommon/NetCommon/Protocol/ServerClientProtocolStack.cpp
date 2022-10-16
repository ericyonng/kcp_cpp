
#include "pch.h"
#include "KcpCommon/NetCommon/Protocol/IMessage.h"
#include "KcpCommon/NetCommon/ProtocolCoder.h"
#include "KcpCommon/NetCommon/LibPacket.h"
#include "KcpCommon/NetCommon/KcpChannel.h"

#include "KcpCommon/NetCommon/Protocol/ServerClientProtocolStack.h"

ServerClientProtocolStack::~ServerClientProtocolStack()
{
    for(auto iter:_opcodeRefCoderFactory)
        iter.second->Release();
    _opcodeRefCoderFactory.clear();
}

bool ServerClientProtocolStack::Encode(LibPacket *packet, KCP_CPP_NS::LibStream &dest)
{
    KCP_CPP_NS::LibStream stream;
    stream.InitBuffer(1);
    auto coder = packet->GetCoder();
    coder->Encode(stream);

    // 包头
    ProtocolHeader header;
    header._opcode = packet->GetOpcode();
    header._packetId = packet->GetPacketId();
    header._len = static_cast<UInt32>(stream.GetReadableBytes() + ProtocolHeaderStructure::HEADER_SIZE - ProtocolHeaderStructure::LEN_SIZE);

    if(dest.GetCapicity() == 0)
        dest.AppendCapacity(ProtocolHeaderStructure::HEADER_SIZE);

    dest.Write(&header._len, ProtocolHeaderStructure::LEN_SIZE);
    dest.Write(&header._opcode, ProtocolHeaderStructure::OPCODE_SIZE);
    dest.Write(&header._packetId, ProtocolHeaderStructure::PACKET_ID_SIZE);

    dest.Write(stream.GetReadBegin(), stream.GetReadableBytes());

//     g_Log->NetInfo(LOGFMT_OBJ_TAG("encode opcode:%hu, packetId:%lld, len:%u")
//                     , header._opcode, header._packetId, header._len);
    return true;
}

void ServerClientProtocolStack::Decode(UInt64 sessionId, KCP_CPP_NS::LibStream &stream, std::vector<LibPacket *> &messageList, bool &hasBadMsg)
{
    hasBadMsg = false;
    Int64 packetId = 0;
    Int32 opcode = 0;
    UInt64 sucLen = 0;
    while(true)
    {
        // 托管保护原始流
        KCP_CPP_NS::LibStream steamCache;
        steamCache.AttachBuffer(stream.GetReadBegin(), stream.GetReadableBytes(), 0, stream.GetReadableBytes());

        // 1.解码包头
        ProtocolHeader header;
        {
            // 1.1判断可否解码包头
            if(steamCache.GetReadableBytes() < ProtocolHeaderStructure::HEADER_SIZE)
            {
                // g_Log->Debug(LOGFMT_OBJ_TAG("msg header len not enough."));
                break;
            }

            // 2.包头足够校验包长度
            steamCache.Read(&header._len, ProtocolHeaderStructure::LEN_SIZE);
            steamCache.Read(&header._opcode, ProtocolHeaderStructure::OPCODE_SIZE);
            steamCache.Read(&header._packetId, ProtocolHeaderStructure::PACKET_ID_SIZE);
            if(header._len > ProtocolHeaderStructure::TOTAL_MSG_BYTES_LIMIT)
            {
                g_Log->Error(LOGFMT_OBJ_TAG("msg total size over limit msg size:%llu, limit:%d, opcode:%hu, packet id:%lld")
                , header._len, ProtocolHeaderStructure::TOTAL_MSG_BYTES_LIMIT, header._opcode, header._packetId);
                g_Log->NetError(LOGFMT_OBJ_TAG("decode error session id:%llu, bad header len"), sessionId);
                hasBadMsg = true;
                break;
            }

            if ((header._len - ProtocolHeaderStructure::HEADER_SIZE + ProtocolHeaderStructure::LEN_SIZE) > steamCache.GetReadableBytes())
            {// 包还没完整到来
                // g_Log->Debug(LOGFMT_OBJ_TAG("packet not a complate pack wait next time sessionId:%llu."), sessionId);
                break;
            }

            // 3.包id校验
            if(header._packetId < 0)
            {
                g_Log->Error(LOGFMT_OBJ_TAG("bad packet id msg size:%llu, limit:%d, opcode:%hu, packet id:%lld")
                , header._len, ProtocolHeaderStructure::TOTAL_MSG_BYTES_LIMIT, header._opcode, header._packetId);
                g_Log->NetError(LOGFMT_OBJ_TAG("decode error session id:%llu, bad pakcet id"), sessionId);
                hasBadMsg = true;
                break;
            }
        }
        
        // 2.解码包体
        auto coderFactory = _GetCoderFactory(header._opcode);
        if(!coderFactory)
        {
            g_Log->Error(LOGFMT_OBJ_TAG("msg coder factory not found opcode:%hu, packetId:%llu, len:%u")
                        , header._opcode, header._packetId, header._len);

            g_Log->NetError(LOGFMT_OBJ_TAG("decode error session id:%llu, have no coder factory"), sessionId);
            hasBadMsg = true;
            break;
        }

        // 3.解码器
        auto coder = coderFactory->Create();
        coder->Decode(steamCache.GetReadBegin(), header._len - ProtocolHeaderStructure::HEADER_SIZE + ProtocolHeaderStructure::LEN_SIZE);
        steamCache.ShiftReadPos(header._len - ProtocolHeaderStructure::HEADER_SIZE + ProtocolHeaderStructure::LEN_SIZE);
        stream.ShiftReadPos(header._len + ProtocolHeaderStructure::LEN_SIZE);

        // 4.生成包
        LibPacket *newPacket = new LibPacket;
        newPacket->SetOpcode(header._opcode);
        newPacket->SetCoder(coder);
        newPacket->SetSessionId(sessionId);
        newPacket->SetPacketId(header._packetId);
        messageList.push_back(newPacket);
        packetId = header._packetId;
        opcode = header._opcode;
        sucLen = header._len;

//         g_Log->Info(LOGFMT_OBJ_TAG("suc packet: sessionId:%llu packetId:%lld, opcode:%d,%s sucLen:%llu")
//                         , sessionId, packetId, opcode, coder->GetOpcodeName().c_str(), sucLen + ProtocolHeaderStructure::LEN_SIZE);
    }
//     g_Log->NetInfo(LOGFMT_OBJ_TAG("protocol stack decode %llu msgs, sessionId:%llu, hasBadMsg:%d, last suc packetId:%lld, last suc opcode:%d, sucLen:%llu")
//                 , messageList.size(), sessionId, hasBadMsg, packetId, opcode, sucLen + ProtocolHeaderStructure::LEN_SIZE);

}

void ServerClientProtocolStack::DecorateKcpRawSend(KcpChannel *channel, const Byte8 *buf, int len, KCP_CPP_NS::LibStream &outputStream)
{
    // 每个udp包结构：
    // |    1byt      |  4byte  |   ...   |
    // | KcpStateFlag | KcpConv |   data  |

    auto conv = channel->GetConv();
    if(conv == 0)
    {
        g_Log->Error(LOGFMT_OBJ_TAG("bad conv cant decorate channel:%s, buf len:%d"), channel->ToString().c_str(), len);
        return;
    }
    U8 flag = 0;
    outputStream.Write(&flag, KcpFlags::BIT_FLAGS_SIZE);
    outputStream.Write(&conv, KcpFlags::BIT_CONV_SIZE);
    outputStream.Write(buf, len);

    KCP_CPP_NS::LibString hex;
    KCP_CPP_NS::StringUtil::ToHexStringView(outputStream.GetReadBegin(), outputStream.GetReadableBytes(), hex);
    // g_Log->NetInfo(LOGFMT_OBJ_TAG("decorate kcp raw data channel:%s, data:\n%s "), channel->ToString().c_str(), hex.ToString().c_str());
}
