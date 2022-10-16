

#include <pch.h>
#include <kcp/util/RttiUtil.h>

// 从rtti中剔除str
#define __LIB_GET_TYPE_NAME_Trim(str, len)       \
    it = rtti;                                   \
    skipCopy = 0;                                \
    itEnd = rtti + rawTyNameLen - totalSkipCopy; \
    while ((it = strstr(it, str)))               \
    {                                            \
        size_t copyLen = itEnd - it - len;       \
        memmove(it, it + len, copyLen);          \
        itEnd -= len;                            \
        skipCopy += len;                         \
        if (copyLen == 0)                        \
            break;                               \
    }                                            \
    totalSkipCopy += skipCopy;                   \
    rtti[rawTyNameLen - totalSkipCopy] = '\0'    \


#ifdef _WIN32

#define PTR_SUFFIX " __ptr64"

#elif defined(_WIN64)

#define PTR_SUFFIX " __ptr64"

#endif

KCP_CPP_BEGIN

const Byte8 *RttiUtil::GetByTypeName(const char *rawTypeName)
{
#ifdef _WIN32
    static thread_local Byte8 *rtti = new Byte8[512];
    ::memset(rtti, 0, 512);

    size_t rawTyNameLen = strlen(rawTypeName);
    ::memcpy(rtti, rawTypeName, rawTyNameLen);
    rtti[rawTyNameLen] = '\0';

    size_t totalSkipCopy = 0;

    Byte8 *it;
    Byte8 *itEnd;
    size_t skipCopy;
    __LIB_GET_TYPE_NAME_Trim("class ", 6);
    __LIB_GET_TYPE_NAME_Trim("struct ", 7);
    __LIB_GET_TYPE_NAME_Trim(PTR_SUFFIX, 8);
    //__LIB_GET_TYPE_NAME_Trim(" *", 2);

    Byte8 *anonBeg = rtti;
    while((anonBeg = strchr(anonBeg, '`')))
    {
        *anonBeg = '(';
        Byte8 *anonEnd = strchr(anonBeg + 1, '\'');
        *anonEnd = ')';
    }

    return rtti;
#else // Non-Win32
    return GetCxxDemangle(rawTypeName);
#endif
}

#undef __LIB_GET_TYPE_NAME_Trim


KCP_CPP_END

