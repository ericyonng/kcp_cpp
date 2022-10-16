
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_STRING_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_STRING_UTIL_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

class LibTime;
class LibStream;

class StringUtil
{
public:
	static Int32 StringToInt32(const char *str);
    static UInt32 StringToUInt32(const char *str);
    static Int16 StringToInt16(const char *str);
    static UInt16 StringToUInt16(const char *str);
    static Long StringToLong(const char *str);
    static ULong StringToULong(const char *str);
    static Int64 StringToInt64(const char *str);
    static UInt64 StringToUInt64(const char *str);
    static Double StringToDouble(const char *str);
    static LibString ItoA(Int32 value, Int32 radix);
    static LibString UItoA(UInt32 value, Int32 radix);
    static LibString I64toA(Int64 value, Int32 radix);
    static LibString UI64toA(UInt64 value, Int32 radix);
	static bool IsHex(U8 ch);

    template <typename ObjType>
    static LibString Num2Str(ObjType val, Int32 radix = 10);
    static bool ToHexString(const LibString &src, LibString &outHexString);
    static bool FromHexString(const LibString &hexString, LibString &outBin);
	// 加了换行 以便显示
    static bool ToHexStringView(const Byte8 *buff, Int64 len, LibString &outHexString);
    static bool ToHexStringView(const LibStream &stream, LibString &outHex);
    static void PreInstertTime(const LibTime &time, LibString &src);

	/**
	 * Split string using specific separator.
	 * @param[in]  str            - the source string.
	 * @param[in]  separator      - separator string.
	 * @param[out] destStrList    - sestination string list.
	 * @param[in]  justSplitFirst - split first flag, if true, when split one time, will stop.
	 * @param[in]  escapeChar     - escape character, default is '\0' 成功匹配 separator后剔除 separator之前的escapeChar字符.
	 * @param[in]  enableEmptyPart - 切割后是否允许存在空字符串
	 */
    static void SplitString(const LibString &str,
                     const LibString &separator,
                     std::vector<LibString> &destStrList,
                     bool justSplitFirst = false,
                     char escapeChar = '\0', bool enableEmptyPart = true);
    // src原文,start:第一个字符串,end第一个结尾
    static LibString CutString(const LibString &src, const LibString &start, const LibString &end);
    static LibString FilterOutString(const LibString &str, const LibString &filterStr);
	static bool CheckDoubleString(const LibString &str);

	static UInt64 CalcUtf8CharBytes(U8 ctrlChar);
};

inline Int32 StringUtil::StringToInt32(const char *str)
{
    return ::atoi(str);
}

inline UInt32 StringUtil::StringToUInt32(const char *str)
{
    return static_cast<UInt32>(StringToInt32(str));
}

inline Int16 StringUtil::StringToInt16(const char *str)
{
    return static_cast<Int16>(StringToInt32(str));
}

inline UInt16 StringUtil::StringToUInt16(const char *str)
{
    return static_cast<UInt16>(StringToInt32(str));
}

inline Long StringUtil::StringToLong(const char *str)
{
    return ::atol(str);
}

inline ULong StringUtil::StringToULong(const char *str)
{
    return static_cast<ULong>(StringToLong(str));
}

inline Int64 StringUtil::StringToInt64(const char *str)
{
    return ::_atoi64(str);
}

inline UInt64 StringUtil::StringToUInt64(const char *str)
{
    return static_cast<UInt64>(StringToInt64(str));
}

inline Double StringUtil::StringToDouble(const char *str)
{
    return ::atof(str);
}

inline LibString StringUtil::ItoA(Int32 value, Int32 radix)
{
    return I64toA(value, radix);
}

inline LibString StringUtil::UItoA(UInt32 value, Int32 radix)
{
    return UI64toA(value, radix);
}

#pragma endregion

template <>
inline LibString StringUtil::Num2Str(Int64 val, Int32 radix)
{
    return I64toA(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(UInt64 val, Int32 radix)
{
    return UI64toA(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(Int32 val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(UInt32 val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(Int16 val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(UInt16 val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(Byte8 val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(U8 val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(Long val, Int32 radix)
{
    return Num2Str<Int64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(ULong val, Int32 radix)
{
    return Num2Str<UInt64>(val, radix);
}

template <>
inline LibString StringUtil::Num2Str(Double val, Int32 radix)
{
    char buf[64] = {0};
    sprintf_s(buf, sizeof(buf), "%f", val);
    return buf;
}

template <>
inline LibString StringUtil::Num2Str(Float val, Int32 radix)
{
    return Num2Str<Double>(val, radix);
}

template <typename ObjType>
inline LibString StringUtil::Num2Str(ObjType val, Int32 radix)
{
    if(radix != 10 && radix != 16)
        radix = 10;

    LibString str;
    if(radix == 16)
        str.GetRaw() += "0x";

    UInt64 ptrVal = 0;
	auto objsz = sizeof(ObjType);
	auto u64sz = sizeof(UInt64);
    ::memcpy(&ptrVal, &val, objsz > u64sz ? u64sz : objsz);
    return (str + Num2Str<UInt64>(ptrVal, radix));
}

inline UInt64 StringUtil::CalcUtf8CharBytes(U8 ctrlChar)
{
	if ((ctrlChar & 0x80) == 0x00)
		return 1;
    // 110x xxxx
    // Encoding len: 2 bytes.
    else if ((ctrlChar & 0xe0) == 0xc0)
        return 2;
    // 1110 xxxx
    // Encoding len: 3 bytes.
    else if ((ctrlChar & 0xf0) == 0xe0)
        return 3;
    // 1111 0xxx
    // Encoding len: 4 bytes.
    else if ((ctrlChar & 0xf8) == 0xf0)
        return 4;
    // 1111 10xx
    // Encoding len: 5 bytes.
    else if ((ctrlChar & 0xfc) == 0xf8)
        return 5;
    // 1111 110x
    // Encoding len: 6 bytes.
    else if ((ctrlChar & 0xfe) == 0xfc)
        return 6;

	return 0;
}

KCP_CPP_END

#endif
