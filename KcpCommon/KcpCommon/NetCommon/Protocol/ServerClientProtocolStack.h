
#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_SERVER_CLIENT_STACK_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_SERVER_CLIENT_STACK_H__

#pragma once

#include "KcpCommon/NetCommon/Protocol/IProtocolStack.h"

class ServerClientProtocolStack : public IProtocolStack
{
public:
    ServerClientProtocolStack(){}
    ~ServerClientProtocolStack();

    virtual bool Encode(LibPacket *packet, KCP_CPP_NS::LibStream &dest);

    // return(bool):false:表示stream不能继续解码, 
    virtual void Decode(UInt64 sessionId, KCP_CPP_NS::LibStream &stream, std::vector<LibPacket *> &messageList, bool &hasBadMsg);

    virtual void DecorateKcpRawSend(KcpChannel *channel, const Byte8 *buf, int len, KCP_CPP_NS::LibStream &outputStream);

    virtual void RegisterOpcodeCoderFactory(UInt16 opcode, IProtocolCoderFactory *factory);

protected:
    IProtocolCoderFactory *_GetCoderFactory(UInt16 opcode);

protected:
    std::unordered_map<UInt16, IProtocolCoderFactory *> _opcodeRefCoderFactory;
};

inline void ServerClientProtocolStack::RegisterOpcodeCoderFactory(UInt16 opcode, IProtocolCoderFactory *factory)
{
    _opcodeRefCoderFactory.insert(std::make_pair(opcode, factory));
}

inline IProtocolCoderFactory *ServerClientProtocolStack::_GetCoderFactory(UInt16 opcode)
{
    auto iter = _opcodeRefCoderFactory.find(opcode);
    if(iter == _opcodeRefCoderFactory.end())
        return NULL;

    return iter->second;
}

#endif
