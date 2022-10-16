
#include "pch.h"
#include "kcp/kcp_cpp.h"

#include "KcpCommon/MessageDefines/HeartbeatReq.h"


void HeartbeatReq::Encode(KCP_CPP_NS::LibStream &stream)
{
    // g_Log->Info(LOGFMT_OBJ_TAG("HeartbeatReq Encode"));
}

void HeartbeatReq::Decode(Byte8 *buffer, UInt64 bufferSize)
{
    // g_Log->Info(LOGFMT_OBJ_TAG("HeartbeatReq decode"));
}

IProtocolCoder *HeartbeatReqFactory::Create()
{
    return new HeartbeatReq();
}