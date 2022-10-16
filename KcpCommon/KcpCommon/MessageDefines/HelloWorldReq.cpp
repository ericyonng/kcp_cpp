

#include "pch.h"
#include "KcpCommon/MessageDefines/HelloWorldReq.h"

static int g_count = 0;

void HelloWorldReq::Encode(KCP_CPP_NS::LibStream &stream)
{
    // 默认mtu
    // _content.AppendFormat("b");
    _content = "hello world";
    
    // _content.AppendFormat("e");

    UInt64 len = _content.size();
    if(len)
    {
        stream.Write(&len, 4);
        stream.Write(_content.data(), len);
    }

    _startTime = KCP_CPP_NS::LibTime::NowMilliTimestamp();
    stream.WriteInt64(_startTime);

    // g_Log->Info(LOGFMT_OBJ_TAG("HelloWorldReq encode finish content len:%llu"), len);
}

void HelloWorldReq::Decode(Byte8 *buffer, UInt64 bufferSize)
{
    KCP_CPP_NS::LibStream stream;
    stream.AttachBuffer(buffer, bufferSize, 0, bufferSize);
    
    UInt64 len = 0;
    stream.Read(&len, 4);
    if(len)
    {
        _content = std::string(stream.GetReadBegin(), len);
        stream.ShiftReadPos(len);
    }

    _startTime = stream.ReadInt64();
    
    // g_Log->Info(LOGFMT_OBJ_TAG("HelloWorldReq decode finish content len:%llu."), len);
}

IProtocolCoder *HelloWorldReqFactory::Create()
{
    return new HelloWorldReq();
}