

#ifndef __KCP_CPP_KCP_COMMON_HELLO_WORLD_RES_H__
#define __KCP_CPP_KCP_COMMON_HELLO_WORLD_RES_H__

#pragma once


#include "kcp/kcp_cpp.h"
#include "KcpCommon/NetCommon/ProtocolCoder.h"
#include "KcpCommon/MessageDefines/Opcodes.h"

class HelloWorldRes : public IProtocolCoder
{
public:
    HelloWorldRes(){}
    ~HelloWorldRes(){}
    
    virtual void Encode(KCP_CPP_NS::LibStream &stream) override;
    virtual void Decode(Byte8 *buffer, UInt64 bufferSize) override;
    virtual Int32 GetOpcode() const { return Opcodes::HelloWorldRes; }
    
    virtual void Release()
    {
        delete this;
    }

    KCP_CPP_NS::LibString _content;
    Int64 _startTime = 0;
};


class HelloWorldResFactory : public IProtocolCoderFactory
{
public:
    HelloWorldResFactory() {}
    ~HelloWorldResFactory() {}

    virtual IProtocolCoder *Create() override;

    virtual void Release() override
    {
        delete this;
    }
};
#endif
