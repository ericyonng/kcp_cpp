#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_KCP_CONTEXT_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_KCP_CONTEXT_H__

#pragma once

#include "kcp/kcp_cpp.h"

struct KcpContext
{
    KCP_CPP_NS::EndPoint _endpoint;
    KCP_CPP_NS::LibStream _data;
};

#endif
