
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_ILOG_FACTORY_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_ILOG_FACTORY_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

class ILog;

class ILogFactory
{
public:
    ILogFactory() {}
    virtual ~ILogFactory() {}

    virtual ILog *Create() = 0;
};

KCP_CPP_END

#endif
