
#include "pch.h"
#include "KcpCommon/NetCommon/ProtocolCoder.h"

#include "KcpCommon/NetCommon/LibPacket.h"

LibPacket::~LibPacket()
{
    if(_coder)
        _coder->Release();

    _coder = NULL;
}

void LibPacket::SetCoder(IProtocolCoder *coder)
{
    if(_coder)
        _coder->Release();
    _coder = coder;
}

KCP_CPP_NS::LibString LibPacket::ToString() const
{
    KCP_CPP_NS::LibString info;
    info.AppendFormat("packet :opcode:%hu, sessionId:%llu, packetId:%llu"
                    , _opcode, _sessionId, _packetId);

    return info;
}
