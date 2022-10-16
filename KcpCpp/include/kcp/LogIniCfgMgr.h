
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_INI_CFG_MGR_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LOG_INI_CFG_MGR_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LogDefs.h>

KCP_CPP_BEGIN

class LibIniFile;
struct LogLevelCfg;
struct LogConfig;

class LogIniCfgMgr
{
public:
    LogIniCfgMgr();
    ~LogIniCfgMgr();

    // 默认在当前程序下的ini目录
    virtual bool Init(const Byte8 *iniFileName);
    void Close();

public:
    const LogLevelCfg *GetLevelCfg(Int32 logLevel) const;
    const LogConfig *GetLogCfg(Int32 logLevel) const;
    Int64 GetMaxLogCacheBytes() const;
    const std::vector<LogConfig *> &GetLogFileCfgs() const;
    const Int32 GetMaxLogFileId() const;
    const bool IsEnableLog() const;
    const LibString &GetLogPath() const;
    const std::map<Int32, std::vector<LogConfig *>> &GetThreadRelationLogCfgs() const;
    const Int32 GetIntervalMs() const;


public:
    LibIniFile *_iniFile;

    std::vector<LogLevelInfo *> _levelInfos;    // levelid做下标索引
    std::vector<LogLevelCfg *> _levelIdxRefCfg; // level做下表索引
    std::vector<LogConfig *> _fileIdIdxRefCfg;  // 文件id做下标索引
    std::map<Int32, std::vector<LogConfig *>> _threadRelationIdRefLogCfgs;      // 日志与线程相关性dict
    Int64 _maxLogCacheBytes;                    // 最大日志缓存大小
    Int32 _maxFileId;                           // 最大文件id
    bool _isEnableLog;                          // 是否产出日志
    LibString _logPath;                         // 日志路径
    Int32 _intervalMs;                          // 着盘间隔ms
};

inline const LogLevelCfg *LogIniCfgMgr::GetLevelCfg(Int32 logLevel) const
{
    return _levelIdxRefCfg[logLevel];
}

inline const LogConfig *LogIniCfgMgr::GetLogCfg(Int32 logLevel) const
{
    auto levelCfg = GetLevelCfg(logLevel);
    return _fileIdIdxRefCfg[levelCfg->_fileId];
}

inline Int64 LogIniCfgMgr::GetMaxLogCacheBytes() const
{
    return _maxLogCacheBytes;
}

inline const std::vector<LogConfig *> &LogIniCfgMgr::GetLogFileCfgs() const
{
    return _fileIdIdxRefCfg;
}

inline const Int32 LogIniCfgMgr::GetMaxLogFileId() const
{
    return _maxFileId;
}

inline const bool LogIniCfgMgr::IsEnableLog() const
{
    return _isEnableLog;
}

inline const LibString &LogIniCfgMgr::GetLogPath() const
{
    return _logPath;
}

inline const std::map<Int32, std::vector<LogConfig *>> &LogIniCfgMgr::GetThreadRelationLogCfgs() const
{
    return _threadRelationIdRefLogCfgs;
}

inline const Int32 LogIniCfgMgr::GetIntervalMs() const
{
    return _intervalMs;
}

KCP_CPP_END

#endif
