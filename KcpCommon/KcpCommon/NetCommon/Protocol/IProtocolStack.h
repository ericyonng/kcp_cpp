
#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_IPROTOCOL_STACK_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_IPROTOCOL_STACK_H__

#pragma once

#include "kcp/kcp_cpp.h"

class IProtocolCoderFactory;
class LibPacket;
class KcpChannel;

class IProtocolStack
{
public:
    IProtocolStack() {}
    virtual ~IProtocolStack() {}
    virtual bool Encode(LibPacket *packet, KCP_CPP_NS::LibStream &stream) = 0;
    virtual void Decode(UInt64 sessionId, KCP_CPP_NS::LibStream &stream, std::vector<LibPacket *> &messageList, bool &hasBadMsg) = 0;

    // 装饰kcp
    virtual void DecorateKcpRawSend(KcpChannel *channel, const Byte8 *buf, int len, KCP_CPP_NS::LibStream &outputStream) = 0;

    virtual void RegisterOpcodeCoderFactory(UInt16 opcode, IProtocolCoderFactory *factory) = 0;
};

#endif
