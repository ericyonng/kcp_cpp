
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_LOG_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_LIB_LOG_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/ILog.h>
#include <kcp/ConditionLocker.h>

KCP_CPP_BEGIN

class LibThread;
class SpecifyLog;
class LogIniCfgMgr;

class LibLog : public ILog
{
public:
    LibLog();
    virtual ~LibLog();

public:
    virtual bool Init(const Byte8 *logConfigFile = "LogCfg.ini", const Byte8 *logCfgDir = NULL);
    virtual void Start();
    virtual void Close();
    virtual void FlushAll();
    virtual const LibString &GetLogRootPath() const;
    virtual bool IsStart() const;
    virtual bool IsLogOpen() const;

    virtual void UnInstallAfterLogHookFunc(Int32 level, const IDelegate<void> *delegate);
    virtual void UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *delegate);

protected:
    virtual void _InstallAfterLogHookFunc(const LogLevelCfg *levelCfg, IDelegate<void> *delegate);
    virtual void _InstallBeforeLogHookFunc(const LogLevelCfg *levelCfg, IDelegate<void, LogData *> *delegate);

protected:
    virtual const LogLevelCfg *_GetLevelCfg(Int32 level) const;
    virtual void _WriteLog(const LogLevelCfg *levelCfg, LogData *logData);
    void _OnLogThreadFlush(LibThread *t, UInt64 param);
    void _OnLogFlush(std::vector<SpecifyLog *> &logs, Int32 logCount);

private:
    std::atomic_bool _isInit;
    std::atomic_bool _isStart;
    std::atomic_bool _isFinish;
    LibString _rootDirName;
    LogIniCfgMgr *_logConfigMgr;
    std::atomic<Int64> _curCacheBytes;
    UInt64 _logTimerIntervalMs;

    std::vector<SpecifyLog *> _fileIdIdxRefLog;     

    // 线程锁,线程,flush时间间隔
    std::vector<LibThread *> _flushThreads;                         // 下标与线程相关性id绑定
    std::vector<ConditionLocker *> _flushLocks;                     // 锁
    std::vector<std::vector<SpecifyLog *> *> _threadRelationLogs;   // 下标与线程相关id绑定,目的在于一个线程执行多个日志的着盘
};


KCP_CPP_END

#endif
