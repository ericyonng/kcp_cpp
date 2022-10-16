
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_CONFIG_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_CONFIG_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>

KCP_CPP_BEGIN

struct LogLevelCfg
{
    Int32 _level = 0;                   // 日志等级
    LibString _levelName;               // 日志等级名
    bool _enable = false;               // 是否允许写日志
    Int32 _fileId = 0;                  // 绑定日志文件id
    bool _enableConsole = false;            // 是否允许控制台显示
    Int32 _fgColor = 0;                 // 前景色 默认白色
    Int32 _bgColor = 0;                 // 背景色 默认黑色
    bool _enableRealTime = false;       // 是否开启实时着盘
    bool _printStackTraceBack = false;  // 开启打印堆栈信息
};

struct LogConfig
{
    Int32 _logFileId = 0;                               // 与文件名捆绑 0表示程序进程名做文件名
    std::vector<LogLevelCfg *>    _levelIdxRefCfg;      // level当下表索引  需要额外判空,有可能为空
    LibString _logFileName;                             // 文件名
    LibString _extName;                                 // 文件后缀名
    Int32 _intervalMs = 0;                              // 日志着盘间隔
    Int64 _maxFileSize = 0;                             // 文件最大大小 
    Int32 _threadRelationId;                            // 线程相关性id
    bool _isEnable = false;                             // 只要有一个级别的日志开启则为true
};

KCP_CPP_END

// 文件名配置=0,|1,文件名|2,文件名|3,文件名
// 最大缓存限制=maxMB ;用于限制日志缓存大小,避免内存暴涨
// 日志着盘时间间隔:
// 日志文件最大大小
// ini 配置：[日志等级] = [enable],[fileid],[enableConsole],[fgcolor]:white,red,black,[bgcolor]:...
#endif

