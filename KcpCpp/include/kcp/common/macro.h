#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_MACRO_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_MACRO_H__

#pragma once

typedef char Byte8;
typedef unsigned char U8;
typedef short Int16;
typedef unsigned short UInt16;
typedef int Int32;
typedef unsigned int UInt32;
typedef long Long;
typedef unsigned long ULong;
typedef long long Int64;
typedef unsigned long long UInt64;
typedef float Float;
typedef double Double;

// cache
#define BUFFER_LEN2         2
#define BUFFER_LEN4         4
#define BUFFER_LEN8         8
#define BUFFER_LEN16        16
#define BUFFER_LEN32        32
#define BUFFER_LEN64        64
#define BUFFER_LEN128       128
#define BUFFER_LEN256       256
#define BUFFER_LEN512       512
#define BUFFER_LEN1024      1024
typedef Byte8 BUFFER2[BUFFER_LEN2];
typedef Byte8 BUFFER4[BUFFER_LEN4];
typedef Byte8 BUFFER8[BUFFER_LEN8];
typedef Byte8 BUFFER16[BUFFER_LEN16];
typedef Byte8 BUFFER32[BUFFER_LEN32];
typedef Byte8 BUFFER64[BUFFER_LEN64];
typedef Byte8 BUFFER128[BUFFER_LEN128];
typedef Byte8 BUFFER256[BUFFER_LEN256];
typedef Byte8 BUFFER512[BUFFER_LEN512];
typedef Byte8 BUFFER1024[BUFFER_LEN1024];

#undef CRYSTAL_TRACE
#define CRYSTAL_TRACE(...)

#undef ASSERT
#define ASSERT(...)

#undef NULL
#define NULL nullptr

// 自旋锁，自选轮询次数
#undef SPINNING_COUNT
#define SPINNING_COUNT 8000

// 快照帧数
#undef SYMBOL_MAX_CAPTURE_FRAMES
#define SYMBOL_MAX_CAPTURE_FRAMES 100

// 符号最大长度
#undef SYMBOL_MAX_SYMBOL_NAME
#define SYMBOL_MAX_SYMBOL_NAME 63

// 命名空间
#undef KCP_CPP_BEGIN
#define KCP_CPP_BEGIN namespace KCP_CPP {

#undef KCP_CPP_END
#define KCP_CPP_END }


// CRYSTAL_NET命名空间
#undef KCP_CPP_NS
#define KCP_CPP_NS ::KCP_CPP

#ifndef INFINITE
 #define INFINITE        0xFFFFFFFF
#endif


#undef MAX_NAME_LEN
#define MAX_NAME_LEN 32

#undef MAX_PWD_LEN
#define MAX_PWD_LEN 32

#undef MAX_CEIL_WIDE
#define MAX_CEIL_WIDE 16

#undef DOUBLE_FMT_STR
#define DOUBLE_FMT_STR "%.16lf"

#undef FLOAT_FMT_STR
#define FLOAT_FMT_STR "%.8lf"

// 内存对齐
#undef MEM_ALIGN_BEGIN
#define MEM_ALIGN_BEGIN(n)  pack(push, n)
#undef MEM_ALIGN_END
#define MEM_ALIGN_END       pack(pop)

#undef __MEMORY_ALIGN_BYTES__
#define __MEMORY_ALIGN_BYTES__      (sizeof(void *)<<1)     // 默认16字节对齐 涉及跨cache line 开销

// 面向__MEMORY_ALIGN_BYTES__字节内存对齐
#undef __MEMORY_ALIGN__
#define __MEMORY_ALIGN__(bytes)                                                     \
((bytes) / __MEMORY_ALIGN_BYTES__ * __MEMORY_ALIGN_BYTES__ + ((bytes)%__MEMORY_ALIGN_BYTES__?__MEMORY_ALIGN_BYTES__:0))

#undef ARRAY_ELEM_COUNT
#define ARRAY_ELEM_COUNT(x) sizeof(x)/sizeof(x[0])

#ifdef _WIN32
// 是否因为信号被唤醒 windows event组件配合wait
#undef IS_EVENT_SINAL_WAKE_UP
#define IS_EVENT_SINAL_WAKE_UP(waitRet)                                      \
(static_cast<Int64>(WAIT_OBJECT_0) <= (waitRet)) &&                                  \
((waitRet) <= static_cast<Int64>(MAXIMUM_WAIT_OBJECTS + WAIT_OBJECT_0))
#endif

// 默认需要剔除的符号
#undef DEF_STRIP_CHARS
#define DEF_STRIP_CHARS   " \t\v\r\n\f"       

// 不同平台下文本行结束字符
#undef LINE_END_CHARS
#ifdef _WIN32
    #define LINE_END_CHARS "\r\n"
#else
    #define LINE_END_CHARS "\n"
#endif

#undef FMT_FLAGS
#define FMT_FLAGS    "-+ #0"

// 浮点数精度
#define FLOAT_NUM_PRECISION     DBL_EPSILON
// 浮点数0值比较
#define COMP_FLOAT_WITH_ZERO(floatNum)                  \
( ((floatNum) < FLOAT_NUM_PRECISION) &&  ((floatNum) > -FLOAT_NUM_PRECISION) )
// 浮点数相等
#define IS_DOUBLE_EQUAL(a, b)                           \
COMP_FLOAT_WITH_ZERO(a-b)

// 浮点数比较大小
#define IS_DOUBLE_BIGGER(a, b)                          \
!IS_DOUBLE_EQUAL(a, b) && ((a-b)>0)

/*
 * Number of milli-seconds/micro-seconds/100-nano seconds between the beginning of the
 * Windows epoch (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970).
 *
 * This assumes all Win32 compilers have 64-bit support.
 */
#ifdef _WIN32
 #if defined(_MSC_VER) || defined(_MSC_EXTENSIONS) || defined(__WATCOMC__)
    #define CRYSTAL_EPOCH_IN_MSEC    11644473600000LLU
    #define CRYSTAL_EPOCH_IN_USEC    11644473600000000LLU
    #define CRYSTAL_EPOCH_IN_100NSEC 116444736000000000LLU
 #else
    #define CRYSTAL_EPOCH_IN_MSEC    11644473600000LLU
    #define CRYSTAL_EPOCH_IN_USEC    11644473600000000LLU
    #define CRYSTAL_EPOCH_IN_100NSEC 116444736000000000LLU
 #endif
#endif

#ifndef LIB_LOG_FMT
 #define LIB_LOG_FMT(...) (##__VA_ARGS__)
#endif

// ini 统一放在程序根目录下的ini目录
#define ROOT_DIR_INI_SUB_DIR    "ini/"
#define ROOT_DIR_LOG_SUB_DIR    "Log/"

// socket
#ifndef _WIN32
    #ifdef __APPLE__
        #define _DARWIN_UNLIMITED_SELECT
    #endif // !__APPLE__

    #define SOCKET Int32
    #define INVALID_SOCKET          -1
    #define SOCKET_ERROR            (-1)
#endif




#ifdef _WIN32
    #ifndef LIKELY
        #define LIKELY(x) (x)
    #endif
    #ifndef UNLIKELY
        #define UNLIKELY(x) (x)
    #endif
#else
    #ifndef LIKELY
        #define LIKELY(x) __builtin_expect(!!(x), 1)
    #endif
    #ifndef UNLIKELY
        #define UNLIKELY(x) __builtin_expect(!!(x), 0)
    #endif
#endif

// 文件光标位置
KCP_CPP_BEGIN

class FileCursorOffsetType
{
public:
    enum FILE_CURSOR_POS
    {
        FILE_CURSOR_POS_SET = 0,    // 起始位置
        FILE_CURSOR_POS_CUR = 1,    // 当前位置
        FILE_CURSOR_POS_END = 2,    // 文件末尾
    };
};

KCP_CPP_END

#endif
