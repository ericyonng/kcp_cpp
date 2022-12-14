#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_STRING_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_STRING_H__

#pragma once

#include "kcp/common/common.h"
#include "kcp/LibStringOut.h"

KCP_CPP_BEGIN

class LibString
{
public:
    typedef std::vector<LibString> _These, LibStrings;
    typedef std::string::size_type size_type;
    typedef LibString _ThisType;
    typedef Byte8 _Elem;

    static const Byte8 *endl;
    static const std::string _defStripChars;

public:
    LibString();
    LibString(LibString &&other);
    LibString(const LibString &other);
    LibString(const std::string &other);
    LibString(const Byte8 *other);
    LibString(const Byte8 *other, UInt64 cacheSize);
    LibString(Byte8 other);

    #pragma region 运算符
    LibString &operator = (const Byte8 *other);
    LibString &operator = (const std::string &other);
    LibString &operator = (const LibString &other);

    LibString operator + (const LibString &other) const;
    LibString operator + (const Byte8 *other) const; 
    LibString operator + (const std::string &other) const;
    LibString &operator += (const LibString &other);
    LibString &operator += (const Byte8 *other);
    LibString &operator += (const std::string &other);
    LibString &operator *= (size_t right);

    LibString &operator << (const LibString &str);
    LibString &operator << (LibString &&str);
    LibString &operator << (const std::string &str);
    LibString &operator << (std::string &&str);
    LibString &operator << (const bool &val);
    LibString &operator << (const Byte8 &val);
    LibString &operator << (const U8 &val);
    LibString &operator << (const Int16 &val);
    LibString &operator << (const UInt16 &val);
    LibString &operator << (const Int32 &val);
    LibString &operator << (const UInt32 &val);
    LibString &operator << (const Long &val);
    LibString &operator << (const ULong &val);
    LibString &operator << (const Int64 &val);
    LibString &operator << (const UInt64 &val);
    LibString &operator << (const Float &val);
    LibString &operator << (const Double &val);
    LibString &operator << (const Byte8 *val);
    LibString &operator << (const void *addr); 
    LibString &operator << (void *&&addr);
    template<typename T>
    LibString &operator <<(T &&value);
    template<typename T>
    LibString &operator <<(const T &value);

    Byte8 &operator [] (UInt64 index);
    const Byte8 &operator [] (UInt64 index) const;
    Byte8 &at (UInt64 index);
    const Byte8 &at (UInt64 index) const;

    bool operator == (const LibString &other) const;
    bool operator == (const std::string &other) const;
    bool operator == (const Byte8 *other) const;
    bool operator != (const Byte8 *other) const;
    bool operator != (const LibString &other) const;
    bool operator < (const LibString &right) const;

    friend std::basic_ostream<_Elem> &operator <<(std::basic_ostream<_Elem> &o, const _ThisType &str)
    {
        o.write(str.data(), str.size());
        return o;
    }

    // TODO:测试const std::string &a = LibString()是否会走该接口
//     operator std::string &();
//     operator const std::string &() const;
//     operator const Byte8 *() const;
    #pragma endregion

    size_t CopyTo(char *destData, UInt64 destSize, UInt64 cntToCopy, UInt64 srcOffset = 0) const;
    // '\0'结尾
    const Byte8 *c_str() const;
    // 没有'\0'结尾
    const Byte8 *data() const;
    bool empty() const;
    size_t size() const;
    size_t length() const;
    void clear();
    void resize(UInt64 bytes);
    std::string &GetRaw();
    const std::string &GetRaw() const;

    LibString ToHexString() const;    
    void ToHexString(LibString &target) const;   
    void ToHexView(LibString &target) const; 
    bool FromHexString(const LibString &hexString);
    void Swap(LibString &&str);
    void CompressString();                  // 去除字符串末尾的\0压缩空间只对0结尾的字符串有效
    const LibString &RemoveZeroTail();      // 移除容器中尾部的0
    const LibString &RemoveHeadZero();      // 移除容器中首部的0

    LibString &AppendData(const Byte8 *bitData, Int64 dataSize);
    LibString &AppendFormat(const Byte8 *fmt, ...);
    LibString &AppendFormatWithVaList(const Byte8 *fmt, va_list va);
    LibString &AppendEnd();

    // 支持任意类型的追加只要对象重载<<即可
    template< typename... Args>
    LibString &Append(Args&&... rest);
    // 当参数为0时会匹配非泛型版本
    LibString &Append();

    _ThisType &findreplace(const _Elem &dest, const _Elem &with, Int32 count = -1);
    _ThisType &findreplace(const _ThisType &dest, const _ThisType &with, Int32 count = -1);

    _ThisType &EraseAnyOf(const _ThisType &dest);

    // @param(sep):切割关键字符
    // @param(max_split):切割成多少份
    // @param(enableEmptyPart):允许空字符串
    _These Split(Byte8 sep, size_type max_split = -1, bool enableEmptyPart = true) const;    // only ascii max_split分割的次数，一次分割两块数据一共分割max_split+1块数据
    // @param(sep):切割关键字符串
    // @param(max_split):切割成多少份
    // @param(onlyLikely):true:模糊匹配（匹配字符串中的某个字符即可）, false:必须完全匹配字符串
    // @param(enableEmptyPart):允许空字符串
    _These Split(const Byte8 *sep, size_type max_split = -1, bool onlyLikely = false, bool enableEmptyPart = true) const;    // only ascii
    // @param(sep):切割关键字符串
    // @param(max_split):切割成多少份
    // @param(onlyLikely):true:模糊匹配（匹配字符串中的某个字符即可）, false:必须完全匹配字符串
    // @param(enableEmptyPart):允许空字符串
    _These Split(const LibString &sep, std::string::size_type max_split = -1, bool onlyLikely = false, bool enableEmptyPart = true) const;
    // @param(seps):切割关键字符串， 完全匹配这组字符串中的一个即可
    // @param(max_split):切割成多少份
    // @param(enableEmptyPart):允许空字符串
    _These Split(const _These &seps, size_type max_split = -1, bool enableEmptyPart = true) const;
    

    // strip operation: strip left. 去首部连续字符
    LibString &lstrip(const LibString &chars = LibString());
    LibString lstrip(const LibString &chars = LibString()) const;

    // strip operation: strip right. 去尾部连续字符
    LibString &rstrip(const LibString &chars = LibString());
    LibString rstrip(const LibString &chars = LibString()) const;

    // strip operation: 去除首尾字符
    LibString &strip(const LibString &chars = LibString());
    LibString strip(const LibString &chars = LibString()) const;

    // isalpha/isupper/islower 是否字母
    static bool isalpha(const char &c);
    static bool isalpha(const LibString &s);
    bool isalpha() const;
    static bool islower(const char &c);
    static bool islower(const LibString &s);
    bool islower() const;
    static bool isupper(const char &c);
    static bool isupper(const LibString &s);
    bool isupper() const;
    // 是否数值
    static bool isdigit(const char &c);
    static bool isdigit(const LibString &s);
    bool isdigit() const;
    // isspace: space[' '], carriage return['\r'], line feed['\n'], form feed['\f'], horizontal tab['\t'], vertical tab['\v']
    static bool isspace(const char &c);
    static bool isspace(const LibString &s);
    bool isspace() const;

    // startswith/endswith
    bool IsStartsWith(const LibString &s) const;
    bool IsEndsWith(const LibString &s) const;

    // tolower/toupper operations. 请确保是英文字符串 isalpha
    LibString tolower() const;
    LibString toupper() const;

    // escape support: escape string
    _ThisType &escape(const _ThisType &willbeEscapeChars, const _Elem &escapeChar);
    _ThisType escape(const _ThisType &willbeEscapeChars, const _Elem &escapeChar) const;
    // escape support: unescape string
    _ThisType &unescape(const _Elem &escapeChar);
    _ThisType unescape(const _Elem &escapeChar) const;

    #pragma region UTF8 Surport
    // 添加bomb
    void add_utf8_bomb();
    // utf8字符个数
    std::string::size_type length_with_utf8() const;
    // 截取utf8字符串pos是按照utf8字符算的（不是按照字节）
    _ThisType substr_with_utf8(std::string::size_type pos = 0, std::string::size_type n = std::string::npos) const;
    // 从第charIndex个字符拆分成两个utf8字符串（前charIndex个utf8字符在strs的第一个）
    void split_utf8_string(size_type charIndex, _These &strs) const;
    // 打散ut8f字符串 scatterCount:按顺序打散n次，生产scatterCount个utf8字符串
    void scatter_utf8_string(_These &chars, std::string::size_type scatterCount = 0) const;
    // utf8有没带bomb（windows下utf8识别带bomb（字符串前三个字节：\xef\xbb\xbf））
    bool has_utf8_bomb() const;
    // 移除bomb
    void remove_utf8_bomb();

    std::string ToString() const
    {
        return _raw;
    }
    
private:
    // 下一个utf8字符索引pos
    std::string::size_type _next_utf8_char_pos(std::string::size_type &beginBytePos) const;
    // hex=>decimal
    U8 _TurnDecimal(const Byte8 hexChar);

    #pragma endregion
private:
    std::string _raw;
};

inline  LibString::LibString()
{
}

inline  LibString::LibString(LibString &&other)
{
    _raw.swap(other._raw);
}

inline  LibString::LibString(const Byte8 *other, UInt64 cacheSize)
:_raw(other, cacheSize)
{
}

inline  LibString::LibString(const LibString &other)
{
    _raw = other._raw;
}

inline  LibString::LibString(const std::string &other)
{
    _raw = other;
}

inline  LibString::LibString(const Byte8 *other)
{
    _raw = other;
}

inline  LibString::LibString(Byte8 other)
{
    _raw = other;
}

inline  LibString &LibString::operator = (const Byte8 *other)
{
    _raw = other;
    return *this;
}

inline  LibString &LibString::operator = (const std::string &other)
{
    _raw = other;
    return *this;
}

inline  LibString &LibString::operator = (const LibString &other)
{
    _raw = other._raw;
    return *this;
}

inline  LibString LibString::operator + (const LibString &other) const
{
    return _raw + other._raw;
}

inline  LibString LibString::operator + (const Byte8 *other) const
{
    return _raw + other;
}

inline  LibString LibString::operator + (const std::string &other) const
{
    return _raw + other;
}

inline  LibString &LibString::operator += (const LibString &other)
{
    _raw += other._raw;
    return *this;
}

inline  LibString &LibString::operator += (const Byte8 *other)
{
    _raw += other;
    return *this;
}

inline  LibString &LibString::operator += (const std::string &other)
{
    _raw += other;
    return *this;
}

inline  LibString &LibString::operator *= (size_t right)
{
    if (_raw.empty() || right == 1)
        return *this;
        
    if (right == 0)
    {
        _raw.clear();
        return *this;
    }

    _ThisType unitStr(*this);
    std::string &unitRaw = unitStr._raw;

    const _Elem *unitStrBuf = unitRaw.data();
    typename LibString::size_type unitStrSize = unitRaw.size();

    _raw.resize(unitStrSize * right);
    _Elem *buf = const_cast<_Elem *>(_raw.data());
    for (size_type i = 1; i < right; ++i)
        ::memcpy(buf + i * unitStrSize, unitStrBuf, unitStrSize * sizeof(_Elem));

    return *this;
}

inline  LibString &LibString::operator << (const LibString &str)
{
    _raw += str._raw;
    return *this;
}

inline  LibString &LibString::operator << (LibString &&str)
{
    _raw.append(str._raw);
    if (UNLIKELY(this != &str))
        str.clear();
    
    return *this;
}

inline  LibString &LibString::operator << (const std::string &str)
{
    _raw += str;
    return *this;
}

inline  LibString &LibString::operator << (std::string &&str)
{
    _raw.append(str);
    if (UNLIKELY(&_raw != &str))
        str.clear();

    return *this;
}

inline  LibString &LibString::operator << (const bool &val)
{
    BUFFER4 cache;
    auto len = sprintf(cache, "%d", val);
    cache[len] = 0;
    _raw += cache;
    return *this;
}

inline  LibString &LibString::operator << (const Byte8 &val)
{
    BUFFER8 cache;
    auto len = sprintf(cache, "%d", val);
    cache[len] = 0;
    _raw += cache;
    return *this;
}

inline  LibString &LibString::operator << (const U8 &val)
{
    BUFFER8 cache;
    auto len = sprintf(cache, "%u", val);
    cache[len] = 0;
    _raw += cache;
    return *this;
}

inline  LibString &LibString::operator << (const Int16 &val)
{
    BUFFER8 cache;
    auto len = sprintf(cache, "%hd", val);
    cache[len] = 0;
    _raw += cache;
    return *this;
}

inline  LibString &LibString::operator << (const UInt16 &val)
{
    BUFFER8 cache;
    auto len = sprintf(cache, "%hu", val);
    cache[len] = 0;
    _raw += cache;
    return *this;
}

inline  LibString &LibString::operator << (const Int32 &val)
{
    return AppendFormat("%d", val);
}

inline  LibString &LibString::operator << (const UInt32 &val)
{
    return AppendFormat("%u", val);
}

inline  LibString &LibString::operator << (const Long &val)
{
    return AppendFormat("%ld", val);
}

inline  LibString &LibString::operator << (const ULong &val)
{
    return AppendFormat("%lu", val);
}

inline  LibString &LibString::operator << (const Int64 &val)
{
    return AppendFormat("%lld", val);
}

inline  LibString &LibString::operator << (const UInt64 &val)
{
    return AppendFormat("%llu", val);
}

inline  LibString &LibString::operator << (const Float &val)
{
    return AppendFormat("%f", val);
}

inline  LibString &LibString::operator << (const Double &val)
{
    return AppendFormat("%lf", val);
}

inline  LibString &LibString::operator << (const Byte8 *val)
{
    if(UNLIKELY(!val))
        return *this;

    _raw += val;
    return *this;
}

inline  LibString &LibString::operator << (const void *addr)
{
    return AppendFormat("%p", addr);
}

inline  LibString &LibString::operator << (void *&&addr)
{
    return AppendFormat("%p", addr);
}

template<typename T>
inline  LibString &LibString::operator <<(T &&value)
{
    return StringOutAdapter<T>::output(*this, std::forward<T>(value));
}

template<typename T>
inline  LibString &LibString::operator <<(const T &value)
{
    return StringOutAdapter<T>::output(*this, value);
}

inline  Byte8 &LibString::operator [] (UInt64 index)
{
    return _raw[index];
}

inline  const Byte8 &LibString::operator [] (UInt64 index) const
{
    return _raw[index];
}

inline  Byte8 &LibString::at (UInt64 index)
{
    return _raw.at(index);
}

inline  const Byte8 &LibString::at (UInt64 index) const
{
    return _raw.at(index);
}

inline  bool LibString::operator == (const LibString &other) const
{
    return _raw == other._raw;
}

inline  bool LibString::operator == (const std::string &other) const
{
    return _raw == other;
}

inline  bool LibString::operator == (const Byte8 *other) const
{
    return _raw == other;
}

inline  bool LibString::operator != (const Byte8 *other) const
{
    return _raw != other;
}

inline  bool LibString::operator != (const LibString &other) const
{
    return _raw != other._raw;
}

inline  bool LibString::operator < (const LibString &right) const
{
    return _raw < right._raw;
}
// 
// inline  LibString::operator std::string &()
// {
//     return _raw;
// }
// 
// inline  LibString::operator const std::string &() const
// {
//     return _raw;
// }
// 
// inline  LibString::operator const Byte8 *() const
// {
//     return _raw.c_str();
// }

inline  size_t LibString::CopyTo(char *destData, UInt64 destSize, UInt64 cntToCopy, UInt64 srcOffset) const
{
    const UInt64 maxLen = std::min<UInt64>(destSize, cntToCopy);
    ::memcpy(destData, _raw.data() + srcOffset, maxLen);

    return maxLen;
}

inline  const Byte8 *LibString::c_str() const
{
    return _raw.c_str();
}

inline  const Byte8 *LibString::data() const
{
    return _raw.data();
}

inline  bool LibString::empty() const
{
    return _raw.empty();
}

inline  size_t LibString::size() const
{
    return _raw.size();
}

inline  size_t LibString::length() const
{
    return _raw.length();
}

inline  void LibString::clear()
{
    _raw.clear();
}

inline  void LibString::resize(UInt64 bytes)
{
    _raw.resize(bytes);
}

inline  std::string &LibString::GetRaw()
{
    return _raw;
}

inline  const std::string &LibString::GetRaw() const
{
    return _raw;
}

inline  LibString LibString::ToHexString() const
{
    LibString info;
    ToHexString(info);
    return info;
}

inline  void LibString::ToHexString(LibString &target) const
{
    const Int64 bufferSize = static_cast<Int64>(_raw.size());
    if(bufferSize == 0)
        return;

    std::string info;
    char cache[4] = {0};
    info.reserve(bufferSize * 2);
    for(Int64 i = 0; i < bufferSize; ++i)
    {
        cache[0] = 0;
#ifdef _WIN32
        const Int32 len = ::sprintf_s(cache, sizeof(cache), "%02x", static_cast<U8>(_raw[i]));
#else
        const Int32 len = ::sprintf(cache, "%02x", static_cast<U8>(_raw[i]));
#endif
        cache[len] = 0;
        info.append(cache);
    }

    target += info;
} 

inline  void LibString::ToHexView(LibString &target) const
{
    const Int64 bufferSize = static_cast<Int64>(_raw.size());
    if(bufferSize == 0)
        return;

    auto &targetRaw = target.GetRaw();
    targetRaw.reserve(target.size() + (bufferSize * 3));

    // 每16字节一行
    Byte8 cache[8] = { 0 };
    Int32 cacheLen = 0;
    for (Int64 i = 0; i < bufferSize; ++i)
    {
        cacheLen = ::sprintf(cache, "%02x%s"
            , static_cast<U8>(_raw[i]), ((i + 1) % 16 == 0) ? "\n" : " ");

        cache[cacheLen] = 0;
        targetRaw.append(cache);
    }
}

inline  U8 LibString::_TurnDecimal(const Byte8 hexChar)
{
    switch (hexChar)
    {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'a': 
    case 'A': return 10;
    case 'b': 
    case 'B': return 11;
    case 'c': 
    case 'C': return 12;
    case 'd': 
    case 'D': return 13;
    case 'e': 
    case 'E': return 14;
    case 'f': 
    case 'F': return 15;
    default:
        break;
    }

    throw std::logic_error("_TurnDecimal hex char not standard hex number");

    return 0;
}

inline  bool LibString::FromHexString(const LibString &hexString)
{
    const UInt64 hexLen = hexString.size();
    if(UNLIKELY(hexLen == 0 || ((hexLen % 2) != 0) ))
        return false;

    _raw.reserve(_raw.size() + hexLen / 2);
    for(UInt64 idx = 0; idx < hexLen; idx += 2)
    {
        U8 decimalNumber = 0;
        decimalNumber = _TurnDecimal(hexString[idx]);
        decimalNumber <<= 4;
        decimalNumber |= _TurnDecimal(hexString[idx + 1]);
        _raw.append(reinterpret_cast<Byte8 *>(&decimalNumber), 1);
    }

    return true;
}

inline  void LibString::Swap(LibString &&str)
{
    if (UNLIKELY(this == &str))
        return;
    _raw.swap(str._raw);
}

inline  void LibString::CompressString()  
{
    const auto strSize = _raw.size();
    if(strSize == 0)
        return;

    auto len = strlen(_raw.c_str());
    if(strSize > len)
    {
        _raw.erase(len + 1, strSize - len - 1);
    }
}                
    
inline  const LibString &LibString::RemoveZeroTail()
{
    const Int64 bufferSize = static_cast<Int64>(_raw.size());
    if(bufferSize == 0)
        return *this;

    auto pos = _raw.find_first_of((const char)(0), 0);
    if(pos == std::string::npos)
        return *this;

    _raw.erase(pos, bufferSize - pos);
    return *this;
}

inline  const LibString &LibString::RemoveHeadZero()
{
    const Int64 bufferSize = static_cast<Int64>(_raw.size());
    if(bufferSize == 0)
        return *this;

    auto pos = _raw.find_first_not_of((const char)(0), 0);
    if(pos == std::string::npos)
        return *this;

    _raw.erase(0, pos);
    return *this;
}

inline  LibString &LibString::AppendData(const Byte8 *bitData, Int64 dataSize)
{
    _raw.append(bitData, dataSize);
    return *this;
}

// inline  LibString &LibString::AppendFormat(const Byte8 *fmt, ...)
// {
//     va_list ap;
//     va_start(ap, fmt);
//     StringUtil::snprintf(_raw, fmt, ap);
//     va_end(ap);
// 
// 	return *this;
// }

inline  LibString &LibString::AppendFormat(const Byte8 *fmt, ...)
{
    // TODO:比较优化前后性能
    // char *buf; int len;
    // MemoryPool *pool = KernelGetTlsMemoryPool();
    // __CRYSTAL_BuildFormatStr_(fmt, buf, len, pool, _Build::TL::Type);
    // _raw.append(buf, len);
    // pool->Free(buf);
    va_list va;
    va_start(va, fmt);
    this->AppendFormatWithVaList(fmt, va);
    va_end(va);

    return *this;
}

inline  LibString &LibString::AppendFormatWithVaList(const Byte8 *fmt, va_list va)
{
    // if fmt args is null, return.
    if (UNLIKELY(!fmt))
        return *this;

    // try detach detach format require buffers and resize it.
    const size_type oldSize = _raw.size();
    Int32 len =::vsnprintf(nullptr, 0, fmt, va);
    if (len <= 0)
        return *this;

    // exec format.
    _raw.resize(oldSize + len);
    len = ::vsnprintf(const_cast<Byte8 *>(_raw.data() + oldSize),
                        len + 1,
                        fmt,
                        va);

    // len < 0 then _raw.size() - len > oldSize back to old string
    if (UNLIKELY(oldSize != (_raw.size() - len)))
    {
        CRYSTAL_TRACE("wrong apend format and back to old string, oldSize:%llu, len:%d, new size:%llu", oldSize, len, _raw.size());
        _raw.resize(oldSize);
    }

    return *this;
}

inline  LibString &LibString::AppendEnd()
{
    _raw += LibString::endl;
    return *this;
}

template< typename... Args>
inline  LibString &LibString::Append(Args&&... rest)
{
    // 使用数组展开参数包，并同时调用<<
    char expandArray[] = {((*this) << rest, char(0))...};
    return *this;
}

inline  LibString &LibString::Append()
{
    return *this;
}

inline  LibString::_ThisType &LibString::findreplace(const _Elem &dest, const _Elem &with, Int32 count)
{
    if (dest == with)
        return *this;

    for (size_type i = 0; i < this->size(); ++i)
    {
        if (_raw[i] == dest)
            _raw.replace(i, 1, 1, with);
    }

    return *this;
}

inline  LibString::_ThisType &LibString::findreplace(const _ThisType &dest, const _ThisType &with, Int32 count)
{
    if (dest == with)
        return *this;

    size_type found = 0;
    const std::string &destRaw = dest._raw;
    const std::string &withRaw = with._raw;
    while ((found = _raw.find(destRaw, found)) != std::string::npos)
    {
        _raw.replace(found, destRaw.size(), withRaw);
        found += withRaw.size();
    }

    return *this;
}

inline  LibString::_ThisType &LibString::EraseAnyOf(const _ThisType &dest)
{
    std::tuple<float, int> tup;
    auto a = std::get<0>(tup);
    auto &destRaw = dest.GetRaw();
    const UInt64 len = destRaw.length();
    UInt64 curPos = 0;
    for (UInt64 idx = 0; idx < len; ++idx)
    {
        do 
        {
            curPos = _raw.find(destRaw[idx]);
            if (curPos != std::string::npos)
                _raw.erase(curPos, 1);
        } while (curPos != std::string::npos);
    }

    return *this;
}

inline  LibString::_These LibString::Split(Byte8 sep, size_type max_split, bool enableEmptyPart) const
{
    return this->Split(LibString(sep), max_split, false, enableEmptyPart);
}

inline  LibString::_These LibString::Split(const Byte8 *sep, size_type max_split, bool onlyLikely, bool enableEmptyPart) const
{
    return this->Split(LibString(sep), max_split, onlyLikely, enableEmptyPart);
}

inline  LibString::_These LibString::Split(const LibString &sep, std::string::size_type max_split, bool onlyLikely, bool enableEmptyPart) const
{
    _These substrs;
    if(sep.empty() || max_split == 0 || this->empty())
    {
        if(!this->empty() || enableEmptyPart)
          substrs.push_back(*this);

        return substrs;
    }

    size_type idx = 0;
    UInt32 splitTimes = 0;
    const std::string &sepRaw = sep._raw;
    const UInt64 stepSize = onlyLikely ? 1 : sepRaw.size();
    for(; splitTimes < static_cast<UInt32>(max_split); ++splitTimes)
    {
        size_type findIdx = std::string::npos;
        if(onlyLikely)
        {
            for(size_t i = 0; i < sepRaw.size(); i++)
            {
                findIdx = _raw.find(sepRaw[i], idx);
                if(findIdx != std::string::npos)
                    break;
            }
        }
        else
        {
            findIdx = _raw.find(sepRaw, idx);
        }

        if(findIdx == std::string::npos)
            break;
        
        if (findIdx == idx)
        {
            if(enableEmptyPart)
                substrs.push_back(_ThisType());

            if ((idx = findIdx + stepSize) == this->size())
            {
                if (enableEmptyPart)
                    substrs.push_back(_ThisType());
                break;
            }

            continue;
        }

        substrs.push_back(_raw.substr(idx, findIdx - idx));

        if((idx = findIdx + stepSize) == this->size())
        {
            if(enableEmptyPart)
               substrs.push_back(_ThisType());

            break;
        }
    }

    // 还有剩余
    if(idx != _raw.size())
    {
        const auto &subStr = _raw.substr(idx);
        if(!subStr.empty() || enableEmptyPart)
            substrs.push_back(subStr);
    }

    return substrs;
}

inline  LibString::_These LibString::Split(const _These &seps, size_type max_split, bool enableEmptyPart) const
{
   _These substrs;
    if(seps.empty() || max_split == 0 || this->empty())
    {
        if(!this->empty() || enableEmptyPart)
           substrs.push_back(*this);
        return substrs;
    }

    size_type idx = 0;
    UInt32 splitTimes = 0;
    std::set<size_type> minIdx;
    for(; splitTimes < static_cast<UInt32>(max_split); ++splitTimes)
    {
        size_type findIdx = std::string::npos;
        minIdx.clear();
        for(size_t i = 0; i < seps.size(); i++)
        {
            findIdx = _raw.find(seps[i]._raw, idx);
            if(findIdx != std::string::npos)
                minIdx.insert(findIdx);
        }

        if(!minIdx.empty())
            findIdx = *minIdx.begin();

        if(findIdx == std::string::npos)
            break;

        substrs.push_back(_raw.substr(idx, findIdx - idx));
        if((idx = findIdx + 1) == this->size())
        {
            if(enableEmptyPart)
               substrs.push_back(_ThisType());
            break;
        }
    }

    if(idx != this->size())
    {
        const auto &subStr = _raw.substr(idx);
        if(!subStr.empty() || enableEmptyPart)
            substrs.push_back(subStr);
    }

    return substrs;
}

inline  LibString &LibString::lstrip(const LibString &chars)
{
    _ThisType willStripChars = chars;
    if (chars.empty())
    {
        willStripChars._raw.append(reinterpret_cast<const _Elem *>(" \t\v\r\n\f"));
    }

    std::string &thisRaw = _raw;
    size_type stripTo = 0;
    std::string &willStripRaw = willStripChars._raw;
    const size_type thisSize = static_cast<size_type>(thisRaw.size());
    for (size_type i = 0; i != thisSize; ++i)
    {
        bool found = false;
        const _Elem &now = thisRaw[i];
        for (size_type j = 0; j != willStripRaw.size(); ++j)
        {
            if (now == willStripRaw[j])
            {
                found = true;
                break;
            }
        }

        if (found)
            stripTo = i + 1;
        else
            break;
    }

    if (stripTo != 0)
        thisRaw.erase(0, stripTo);

    return *this;
}

inline  LibString LibString::lstrip(const LibString &chars) const
{
    LibString copyThis(*this);
    return copyThis.lstrip(chars);
}

inline  LibString &LibString::rstrip(const LibString &chars)
{
    _ThisType willStripChars = chars;
    if (chars.empty())
        willStripChars._raw.append(reinterpret_cast<const _Elem *>(" \t\v\r\n\f"));

    std::string &thisRaw = _raw;
    std::string &willStripRaw = willStripChars._raw;
    const size_type thisSize = static_cast<size_type>(thisRaw.size());

    size_type stripFrom = thisSize;
    for (size_type i = thisSize - 1; i >= 0; --i)
    {
        bool found = false;
        const _Elem &now = thisRaw[i];
        for (size_type j = 0; j != willStripRaw.size(); ++j)
        {
            if (now == willStripRaw[j])
            {
                found = true;
                break;
            }
        }

        if (found)
            stripFrom = i;
        else
            break;
    }

    if (stripFrom != thisSize)
        thisRaw.erase(stripFrom);

    return *this;
}

inline  LibString LibString::rstrip(const LibString &chars) const
{
    _ThisType copyThis(*this);
    return copyThis.rstrip(chars);
}

inline  LibString &LibString::strip(const LibString &chars)
{
    return this->lstrip(chars).rstrip(chars);
}

inline  LibString LibString::strip(const LibString &chars) const
{
    _ThisType copyThis(*this);
    return copyThis.lstrip(chars).rstrip(chars);
}

inline  bool LibString::isalpha(const char &c)
{
    return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z');
}

inline  bool LibString::isalpha(const LibString &s)
{
    if(s.empty())
        return false;

    const std::string &sRaw = s._raw;
    const size_type sSize = sRaw.size();
    for(size_t i = 0; i < sSize; i++)
    {
        if(!isalpha(sRaw[i]))
            return false;
    }

    return true;
}

inline  bool LibString::isalpha() const
{
    return isalpha(*this);
}

inline  bool LibString::islower(const char &c)
{
    return 'a' <= c && c <= 'z';
}

inline  bool LibString::islower(const LibString &s)
{
    if(s.empty())
        return false;

    bool foundLower = false;
    const std::string &sRaw = s._raw;
    const size_type sSize = sRaw.size();
    for(size_type i = 0; i < sSize; i++)
    {
        if(isupper(sRaw[i]))
            return false;
        else if(islower(sRaw[i]))
            foundLower = true;
    }

    return foundLower;
}

inline  bool LibString::islower() const
{
    return islower(*this);
}

inline  bool LibString::isupper(const char &c)
{
    return 'A' <= c && c <= 'Z';
}

inline  bool LibString::isupper(const LibString &s)
{
    if(s.empty())
        return false;

    bool foundUpper = false;
    const std::string &sRaw = s._raw;
    const size_type sSize = sRaw.size();
    for(size_type i = 0; i < sSize; i++)
    {
        if(islower(sRaw[i]))
            return false;
        else if(isupper(sRaw[i]))
            foundUpper = true;
    }

    return foundUpper;
}

inline  bool LibString::isupper() const
{
    return isupper(*this);
}

inline  bool LibString::isdigit(const char &c)
{
    return '0' <= c && c <= '9';
}

inline  bool LibString::isdigit(const LibString &s)
{
    if(s.empty())
        return false;

    const std::string &sRaw = s._raw;
    const size_type sSize = sRaw.size();
    for(size_type i = 0; i < sSize; ++i)
    {
        if(!isdigit(sRaw[i]))
            return false;
    }

    return true;
}

inline  bool LibString::isdigit() const
{
    return isdigit(*this);
}

inline  bool LibString::isspace(const char &c)
{
    return  c == ' ' || c == '\t' || c == '\v' || c == '\r' || c == '\n' || c == '\f';
}

inline  bool LibString::isspace(const LibString &s)
{
    if(s.empty())
        return false;

    const std::string &sRaw = s._raw;
    const size_type sSize = sRaw.size();
    for(size_type i = 0; i < sSize; i++)
    {
        if(!isspace(sRaw[i]))
            return false;
    }

    return false;
}

inline  bool LibString::isspace() const
{
    return isspace(*this);
}

inline  bool LibString::IsStartsWith(const LibString &s) const
{
    if (s.empty())
        return true;

    const std::string &sRaw = s._raw;
    return (_raw.size() >= sRaw.size() && memcmp(sRaw.data(), _raw.data(), sRaw.size() * sizeof(_Elem)) == 0);
}

inline  bool LibString::IsEndsWith(const LibString &s) const
{
    if (s.empty())
        return true;

    const std::string &sRaw = s._raw;
    return (_raw.size() >= sRaw.size() && 
        memcmp(sRaw.data(), _raw.data() + (_raw.size() - sRaw.size()) * sizeof(_Elem), sRaw.size() * sizeof(_Elem)) == 0);
}

// tolower/toupper operations. 请确保是英文字符串 isalpha
inline  LibString LibString::tolower() const
{
    const _Elem *buf = _raw.data();
    const size_type size = this->size();

    _ThisType lower;
    std::string &lowerRaw = lower._raw;
    lowerRaw.resize(size);
    for (size_type i = 0; i < size; ++i)
    {
        if (buf[i] >= 0x41 && buf[i] <= 0x5A)
            lowerRaw[i] = buf[i] + 0x20;
        else
            lowerRaw[i] = buf[i];
    }

    return lower;
}

inline  LibString LibString::toupper() const
{
    const _Elem *buf = this->data();
    const size_type size = this->size();

    _ThisType upper;
    std::string &upperRaw = upper._raw;
    upperRaw.resize(size);
    for (size_type i = 0; i < size; ++i)
        if (buf[i] >= 0x61 && buf[i] <= 0x7a)
            upperRaw[i] = buf[i] - 0x20;
        else
            upperRaw[i] = buf[i];

    return upper;
}

// escape support: escape string
inline  LibString::_ThisType &LibString::escape(const _ThisType &willbeEscapeChars, const _Elem &escapeChar)
{
    if (this->empty())
        return *this;

    const long len = static_cast<long>(this->size());
    std::string &thisRaw = _raw;
    const std::string &willbeEscapeRaw = willbeEscapeChars._raw;
    for (long i = len - 1; i >= 0; --i)
    {
        const _Elem &ch = thisRaw[i];
        if (ch == escapeChar ||
            willbeEscapeRaw.find(ch) != std::string::npos)
            thisRaw.insert(i, 1, escapeChar);
    }

    return *this;
}

inline  LibString::_ThisType LibString::escape(const _ThisType &willbeEscapeChars, const _Elem &escapeChar) const
{
    if (_raw.empty())
        return *this;

    return _ThisType(*this).escape(willbeEscapeChars, escapeChar);
}

// escape support: unescape string
inline  LibString::_ThisType &LibString::unescape(const _Elem &escapeChar)
{
    if (_raw.empty())
        return *this;

    std::string &thisRaw = _raw;
    const Int64 len = static_cast<Int64>(thisRaw.size());
    for (Int64 i = len - 1; i >= 0; --i)
    {
        const _Elem &ch = thisRaw[i];
        if (ch == escapeChar)
        {
            if (i > 0 && thisRaw[i - 1] == escapeChar)
                thisRaw.erase(i--, 1);
            else
                thisRaw.erase(i, 1);
        }
    }

    return *this;
}

inline  LibString::_ThisType LibString::unescape(const _Elem &escapeChar) const
{
    return _ThisType(*this).unescape(escapeChar);   
}

inline  void LibString::add_utf8_bomb()
{
    if (!this->has_utf8_bomb())
        _raw.insert(0, reinterpret_cast<const _Elem *>("\xef\xbb\xbf"));
}

inline  std::string::size_type LibString::length_with_utf8() const
{
    size_type count = 0;
    size_type bytePos = 0;
    while ((bytePos = _ThisType::_next_utf8_char_pos(bytePos)) != std::string::npos)
        ++count;

    return count;
}

inline  LibString::_ThisType LibString::substr_with_utf8(std::string::size_type pos, std::string::size_type n) const
{
    size_type utf8Len = this->length_with_utf8();
    if (pos >= utf8Len || n == 0)
        return _ThisType();

    _These substrs;
    this->split_utf8_string(pos, substrs);
    if (substrs.empty())
        return _ThisType();

    _ThisType str1 = *substrs.rbegin();
    utf8Len = str1.length_with_utf8();
    pos = (n == std::string::npos || n > utf8Len) ? utf8Len : n;

    substrs.clear();
    str1.split_utf8_string(pos, substrs);
    if (substrs.empty())
        return _ThisType();

    return substrs[0];
}

inline  void LibString::split_utf8_string(size_type charIndex, _These &strs) const
{
    strs.clear();
    if (charIndex == 0)
    {
        strs.push_back(*this);
        return;
    }

    size_type utf8Count = _ThisType::length_with_utf8();
    if (UNLIKELY(utf8Count == std::string::npos))
    {
        strs.push_back(*this);
        return;
    }

    charIndex = (charIndex < 0) ? 
        static_cast<size_type>(utf8Count) + charIndex : charIndex;
    if (charIndex <= 0 || charIndex >= static_cast<size_type>(utf8Count))
    {
        strs.push_back(*this);
        return;
    }

    size_type bytePos = 0;
    size_type charPos = 0;
    while (charPos != charIndex)
    {
        bytePos = _ThisType::_next_utf8_char_pos(bytePos);
        ++charPos;
    }

    strs.push_back(_raw.substr(0, bytePos));
    strs.push_back(_raw.substr(bytePos));
}

inline  void LibString::scatter_utf8_string(_These &chars, std::string::size_type scatterCount) const
{
    chars.clear();

    if (scatterCount == 0)
        scatterCount = std::string::npos;
    else if (scatterCount != std::string::npos)
        scatterCount -= 1;

    if (scatterCount == 0)
    {
        chars.push_back(*this);
        return;
    }

    size_type curPos = 0;
    size_type prevPos = 0;
    size_type curScatterCount = 0;
    while ((curPos = this->_next_utf8_char_pos(prevPos)) != std::string::npos)
    {
        chars.push_back(_raw.substr(prevPos, curPos - prevPos));

        if (scatterCount != std::string::npos && ++curScatterCount >= scatterCount)
        {
            chars.push_back(_raw.substr(curPos));
            break;
        }

        prevPos = curPos;
    }
}

inline  bool LibString::has_utf8_bomb() const
{
    if (_raw.size() < 3)
        return false;
    
    return (::memcmp(reinterpret_cast<const Byte8 *>(_raw.data()), 
        reinterpret_cast<const char *>("\xef\xbb\xbf"), 3) == 0) ? true : false;
}

inline  void LibString::remove_utf8_bomb()
{
    if (this->has_utf8_bomb())
        _raw.erase(0, 3);
}

inline  std::string::size_type LibString::_next_utf8_char_pos(std::string::size_type &beginBytePos) const
{
    if (beginBytePos == 0 && this->has_utf8_bomb())
        beginBytePos += 3;

    if (beginBytePos == std::string::npos || beginBytePos >= _raw.size())
        return std::string::npos;

    size_type waitCheckCount = std::string::npos;

    // 0xxx xxxx
    // Encoding len: 1 byte.
    U8 ch = static_cast<U8>(_raw.at(beginBytePos));
    if ((ch & 0x80) == 0x00)
        waitCheckCount = 0;
    // 110x xxxx
    // Encoding len: 2 bytes.
    else if ((ch & 0xe0) == 0xc0)
        waitCheckCount = 1;
    // 1110 xxxx
    // Encoding len: 3 bytes.
    else if ((ch & 0xf0) == 0xe0)
        waitCheckCount = 2;
    // 1111 0xxx
    // Encoding len: 4 bytes.
    else if ((ch & 0xf8) == 0xf0)
        waitCheckCount = 3;
    // 1111 10xx
    // Encoding len: 5 bytes.
    else if ((ch & 0xfc) == 0xf8)
        waitCheckCount = 4;
    // 1111 110x
    // Encoding len: 6 bytes.
    else if ((ch & 0xfe) == 0xfc)
        waitCheckCount = 5;

    if (waitCheckCount == std::string::npos)
        return std::string::npos;

    size_type curPos = beginBytePos + 1;
    size_type endPos = curPos + waitCheckCount;
    if (endPos > _raw.size())
        return std::string::npos;

    for (; curPos != endPos; ++curPos)
    {
        ch = static_cast<U8>(_raw.at(curPos));
        if ((ch & 0xc0) != 0x80)
            return std::string::npos;
    }

    return endPos;
}

KCP_CPP_END


// 重载运算符实现 std::string += LibString
extern inline  std::string &operator +=(std::string &o, const KCP_CPP_NS::LibString &input)
{
    o.append(input.GetRaw());
    return o;
}

// 重载运算符实现 std::string + LibString
extern inline  std::string operator +(const std::string &o, const KCP_CPP_NS::LibString &input)
{
    return o + input.GetRaw();
}

// 重载运算符实现 std::string += LibString
extern inline  std::string &operator +=(std::string &o, KCP_CPP_NS::LibString &&input)
{
    o.append(input.GetRaw());
    return o;
}

// 重载运算符实现 std::string + LibString
extern inline  std::string operator +(const std::string &o, KCP_CPP_NS::LibString &&input)
{
    return o + input.GetRaw();
}

namespace std
{

/**
 * \brief The explicit specialization of std::hash<KCP_CPP_NS::LibString> impl.
 * 
 */
template <>
struct hash<KCP_CPP_NS::LibString>
{
    size_t operator()(const KCP_CPP_NS::LibString &str) const noexcept
    {
            #ifdef _WIN32
            if (!str.empty())
                return ::std::_Hash_array_representation(str.data(), str.size());
            else
                return ::std::_Hash_representation(nullptr);
            #else
            if (!str.empty())
                return ::std::_Hash_impl::hash(str.data(), str.size());
            else
                return ::std::_Hash_impl::hash(nullptr, 0);
            #endif
    }
};

}
#endif
