
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_NET_DEFS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_NET_NET_DEFS_H__

#pragma once

#include <kcp/common/common.h>

// 网络
#ifndef INVALID_SOCKET
#define INVALID_SOCKET -1
#endif
#ifndef SOCKET_ERROR
#define SOCKET_ERROR -1
#endif

using SOCKADDR_IN				= sockaddr_in;
using SOCKADDR					= sockaddr;
using IN_ADDR					= in_addr;


KCP_CPP_BEGIN

struct AddrCollection
{
    sockaddr_storage _adrinf = {0};
    socklen_t _sockSize = 0;
};

KCP_CPP_END

#endif
