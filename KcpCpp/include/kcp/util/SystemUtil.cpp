
#include <pch.h>
#include <kcp/util/SystemUtil.h>
#include <kcp/LibString.h>
#include <kcp/util/DirectoryUtil.h>
#include <kcp/util/FileUtil.h>


#ifdef _WIN32
#pragma region defines
// 获取内存状态函数函数原型指针
// typedef   void(WINAPI *__GlobalMemoryStatusExFunc)(LPMEMORYSTATUSEX);

static inline Int32 GetMemoryStatus(MEMORYSTATUSEX &status)
{
    // 载入动态链接库kernel32.dll，返回它的句柄
//     HMODULE hModule;
//     hModule = LoadLibrary("kernel32.dll");
//     if((!hModule))
//         return StatusDefs::SystemUtil_GetKernel32HandleFailed;

    // 在kernel32.dll句柄里查找GlobalMemoryStatusEx函数，获取函数指针
//     __GlobalMemoryStatusExFunc globalMemoryStatusEx = (__GlobalMemoryStatusExFunc)GetProcAddress(hModule, "GlobalMemoryStatusEx");
//     if((!globalMemoryStatusEx))
//         return StatusDefs::SystemUtil_GetGlobalMemoryStatusExFuncFailed;

//      globalMemoryStatusEx(&status);
// 
//     // 释放链接库句柄
//     FreeLibrary(hModule);

    // 调用函数取得系统的内存情况
    status.dwLength = sizeof(status);
    if(!GlobalMemoryStatusEx(&status))
        return Status::SystemUtil_GetGlobalMemoryStatusExFailed;

    return Status::Success;
}
#pragma endregion

#endif

KCP_CPP_BEGIN

#ifdef _WIN32
#pragma region windowsdefines
typedef struct
{
    HWND    hwndWindow;     // 窗口句柄
    DWORD   dwProcessID;    // 进程ID
}EnumWindowsArg;
#pragma endregion

static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    auto *pArg = (EnumWindowsArg *)lParam;

    // 通过窗口句柄取得进程ID
    DWORD  dwProcessID = 0;
    ::GetWindowThreadProcessId(hwnd, &dwProcessID);
    if(dwProcessID == pArg->dwProcessID)
    {
        pArg->hwndWindow = hwnd;
        // 找到了返回FALSE
        return false;
    }

    // 没找到，继续找，返回TRUE
    return true;
}
#endif

KCP_CPP_END

KCP_CPP_BEGIN

Int32 SystemUtil::GetProgramPath(bool isCurrentProcess, LibString &processPath, UInt64 pid) 
{
#ifdef _WIN32
    HMODULE hModule = NULL;
    HANDLE hProc = NULL;

    do
    {
        if((!isCurrentProcess && !pid))
            return Status::ParamError;

        // 若是当前进程
        Byte8  pathName[MAX_PATH] = {0};
        if(isCurrentProcess)
        {
            if((!GetModuleFileName(NULL, pathName, MAX_PATH)))
                return Status::SystemUtil_GetModuleFileNameFailed;

            processPath.AppendData(pathName, MAX_PATH);
            break;
        }

        hProc = OpenProcess(PROCESS_QUERY_INFORMATION, false, static_cast<DWORD>(pid));
        if((!hProc))
            return Status::SystemUtil_OpenProcessQueryInfomationFailed;

        hModule = LoadLibrary(TEXT("Kernel32.dll"));
        if((!hModule))
            return Status::SystemUtil_LoadKernel32LibraryFailed;

        // 获取QueryFullProcessImageNameA函数
        if(GetProcAddress(hModule, "QueryFullProcessImageNameA"))
        {
            DWORD dwProcPathLen = MAX_PATH / sizeof(Byte8);
            if(!QueryFullProcessImageName(hProc, 0, pathName, &dwProcPathLen))
                return Status::SystemUtil_QueryFullProcessImageNameFailed;

            processPath.AppendData(pathName, MAX_PATH);
            break;
        }

        // 获取进程带驱动器名的路径（驱动器名：\\Device\\HardwareVolume1）
        if(!::GetProcessImageFileName(hProc, pathName, MAX_PATH))
            return Status::SystemUtil_GetProcessImageFileNameFailed;

        // 遍历确认驱动器名对应的盘符名
        Byte8   volNameDev[MAX_PATH] = {0};
        Byte8   volName[MAX_PATH] = {0};
        _tcscat_s(volName, MAX_PATH, TEXT("A:"));
        bool isFound = false;
        for(; *volName <= _T('Z'); (*volName)++)
        {
            // 获取盘符
            if(!QueryDosDevice(volName, volNameDev, MAX_PATH))
            {
                auto lastError = GetLastError();
                if(lastError == 2)
                    continue;

                return Status::SystemUtil_QueryDosDeviceError;
            }

            // 确认是否驱动器名一样
            if(_tcsncmp(pathName, volNameDev, _tcslen(volNameDev)) == 0)
            {
                isFound = true;
                break;
            }
        }

        if(!isFound)
            return Status::SystemUtil_GetDriveError;

        processPath.AppendData(volName, _tcslen(volName));
        processPath.AppendData(pathName + _tcslen(volNameDev), _tcslen(pathName) - _tcslen(volNameDev));
    } while(0);

    if(hModule)
        FreeLibrary(hModule);

    if(hProc)
        CloseHandle(hProc);

    return Status::Success;
#else

    ssize_t ret = -1;
    char buf[PATH_MAX + 1];
    if(isCurrentProcess)
    {
        if((ret = readlink("/proc/self/exe", buf, PATH_MAX)) == -1)
            return Status::SystemUtil_GetProcNameFail;
    }
    else
    {
        BUFFER64 proc = {};
        sprintf(proc, "/proc/%llu/exe", pid);
        if((ret = readlink(proc, buf, PATH_MAX)) == -1)
            return Status::SystemUtil_GetProcNameFail;
    }

    buf[ret] = '\0';
    processPath = buf;
    return Status::Success;
#endif
}

LibString SystemUtil::GetCurProgramName()
{
    LibString path;
    SystemUtil::GetProgramPath(true, path);
    return DirectoryUtil::GetFileNameInPath(path);
}

LibString SystemUtil::GetCurProgramNameWithoutExt()
{
    return FileUtil::ExtractFileWithoutExtension(SystemUtil::GetCurProgramName().RemoveZeroTail());
}

LibString SystemUtil::GetCurProgRootPath()
{
    LibString path;
    SystemUtil::GetProgramPath(true, path);
    return DirectoryUtil::GetFileDirInPath(path);
}

Int32 SystemUtil::CloseProcess(Int32 processId, ULong *lastError)
{
#ifdef _WIN32
    if(!TerminateProcess(OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION, false, processId), 0))
    {
        if(lastError)
            *lastError = GetLastError();

        return Status::Failed;
    }
#else
    auto ret = kill(processId, SIGKILL);
    if(ret != 0)
    {
        perror("kill process fail");
        return Status::Failed;
    }
#endif

    return Status::Success;
}

// 弹窗
void SystemUtil::MessageBoxPopup(const LibString &title, const LibString &content)
{
#ifdef _WIN32
    auto hwnd = GetWindowHwndByPID(GetCurProcessId());
    ::MessageBoxA(hwnd, content.c_str(), title.c_str(), MB_ABORTRETRYIGNORE);
#endif
}

Int32 SystemUtil::SetConsoleColor(Int32 color)
{
#ifdef _WIN32
    HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    if(::SetConsoleTextAttribute(handle, color) == 0)
    {
        ASSERT(!"SetConsoleTextAttribute failed");
        return Status::Failed;
    }
#endif

    return Status::Success;
}

Int32 SystemUtil::GetConsoleColor()
{
#ifdef _WIN32
    HANDLE handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info;
    if(::GetConsoleScreenBufferInfo(handle, &info) == 0)
    {
        ASSERT(!"GetConsoleScreenBufferInfo failed");
        return Status::Error;
    }

    return info.wAttributes;
#endif

    return 0;
}

#ifdef _WIN32
    // 通过进程ID获取窗口句柄
HWND SystemUtil::GetWindowHwndByPID(DWORD dwProcessID)
{
    HWND hwndRet = NULL;
    EnumWindowsArg ewa;
    ewa.dwProcessID = dwProcessID;
    ewa.hwndWindow = NULL;
    EnumWindows(KCP_CPP_NS::EnumWindowsProc, (LPARAM)&ewa);
    if(ewa.hwndWindow)
    {
        hwndRet = ewa.hwndWindow;
    }
    return hwndRet;
}
#endif

KCP_CPP_END


