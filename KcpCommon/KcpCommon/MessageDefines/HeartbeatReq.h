
#ifndef __KCP_CPP_KCP_COMMON_HEARTBEAT_REQ_H__
#define __KCP_CPP_KCP_COMMON_HEARTBEAT_REQ_H__

#pragma once

#include "kcp/kcp_cpp.h"
#include "KcpCommon/NetCommon/ProtocolCoder.h"
#include "KcpCommon/MessageDefines/Opcodes.h"

class HeartbeatReq : public IProtocolCoder
{
public:
    HeartbeatReq() {}
    ~HeartbeatReq() {}

    virtual void Encode(KCP_CPP_NS::LibStream &stream) override;
    virtual void Decode(Byte8 *buffer, UInt64 bufferSize) override;
    virtual Int32 GetOpcode() const { return Opcodes::HeartbeatReq; }
    
    virtual void Release()
    {
        delete this;
    }
};


class HeartbeatReqFactory : public IProtocolCoderFactory
{
public:
    HeartbeatReqFactory() {}
    ~HeartbeatReqFactory() {}

    virtual IProtocolCoder *Create() override;

    virtual void Release() override
    {
        delete this;
    }
};

#endif
