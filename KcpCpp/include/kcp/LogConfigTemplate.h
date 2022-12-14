/*!
 *  MIT License
 *  
 *  Copyright (c) 2020 ericyonng<120453674@qq.com>
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to deal
 *  in the Software without restriction, including without limitation the rights
 *  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *  copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *  SOFTWARE.
 * 
 * Author: Eric Yonng
 * Date: 2021-02-19 14:11:58
 * Description: 配置模版,没有配置会自动生成配置
*/

#ifndef __CRYSTAL_NET_KERNEL_SOURCE_COMP_LOG_LOG_CONFIG_TEMPLATE_H__
#define __CRYSTAL_NET_KERNEL_SOURCE_COMP_LOG_LOG_CONFIG_TEMPLATE_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

class LogConfigTemplate
{
public:
    static LibString GetLogConfigIniContent();
};

inline LibString LogConfigTemplate::GetLogConfigIniContent()
{
    LibString content;

    content = \
"; Author: EricYonng(120453674@qq.com)\n"
"; Date:2021.02.17\n"
"; Desc:日志配置."
"\n"
"[Common]\n"
"; 格式:fileId,文件名,线程相关性id(同id的日志在同一个线程着盘)"
"\nFileName=0,,1|1,Crash,1|2,Net,2|3,Custom,1|4,Sys,1|5,MemMonitor,3|6,Trace,1\n"
"; 文件后缀名."
"\nExtName=.log\n"
"; 日志最大缓存限制,超过则即时着盘."
"\nMaxLogCacheMB=16\n"
"; 日志着盘时间间隔,定时着盘."
"\nLogTimerIntervalMs=1000\n"
"; 日志单文件最大大小,超过会创建分立文件."
"\nMaxFileSizeMB=256\n"
"; 是否产出日志."
"\nIsEnableLog=1\n"
"; 日志路径:[是否使用程序名当日志的一级目录][路径] ."
"; 若使用程序名当一级目录则可以缺省指定路径名 若此时有指定路径名路径名会追加在该指定路径成为二级目录 ."
"; 若指定了绝对路径则必定使用给定的绝对路径 若此时又指定了程序名当目录 追加在该指定路径成为二级目录."
"\nLogPath=1,./\n"
"\n"
"[LogLevel]\n"
"; 日志等级配置,关联日志文件id\n"
"; 系统默认等级id不可更改: Debug,Info,Warn,Error,Crash,Net,Custom,Sys,Trace\n"
"; [日志等级] = [enable],[fileid],[enableConsole],[fgcolor]|[bgcolor],[LevelId],[EnableRealTime],[PRINT_STACK_TRACE_BACK]\n"
"\nDebug        = 1,0,1,White|Black,1,0,0\n"
"Info           = 1,0,1,White|Black,2,0,0\n"
"Warn           = 1,0,1,LightYellow|Black,3,0,0\n"
"Error          = 1,0,1,Red|Black,4,0,1\n"
"Crash          = 1,1,1,Red|Black,5,0,0\n"
"Net            = 1,2,0,White|Black,6,0,0\n"
"Custom         = 1,3,1,White|Black,7,0,0\n"
"Sys            = 1,4,1,White|Black,8,0,0\n"
"MemMonitor     = 1,5,0,White|Black,9,0,0\n"
"Trace          = 1,6,1,Green|Black,10,0,0\n";

    return content;
}

KCP_CPP_END

#endif
