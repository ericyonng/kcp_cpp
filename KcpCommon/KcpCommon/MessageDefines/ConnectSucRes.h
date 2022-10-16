

#ifndef __KCP_CPP_KCP_COMMON_CONNECT_SUC_RES_H__
#define __KCP_CPP_KCP_COMMON_CONNECT_SUC_RES_H__

#pragma once

#include "KcpCommon/NetCommon/ProtocolCoder.h"
#include "KcpCommon/MessageDefines/Opcodes.h"

class ConnectSucRes : public IProtocolCoder
{
public:
    ConnectSucRes(){}
    ~ConnectSucRes() {}

    virtual void Encode(KCP_CPP_NS::LibStream &stream) override;
    virtual void Decode(Byte8 *buffer, UInt64 bufferSize) override;
    virtual Int32 GetOpcode() const { return Opcodes::ConnectSucRes; }

    virtual void Release()
    {
        delete this;
    }
};

class ConnectSucResFactory : public IProtocolCoderFactory
{
public:
    ConnectSucResFactory() {}
    ~ConnectSucResFactory() {}

    virtual IProtocolCoder *Create() override;

    virtual void Release() override
    {
        delete this;
    }
};

#endif
