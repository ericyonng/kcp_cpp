#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_BIT_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_BIT_UTIL_H__

#pragma once

#include "kcp/common/common.h"

KCP_CPP_BEGIN

class BitUtil
{
public:
    template <typename ObjType, typename PosType>
    static inline bool IsSet(ObjType val, PosType pos)
    {
        return (val & (static_cast<ObjType>(1) << pos)) != 0;
    }

    template <typename ObjType, typename PosType>
    static inline ObjType Set(ObjType val, PosType pos)
    {
        return val | (static_cast<ObjType>(1) << pos);
    }

    template <typename ObjType, typename PosType>
    static inline ObjType Clear(ObjType val, PosType pos)
    {
        return val &= ~(static_cast<ObjType>(1) << pos);
    }
};

KCP_CPP_END

#endif
