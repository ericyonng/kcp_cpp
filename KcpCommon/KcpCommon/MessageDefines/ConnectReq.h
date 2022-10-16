



#ifndef __KCP_CPP_KCP_COMMON_CONNECT_REQ_H__
#define __KCP_CPP_KCP_COMMON_CONNECT_REQ_H__

#pragma once

#include "kcp/kcp_cpp.h"
#include "KcpCommon/NetCommon/ProtocolCoder.h"
#include "KcpCommon/MessageDefines/Opcodes.h"

class ConnectReq : public IProtocolCoder
{
public:
    ConnectReq() {}
    ~ConnectReq() {}
    
    virtual void Encode(KCP_CPP_NS::LibStream &stream) override;
    virtual void Decode(Byte8 *buffer, UInt64 bufferSize) override;
    virtual Int32 GetOpcode() const { return Opcodes::ConnectReq; }
    
    virtual void Release()
    {
        delete this;
    }
};


class ConnectReqFactory : public IProtocolCoderFactory
{
public:
    ConnectReqFactory() {}
    ~ConnectReqFactory() {}

    virtual IProtocolCoder *Create() override;

    virtual void Release() override
    {
        delete this;
    }
};

#endif