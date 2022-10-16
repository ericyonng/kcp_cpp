
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_BASIC_DATA_TRAIT_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_BASIC_DATA_TRAIT_H__

#pragma once

#include <kcp/common/macro.h>
#include <kcp/common/libs.h>

namespace std { 

template<class _Ty>
struct is_basic_data : public false_type
{};

template<>
struct is_basic_data<bool> : public true_type
{};
template<>
struct is_basic_data<bool &> : public true_type
{};
template<>
struct is_basic_data<bool &&> : public true_type
{};
template<>
struct is_basic_data<const bool> : public true_type
{};
template<>
struct is_basic_data<const bool &> : public true_type
{};

template<>
struct is_basic_data<U8> : public true_type
{};
template<>
struct is_basic_data<U8 &> : public true_type
{};
template<>
struct is_basic_data<U8 &&> : public true_type
{};
template<>
struct is_basic_data<const U8> : public true_type
{};
template<>
struct is_basic_data<const U8 &> : public true_type
{};

template<>
struct is_basic_data<Byte8> : public true_type
{};
template<>
struct is_basic_data<Byte8 &> : public true_type
{};
template<>
struct is_basic_data<Byte8 &&> : public true_type
{};
template<>
struct is_basic_data<const Byte8> : public true_type
{};
template<>
struct is_basic_data<const Byte8 &> : public true_type
{};

template<>
struct is_basic_data<UInt16> : public true_type
{};
template<>
struct is_basic_data<UInt16 &> : public true_type
{};
template<>
struct is_basic_data<UInt16 &&> : public true_type
{};
template<>
struct is_basic_data<const UInt16> : public true_type
{};
template<>
struct is_basic_data<const UInt16 &> : public true_type
{};

template<>
struct is_basic_data<Int16> : public true_type
{};
template<>
struct is_basic_data<Int16 &> : public true_type
{};
template<>
struct is_basic_data<Int16 &&> : public true_type
{};
template<>
struct is_basic_data<const Int16> : public true_type
{};
template<>
struct is_basic_data<const Int16 &> : public true_type
{};

template<>
struct is_basic_data<UInt32> : public true_type
{};
template<>
struct is_basic_data<UInt32 &> : public true_type
{};
template<>
struct is_basic_data<UInt32 &&> : public true_type
{};
template<>
struct is_basic_data<const UInt32> : public true_type
{};
template<>
struct is_basic_data<const UInt32 &> : public true_type
{};

template<>
struct is_basic_data<Int32> : public true_type
{};
template<>
struct is_basic_data<Int32 &> : public true_type
{};
template<>
struct is_basic_data<Int32 &&> : public true_type
{};
template<>
struct is_basic_data<const Int32> : public true_type
{};
template<>
struct is_basic_data<const Int32 &> : public true_type
{};

template<>
struct is_basic_data<UInt64> : public true_type
{};
template<>
struct is_basic_data<UInt64 &> : public true_type
{};
template<>
struct is_basic_data<UInt64 &&> : public true_type
{};
template<>
struct is_basic_data<const UInt64> : public true_type
{};
template<>
struct is_basic_data<const UInt64 &> : public true_type
{};

template<>
struct is_basic_data<Int64> : public true_type
{};
template<>
struct is_basic_data<Int64 &> : public true_type
{};
template<>
struct is_basic_data<Int64 &&> : public true_type
{};
template<>
struct is_basic_data<const Int64> : public true_type
{};
template<>
struct is_basic_data<const Int64 &> : public true_type
{};

template<>
struct is_basic_data<Float> : public true_type
{};
template<>
struct is_basic_data<Float &> : public true_type
{};
template<>
struct is_basic_data<Float &&> : public true_type
{};
template<>
struct is_basic_data<const Float> : public true_type
{};
template<>
struct is_basic_data<const Float &> : public true_type
{};

template<>
struct is_basic_data<Double> : public true_type
{};
template<>
struct is_basic_data<Double &> : public true_type
{};
template<>
struct is_basic_data<Double &&> : public true_type
{};
template<>
struct is_basic_data<const Double> : public true_type
{};
template<>
struct is_basic_data<const Double &> : public true_type
{};

}

#endif
