
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_COUNT_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_COUNT_UTIL_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

// 只统计无符号的1的个数
class CountUtil
{
public:
    template <typename NumType>
    static UInt32 Count1InBinary(NumType num);

    // 可以将任意整数拆成n个字节并分别算出1的个数再相加
    static void Generate8BitStatistics1Table(UInt32 array[256]);

    // static bool Comp(UInt32 array[256]);

private:
    static UInt32 *_GetCountTable();    
};

template <typename NumType>
inline UInt32 CountUtil::Count1InBinary(NumType num)
{
    const UInt64 sizeNum = sizeof(NumType);
    auto countArr = _GetCountTable();
    UInt32 countNum = 0;
    while (num)
    {
        countNum += countArr[num & 0xff];
        num >>= 8;
    }
    
    return countNum;
}

inline void CountUtil::Generate8BitStatistics1Table(UInt32 array[256])
{
    ::memset(array, 0, 256 * sizeof(UInt32));

    // 偶数1的个数与n/2的个数是一样的,是由n/2左移得到的,个数不变
    // 奇数1的个数是n/2的个数左移后+1而来的
    for(Int32 i = 0; i < 256; ++i )
        array[i] = (i & 1) + array[i/2];
}

// inline bool CountUtil::Comp(UInt32 array[256])
// {
//     auto arr2 = _GetCountTable();
//     for (Int32 i = 0; i < 256; ++i)
//     {
//         if (array[i] != arr2[i])
//             return false;
//     }

//     return true;
// }

KCP_CPP_END

#endif
