

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_REMOVE_REFERENCE_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_REMOVE_REFERENCE_H__

#pragma once

#include <kcp/common/macro.h>

KCP_CPP_BEGIN

template<typename _Ty>
struct RemoveReference
{
using type = typename _Ty;
};

template<typename _Ty>
struct RemoveReference<_Ty &>
{
using type = typename _Ty;
};

template<typename _Ty>
struct RemoveReference<const _Ty &>
{
using type = typename _Ty;
};

template<typename _Ty>
struct RemoveReference<_Ty &&>
{
using type = typename _Ty;
};

template<typename _Ty>
using RemoveReferenceType = typename RemoveReference<_Ty>::type;

KCP_CPP_END

#endif

