#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_PROTOCOL_LAYER_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_PROTOCOL_LAYER_H__

#pragma once

class ProtocolLayer
{
public:
    // 正序 body => 包头
    enum ENUMS
    {
        PrepareHeader = 0,  // 预备header
        MsgBody = 1,        // 消息体
        MsgHeader = 2,      // 包头
        MAX_SIZE,
    };
};

#endif
