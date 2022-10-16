
#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_IMESSAGE_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_IMESSAGE_H__

#pragma once

class IMessage
{
public:
    virtual void Release() = 0;
};

#endif
