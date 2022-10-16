

#include "pch.h"
#include "KcpCommon/MessageDefines/HelloWorldRes.h"

void HelloWorldRes::Encode(KCP_CPP_NS::LibStream &stream)
{
    UInt64 len = _content.length();
    if(len)
    {
        stream.Write(&len, 4);
        stream.Write(_content.data(), len);
    }

    stream.WriteInt64(_startTime);

    // g_Log->Info(LOGFMT_OBJ_TAG("HelloWorldRes encode finish content len:%llu"), len);
}

void HelloWorldRes::Decode(Byte8 *buffer, UInt64 bufferSize)
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
    
    // g_Log->Info(LOGFMT_OBJ_TAG("HelloWorldRes decode finish content len:%llu."), len);
}

IProtocolCoder *HelloWorldResFactory::Create()
{
    return new HelloWorldRes();
}