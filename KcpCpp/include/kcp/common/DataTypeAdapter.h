
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_DATA_TYPE_ADAPTER_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_DATA_TYPE_ADAPTER_H__

#pragma once

#include <kcp/common/macro.h>
#include <kcp/common/libs.h>
#include <kcp/common/BasicDataTrait.h>
#include <kcp/common/RemoveReference.h>

KCP_CPP_BEGIN

class LibDataType
{
public:
    enum ENUMS
    {
        UNKOWN_TYPE = 0,
        BRIEF_DATA_TYPE = 1,         // 基本数据类型:UInt64 Int64 等
        ARRAY_TYPE = 2,              // 数组类型
        UNION_TYPE = 3,              // 联合体
        CLASS_TYPE = 4,              // 类类型
        FUNCTION_TYPE = 5,           // 函数
        VOID_TYPE = 6,               // void类型
        POINTER_TYPE = 7,            // 指针类型
    };
};

template<class _Ty, _Ty Val>
class LibDataTypeConstant
{
public:
    static constexpr _Ty value = Val;
    using ValueType = _Ty;
    using Type = LibDataTypeConstant;
    constexpr operator ValueType() const noexcept
    {
        return (value);
    }
    _NODISCARD constexpr ValueType operator()() const noexcept
    {
        return (value);
    }
};

// 特化
template<LibDataType::ENUMS Val>
using DataTypeConstant = LibDataTypeConstant<LibDataType::ENUMS, Val>;

// 定义类型
using LibUnkownType     = DataTypeConstant<LibDataType::UNKOWN_TYPE>;
using LibBriefDataType  = DataTypeConstant<LibDataType::BRIEF_DATA_TYPE>;
using LibArrayType      = DataTypeConstant<LibDataType::ARRAY_TYPE>;
using LibUnionType      = DataTypeConstant<LibDataType::UNION_TYPE>;
using LibClassType      = DataTypeConstant<LibDataType::CLASS_TYPE>;
using LibFunctionType   = DataTypeConstant<LibDataType::FUNCTION_TYPE>;
using LibVoidType       = DataTypeConstant<LibDataType::VOID_TYPE>;
using LibPointerType    = DataTypeConstant<LibDataType::POINTER_TYPE>;

// 类型识别核心泛型
template<bool _IsConst, bool _IsBreifData, bool _IsArray, bool _IsUnion, bool _IsClass, bool _IsFunction, bool _IsVoid, bool _IsPointer>
class KernelTraitsDataType : public LibUnkownType
{};

// 特化基本数据类型
template<bool _IsConst>
class KernelTraitsDataType<
_IsConst, true, false, false, false, false, false, false> 
: public LibBriefDataType
{};

// 特化数组类型
template<bool _IsConst>
class KernelTraitsDataType<
_IsConst, false, true, false, false, false, false, false> 
: public LibArrayType
{};

// 特化联合类型
template<bool _IsConst>
class  KernelTraitsDataType<
_IsConst, false, false, true, false, false, false, false> 
: public LibUnionType
{};

// 特化类类型
template<bool _IsConst>
class KernelTraitsDataType<
_IsConst, false, false, false, true, false, false, false> 
: public LibClassType
{};

// 特化函数类型
template<bool _IsConst>
class  KernelTraitsDataType<
_IsConst, false, false, false, false, true, false, false> 
: public LibFunctionType
{};

// 特化void类型
template<bool _IsConst>
class  KernelTraitsDataType<
_IsConst, false, false, false, false, false, true, false> 
: public LibVoidType
{};

// 特化void类型
template<bool _IsConst, bool _IsBreifData, bool _IsArray, bool _IsUnion, bool _IsClass, bool _IsFunction, bool _IsVoid>
class  KernelTraitsDataType<
_IsConst, _IsBreifData, _IsArray, _IsUnion, _IsClass, _IsFunction, _IsVoid, true> 
: public LibPointerType
{};

// 统一的类型识别泛型
template<typename _Ty>
class LibTraitsDataType 
: public KernelTraitsDataType< std::is_const<_Ty>::value 
                        , std::is_basic_data<RemoveReferenceType<_Ty>>::value
                        , std::is_array<RemoveReferenceType<_Ty>>::value
                        , std::is_union<RemoveReferenceType<_Ty>>::value
                        , std::is_class<RemoveReferenceType<_Ty>>::value
                        , std::is_function<RemoveReferenceType<_Ty>>::value
                        , std::is_void<_Ty>::value
                        , std::is_pointer<RemoveReferenceType<_Ty>>::value>
{};

KCP_CPP_END

#endif
