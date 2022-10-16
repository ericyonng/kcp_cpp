

#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_SYSTEM_UTIL_DEF_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_SYSTEM_UTIL_DEF_H__

#pragma once

#include <kcp/common/common.h>

KCP_CPP_BEGIN

class LibConsoleColor
{
public:
   enum
    {
        Fg_Black = 0,
        Fg_Red = FOREGROUND_RED,
        Fg_Green = FOREGROUND_GREEN,
        Fg_Blue = FOREGROUND_BLUE,
        Fg_Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
        Fg_Purple = FOREGROUND_RED | FOREGROUND_BLUE,
        Fg_Cyan = FOREGROUND_BLUE | FOREGROUND_GREEN,
        Fg_White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
        Fg_Gray = FOREGROUND_INTENSITY,
        Fg_LightYellow = Fg_Gray | Fg_Yellow,
        
        Bg_Black  = 0,
        Bg_Red    = BACKGROUND_RED,
        Bg_Green  = BACKGROUND_GREEN,
        Bg_Blue   = BACKGROUND_BLUE,
        Bg_Yellow = BACKGROUND_RED  | BACKGROUND_GREEN,
        Bg_Purple = BACKGROUND_RED  | BACKGROUND_BLUE,
        Bg_Cyan   = BACKGROUND_BLUE | BACKGROUND_GREEN,
        Bg_White  = BACKGROUND_RED  | BACKGROUND_GREEN | BACKGROUND_BLUE,

        Highlight_Fg = FOREGROUND_INTENSITY,
        Highlight_Bg = BACKGROUND_INTENSITY,

        Fg_Default = Fg_White,
        Bg_Default = Bg_Black
    };
};

// 进程占用的内存信息
struct ProcessMemInfo
{
    size_t _maxHistorySetSize;              // 使用过的最大工作集，峰值内存占用
    size_t _curSetSize;                     // 当前工作集占用大小，当前进程占用的内存
    size_t _maxHistoryPagedPoolUsage;       // 使用过的最大分页池大小
    size_t _pagedPoolUsage;                 // 分页池大小
    size_t _maxHistoryNonPagedPoolUsage;    // 使用过的最大非分页池大小
    size_t _curNonPagedPoolUsage;           // 当前非分页池大小
    size_t _curPageFileUsage;               // 当前页交换文件使用大小
    size_t _maxHistoryPageFileUsage;        // 历史最大页交换文件使用大小
    size_t _processAllocMemoryUsage;        // 进程运行过程中申请的内存大小
};

KCP_CPP_END

#endif
