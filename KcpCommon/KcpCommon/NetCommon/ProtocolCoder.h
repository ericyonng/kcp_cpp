

#ifndef __KCP_CPP_KCP_COMMON_KCP_COMMON_NET_COMMON_PROTOCOL_CODER_H__
#define __KCP_CPP_KCP_COMMON_KCP_COMMON_NET_COMMON_PROTOCOL_CODER_H__

#pragma once

#include "kcp/kcp_cpp.h"

class IProtocolCoder
{
public:
    IProtocolCoder(){}
    virtual ~IProtocolCoder() {}
    virtual void Encode(KCP_CPP_NS::LibStream &stream) = 0;
    virtual void Decode(Byte8 *buffer, UInt64 bufferSize) = 0;

    virtual void Release() = 0;
    virtual Int32 GetOpcode() const = 0;
    virtual KCP_CPP_NS::LibString GetOpcodeName() const { return KCP_CPP_NS::RttiUtil::GetByObj(this); }
};

class IProtocolCoderFactory
{
public:
    IProtocolCoderFactory(){}
    virtual ~IProtocolCoderFactory() {}

    virtual IProtocolCoder *Create() = 0;
    virtual void Release() = 0;
};

// 消息头
struct ProtocolHeader
{
    UInt32 _len = 0;        // 长度
    UInt16 _opcode = 0;     // 操作命令  
    Int64 _packetId = 0;    // 包id
};
// 消息包结构
struct ProtocolHeaderStructure
{
    static constexpr Int32 LEN_START_POS = 0;   // 起始位置
    static constexpr Int32 LEN_SIZE = 4;        // 4个字节长度

    static constexpr Int32 OPCODE_START_POS = LEN_START_POS + LEN_SIZE;   // 起始位置
    static constexpr Int32 OPCODE_SIZE = 2;        // 2个字节长度

    static constexpr Int32 PACKET_ID_START_POS = OPCODE_START_POS + OPCODE_SIZE;   // 起始位置
    static constexpr Int32 PACKET_ID_SIZE = 8;        // 8个字节长度

    static constexpr Int32 HEADER_SIZE = LEN_SIZE + OPCODE_SIZE + PACKET_ID_SIZE;   // 消息头大小

    static constexpr Int32 MSG_BODY_START_POS = HEADER_SIZE;   // 消息起始位置

    static constexpr Int32 TOTAL_MSG_BYTES_LIMIT = 16777215;        // 16MB整个包大小限制
};

enum class PacketType
{
    Invalid = 0,
    Request = 1,
    Response,
    Exception,
    Push,
};

#endif
