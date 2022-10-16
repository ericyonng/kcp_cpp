#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_LIBS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_COMMON_LIBS_H__

#pragma once

   #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <atomic>
    #include <limits>   
    #include <numeric>  // 随机数等
    #include <thread>
    #include <algorithm>
    #include <stdexcept>
    #include <deque>
    #include <set>
    #include <vector>
    #include <list>
    #include <map>
    #include <unordered_map>        // 与map的不同在于内建的是hash表而不是红黑树
    #include <unordered_set>
    #include <mutex>
    #include <assert.h> 
    #include <iostream>
    #include <string>
    #include <stdarg.h>     // c风格格式化vs_arg等接口
    #include <chrono>       // 时间
    #include <forward_list> // std::forward等
    #include <fstream>
    #include <time.h>
    #include <stddef.h>
    #include <regex>
    #include <random>
    #include <stack>
    #include <queue>
    #include <tuple>
    #include <type_traits>  // is_class, is_pod等
    #include <bitset>
    #include <array>
    #include <iosfwd>
    #include <sstream>  // stringstream
    #include <yvals_core.h> // 使用_NODISCARD
    #include <memory>
    // #include <any>


#ifdef _WIN32
        #include <crtdbg.h>
        #include <WinSock2.h>
        #include <ws2def.h>
        #include <ws2tcpip.h>
        #include <fcntl.h>
        #include <sys/types.h>
        #include <sys/stat.h>
        #pragma comment(lib,"ws2_32.lib")
        #include<MSWSock.h>
        #pragma comment (lib, "MSWSock.lib")

        #include "shlwapi.h"        // 控制台程序接口
        #pragma comment(lib,"shlwapi.lib")

        // 线程等
        #include <process.h>
        #include <tchar.h>

        // 系统信息
        #include "Psapi.h"
        #include "tlhelp32.h"
        #include "sysinfoapi.h"
        #include <iphlpapi.h>   // 网卡
        #pragma comment(lib,"Iphlpapi.lib")
        
        #include<direct.h>      // mkdir func
        #include<io.h>          // access func

        #include <ws2ipdef.h>   // ipv6等
        #include <profileapi.h> // cpucounter
        #include <intrin.h> // 获取cpuid

    #else
        #include <linux/version.h>  // 内核版本宏等 LINUX_VERSION_CODE/KERNEL_VERSION等
        #include <unistd.h>
        #include <limits.h> // 含有PATH_MAX
        #include <errno.h>
        
        // linux socket环境 以及相关网络接口
        #include <sys/types.h>
        #include <sys/socket.h> // 含有 getaddrinfo等
        #include <netdb.h>
        #include <signal.h>
        #include<sys/param.h>
        #include <sys/resource.h>
        #include <netinet/tcp.h>

        #include <semaphore.h>  // 信号量
        #include <pthread.h>    // 线程库
        #include <sys/time.h>
        #include <sys/stat.h>
        #include <fcntl.h>
        #include <sys/eventfd.h>    // 多线程多进程事件通知机制

        // ip相关
        #include <arpa/inet.h>

        // 生成uuid
        #include <uuid/uuid.h> // 真正在uuid/uuid.h

        // syscall 系统调用获取硬件时钟,单调递增,不随用户调整而变化,不受ntp影响 系统调用会有上下文切换开销
        #include <sys/syscall.h>
        // 包含sysinfo结构体信息
        #include <linux/kernel.h>
        #include <sys/sysinfo.h>

        // linux下随机数相关
        #include <tr1/random>

        // linux下类型识别接口相关
        #include <cxxabi.h>
        // linux下堆栈追踪头文件
        #include <execinfo.h>

        // event
        #include <sys/eventfd.h>

        // 支持获取网卡信息
        #include	<net/if.h>

        #include <sys/sysctl.h> // sysctl函数
        #include <net/route.h>  // 路由表 ifa_msghdr等
        #include <net/if_dl.h> // 数据结构 sockaddr_dl

        #include <cpuid.h>  // 获取cpuid
        #include <x86intrin.h>  // rdtsc/rdtscp支持
    #endif
#endif
