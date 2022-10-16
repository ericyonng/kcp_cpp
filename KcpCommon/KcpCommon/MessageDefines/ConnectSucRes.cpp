
#include "pch.h"
#include "kcp/kcp_cpp.h"

#include "KcpCommon/MessageDefines/ConnectSucRes.h"


void ConnectSucRes::Encode(KCP_CPP_NS::LibStream &stream)
{
    // g_Log->Info(LOGFMT_OBJ_TAG("ConnectSucRes Encode"));
}

void ConnectSucRes::Decode(Byte8 *buffer, UInt64 bufferSize)
{
    // g_Log->Info(LOGFMT_OBJ_TAG("ConnectSucRes decode"));
}

IProtocolCoder *ConnectSucResFactory::Create()
{
    return new ConnectSucRes();
}