
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_META_LOCKER_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_META_LOCKER_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LockerHandle.h>

KCP_CPP_BEGIN

class MetaLocker
{
public:
    MetaLocker();
    virtual ~MetaLocker();
    virtual void Release();

public:
    LockerHandle _handle;
};

KCP_CPP_END

#endif
