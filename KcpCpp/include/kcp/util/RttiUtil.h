
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_RTTI_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_RTTI_UTIL_H__

#pragma once

#include "kcp/common/common.h"

KCP_CPP_BEGIN

class RttiUtil
{
public:
    template<typename ObjType>
    static const Byte8 *GetByType();
    template<typename ObjType>
    static const Byte8 *GetByObj(ObjType *obj);
    static const Byte8 *GetByTypeName(const char *rawTypeName);

    // 存在碰撞的可能性，不建议使用
    template<typename ObjType>
    static UInt64 GetTypeHashCode();

};

template<typename ObjType>
inline const Byte8 *RttiUtil::GetByType()
{
    return GetByTypeName(typeid(ObjType).name());
}

template<typename ObjType>
inline const Byte8 *RttiUtil::GetByObj(ObjType *obj)
{
    return GetByTypeName(typeid(*obj).name());
}

template<typename ObjType>
inline UInt64 RttiUtil::GetTypeHashCode()
{
    return typeid(ObjType).hash_code();
}

KCP_CPP_END


#endif
