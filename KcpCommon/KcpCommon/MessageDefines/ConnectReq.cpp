



#include "pch.h"
#include "KcpCommon/MessageDefines/ConnectReq.h"

void ConnectReq::Encode(KCP_CPP_NS::LibStream &stream)
{
    // g_Log->Info(LOGFMT_OBJ_TAG("ConnectReq encode finish"));
}

void ConnectReq::Decode(Byte8 *buffer, UInt64 bufferSize)
{    
    // g_Log->Info(LOGFMT_OBJ_TAG("ConnectReq decode finish."));
}

IProtocolCoder *ConnectReqFactory::Create()
{
    return new ConnectReq();
}