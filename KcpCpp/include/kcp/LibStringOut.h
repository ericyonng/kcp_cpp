
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_STRING_OUT_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_STRING_OUT_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

// 
// ALWAYS_INLINE KERNEL_EXPORT void AppendFormatString(std::string &o, const Byte8 *fmt, ...)
// {
//     // if fmt args is null, return.
//     if (UNLIKELY(!fmt))
//         return;
// 
//     va_list va;
//     va_start(va, fmt);
//     do
//     {
//         // try detach detach format require buffers and resize it.
//         const UInt64 oldSize = o.size();
//         Int32 len =::vsnprintf(nullptr, 0, fmt, va);
//         if (len <= 0)
//             break;
// 
//         // exec format.
//         o.resize(oldSize + len);
//         len = ::vsnprintf(const_cast<Byte8 *>(o.data() + oldSize),
//                             len + 1,
//                             fmt,
//                             va);
// 
//         // len < 0 then o.size() - len > oldSize back to old string
//         if (UNLIKELY(oldSize != (o.size() - len)))
//         {
//             CRYSTAL_TRACE("wrong apend format and back to old string, oldSize:%llu, len:%d, new size:%llu", oldSize, len, o.size());
//             o.resize(oldSize);
//         }
//     } while (0);
//     
//     va_end(va);
// }
// 

// 流输出必须区分,指针,pod类型
template<typename _Ty, LibDataType::ENUMS _DataType>
struct KernelStringOutAdapter;

template<typename _Ty>
struct KernelStringOutAdapter<_Ty, LibDataType::BRIEF_DATA_TYPE>
{
public:
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &&input)
//     {
//         return o << input;
//     }
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &input)
//     {
//         return o << input;
//     }
    template<typename Rtn>
    static Rtn &output(Rtn &o, const _Ty &input)
    {
        return o << input;
    }
};
// Array 输出Address
template<typename _Ty>
struct KernelStringOutAdapter<_Ty, LibDataType::ARRAY_TYPE>
{
public:
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &&input)
//     {
//         return o.AppendFormat("%p", input);
//     }
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &input)
//     {
//         return o.AppendFormat("%p", input);
//     }
    template<typename Rtn>
    static Rtn &output(Rtn &o, const _Ty &input)
    {
        return o.AppendFormat("%p", input);
    }
};
// class 需要支持ToString() const;
template<typename _Ty>
struct KernelStringOutAdapter<_Ty, LibDataType::CLASS_TYPE>
{
public:
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &&input)
//     {
//         o += input.ToString();
//         return o;
//     }
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &input)
//     {
//         o += input.ToString();
//         return o;
//     }
    template<typename Rtn>
    static Rtn &output(Rtn &o, const _Ty &input)
    {
        o += input.ToString();
        return o;
    }
};

// Function 输出地址
template<typename _Ty>
struct KernelStringOutAdapter<_Ty, LibDataType::FUNCTION_TYPE>
{
public:
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &&input)
//     {
//         return o.AppendFormat("%p", input);
//     }
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &input)
//     {
//         return o.AppendFormat("%p", input);
//     }
    template<typename Rtn>
    static Rtn &output(Rtn &o, const _Ty &input)
    {
        return o.AppendFormat("%p", input);
    }
};


// Pointer 输出地址
template<typename _Ty>
struct KernelStringOutAdapter<_Ty, LibDataType::POINTER_TYPE>
{
public:
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &&input)
//     {
//         return o.AppendFormat("%p", input);
//     }
//     template<typename Rtn>
//     static Rtn &output(Rtn &o, _Ty &input)
//     {
//         return o.AppendFormat("%p", input);
//     }
    template<typename Rtn>
    static Rtn &output(Rtn &o, const _Ty &input)
    {
        return o.AppendFormat("%p", input);
    }
};

template<typename _Ty>
struct StringOutAdapter : KernelStringOutAdapter<_Ty, LibTraitsDataType<_Ty>::value>
{};

KCP_CPP_END

#endif
