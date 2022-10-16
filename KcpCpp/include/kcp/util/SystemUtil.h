
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_SYSTEM_UTIL_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_UTIL_SYSTEM_UTIL_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>
#include <kcp/util/SystemUtilDef.h>
#include <kcp/SpinLock.h>

KCP_CPP_BEGIN

class LibString;
struct ProcessMemInfo;

class SystemUtil
{
public:
    // 睡眠挂起
    static void ThreadSleep(UInt64 milliSec, UInt64 microSec = 0);

    // 获取线程id
    static UInt64 GetCurrentThreadId();

    /* 进程线程 */
    // 获取程序目录
    static Int32 GetProgramPath(bool isCurrentProcess, LibString &processPath, UInt64 pid = 0);
    // 获取当前进程名
    static LibString GetCurProgramName();   // 带扩展名
    static LibString GetCurProgramNameWithoutExt();  // 不扩展名
    // 获取当前进程根目录
    static LibString GetCurProgRootPath();
    // 获取进程id
    static Int32 GetCurProcessId();
    // 结束进程
    static Int32 CloseProcess(Int32 processId, ULong *lastError = NULL);
        // 弹窗
    static void MessageBoxPopup(const LibString &title, const LibString &content);
    
    /* 控制台 */
    static void LockConsole();
    static void UnlockConsole();
    // 设置控制台颜色
    static Int32 SetConsoleColor(Int32 color);
    // 获取控制台颜色
    static Int32 GetConsoleColor();
    // 输出给控制台
    static void OutputToConsole(const LibString &outStr);

#ifdef _WIN32
    // 通过进程ID获取窗口句柄
    static HWND GetWindowHwndByPID(DWORD dwProcessID);
#endif
private:
};

inline void SystemUtil::ThreadSleep(UInt64 milliSec, UInt64 microSec)
{
    std::chrono::microseconds t(milliSec * TimeDefs::MICRO_SECOND_PER_MILLI_SECOND + microSec);
    std::this_thread::sleep_for(t);
}

inline UInt64 SystemUtil::GetCurrentThreadId()
{
	return ::GetCurrentThreadId();
}

inline Int32 SystemUtil::GetCurProcessId()
{
    return ::_getpid();
}

inline void SystemUtil::LockConsole()
{
    GetConsoleLocker().Lock();
}

inline void SystemUtil::UnlockConsole()
{
    GetConsoleLocker().Unlock();
}

inline void SystemUtil::OutputToConsole(const LibString &outStr)
{
    printf("%s", outStr.c_str());
}

KCP_CPP_END


#endif
