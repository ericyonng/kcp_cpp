
#ifndef __KCP_CPP_KCP_COMMON_KCP_COMMON_NET_COMMON_LIB_PACKET_H__
#define __KCP_CPP_KCP_COMMON_KCP_COMMON_NET_COMMON_LIB_PACKET_H__

#pragma once

class IProtocolCoder;

class LibPacket
{
public:
    LibPacket() {}
    ~LibPacket();

    void SetCoder(IProtocolCoder *coder);
    IProtocolCoder *GiveupCoder();
    IProtocolCoder *GetCoder();
    const IProtocolCoder *GetCoder() const;

    UInt16 GetOpcode() const;
    void SetOpcode(UInt16 opcode);

    UInt64 GetSessionId() const;
    void SetSessionId(UInt64 sessionId);

    void SetPacketId(Int64 packetId);
    Int64 GetPacketId() const;

    KCP_CPP_NS::LibString ToString() const;

private:
    UInt16 _opcode = 0;
    UInt64 _sessionId = 0;
    Int64 _packetId = 0;
    IProtocolCoder* _coder = NULL;
};

inline IProtocolCoder *LibPacket::GiveupCoder()
{
    auto coder = _coder;
    _coder = NULL;
    return coder;
}

inline IProtocolCoder *LibPacket::GetCoder()
{
    return _coder;
}

inline const IProtocolCoder *LibPacket::GetCoder() const
{
    return _coder;
}

inline UInt16 LibPacket::GetOpcode() const
{
    return _opcode;
}

inline void LibPacket::SetOpcode(UInt16 opcode)
{
    _opcode = opcode;
}

inline UInt64 LibPacket::GetSessionId() const
{
    return _sessionId;
}

inline void LibPacket::SetSessionId(UInt64 sessionId)
{
    _sessionId = sessionId;
}

inline void LibPacket::SetPacketId(Int64 packetId)
{
    _packetId = packetId;
}

inline Int64 LibPacket::GetPacketId() const
{
    return _packetId;
}
#endif
