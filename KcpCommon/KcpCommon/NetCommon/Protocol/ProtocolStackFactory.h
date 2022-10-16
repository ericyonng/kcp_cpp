

#ifndef __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_PROTOCOL_STACK_FACTORY_H__
#define __KCP_CPP_KCP_COMMON_NET_COMMON_PROTOCOL_PROTOCOL_STACK_FACTORY_H__

#pragma once

#include "kcp/kcp_cpp.h"

class IProtocolStack;

class ProtocolStackFactory
{
public:
    static IProtocolStack *Create(Int32 protocolStackType);
};

#endif
