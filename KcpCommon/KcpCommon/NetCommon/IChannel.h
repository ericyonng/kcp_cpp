#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_ICHANNEL_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_ICHANNEL_H__

#pragma once

#include "kcp/kcp_cpp.h"

class IChannel
{
public:
    // 是否监听者
    virtual bool IsListener() const = 0;
    virtual SOCKET GetSocketRaw() const = 0;
};

#endif
