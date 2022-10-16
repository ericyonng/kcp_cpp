
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_SPECIFY_LOG_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_SPECIFY_LOG_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibDelegate.h>
#include <kcp/SpinLock.h>
#include <kcp/ConditionLocker.h>
#include <kcp/LogConfig.h>
#include <kcp/LogData.h>
#include <kcp/util/SystemUtil.h>
#include <kcp/LogLevel.h>

KCP_CPP_BEGIN

class LibLogFile;
class LibThread;

class SpecifyLog
{
    friend class LibLog;
public:
    SpecifyLog();
    ~SpecifyLog();

public:
    Int32 Init(const LibString &rootDirName, const LogConfig *cfg);
    void BindWakeupFlush(ConditionLocker *flusher);
    Int32 Start();
    void Close();
    void Flush();
    void WriteLog(const LogLevelCfg &levelCfg, LogData *logData);
    Int32 GetThreadRelationId() const;

    // 在start之前安装hook
#pragma region // 线程不安全 原则上不可在多线程环境下使用
    void InstallBeforeHook(Int32 level, IDelegate<void, LogData *> *deleg);
    void InstallAfterHook(Int32 level, IDelegate<void> *deleg);
    void UnInstallAfterLogHookFunc(Int32 level, const IDelegate<void> *deleg);
    void UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *deleg);
#pragma endregion

    bool IsClose() const;
    const LibString &GetLogName() const;

private:
    void _OnThreadWriteLog();
    void _WakupFlush();

private:
    void _OutputToConsole(const LogLevelCfg &levelCfg, const LibString &logInfo);

private:
    std::atomic_bool _isInit;
    std::atomic_bool _isStart;
    std::atomic_bool _isClose;
    const LogConfig *_config;
    std::vector<std::list<IDelegate<void, LogData *> *> *> _beforeHook; // level 做下标索引
    std::vector<std::list<IDelegate<void> *> *> _afterHook;    // level 做下标索引
    LibLogFile *_logFile;

    // 锁
    SpinLock _logLck;
    ConditionLocker *_wakeupFlush;

    std::list<LogData *> *_logData;
    std::list<LogData *> *_swapData;
};

inline void SpecifyLog::BindWakeupFlush(ConditionLocker *flusher)
{
    _wakeupFlush = flusher;
}

inline void SpecifyLog::Flush()
{
    _wakeupFlush->Sinal();
}

inline void SpecifyLog::WriteLog(const LogLevelCfg &levelCfg, LogData *logData)
{
    // 1.外部判断,按理来说不应该在日志线程即将关闭或者关闭时写日志
    if(UNLIKELY(_isClose.load()))
    {
        delete logData;
        return;
    }

    // 1.是否允许写日志 外部判断
    // if(!levelCfg._enable)
    //     return false;
        
    // 1.不可做影响log执行的其他事情
    auto beforeHookList = _beforeHook[levelCfg._level];
    if(beforeHookList)
    {
        for(auto iter:*beforeHookList)
            iter->Invoke(logData);
    }

    // 3.拷贝一次数据
    // LogData cache = *logData;

    // 3.打印到控制台
    if(levelCfg._enableConsole)
        _OutputToConsole(levelCfg, logData->_logInfo);

    // 4.将日志数据放入队列
    _logLck.Lock();
    _logData->push_back(logData);
    _logLck.Unlock();

    // // 4.实时写日志
    if(UNLIKELY(levelCfg._enableRealTime))
        _wakeupFlush->Sinal();

    // 6.日志后hook
    auto afterHookList = _afterHook[levelCfg._level];
    if(afterHookList)
    {
        for(auto iter:*afterHookList)
            iter->Invoke();
    }
}

inline Int32 SpecifyLog::GetThreadRelationId() const
{
    return _config->_threadRelationId;
}

inline void SpecifyLog::_WakupFlush()
{
    _wakeupFlush->Sinal();
}

inline void SpecifyLog::_OutputToConsole(const LogLevelCfg &levelCfg, const LibString &logInfo)
{
    SystemUtil::LockConsole();
    const Int32 oldColor = SystemUtil::GetConsoleColor();
    SystemUtil::SetConsoleColor(levelCfg._fgColor | levelCfg._bgColor);
    SystemUtil::OutputToConsole(logInfo);
    SystemUtil::SetConsoleColor(oldColor);
    SystemUtil::UnlockConsole();
}

inline void SpecifyLog::InstallBeforeHook(Int32 level, IDelegate<void, LogData *> *deleg)
{
    auto hookList = _beforeHook[level];
    if(UNLIKELY(!hookList))
    {
        hookList = new std::list<IDelegate<void, LogData *> *>;
        _beforeHook[level] = hookList;
    }

    hookList->push_back(deleg);
}

inline void SpecifyLog::InstallAfterHook(Int32 level, IDelegate<void> *deleg)
{   
    auto hookList = _afterHook[level];
    if(UNLIKELY(!hookList))
    {
        hookList = new std::list<IDelegate<void> *>;
        _afterHook[level] = hookList;
    }

    hookList->push_back(deleg);
}

inline void SpecifyLog::UnInstallAfterLogHookFunc(Int32 level, const IDelegate<void> *deleg)
{
    auto hookList = _afterHook[level];
    if(!hookList)
        return;

    for(auto iter = hookList->begin(); iter != hookList->end(); ++iter)
    {
        if(*iter == deleg)
        {
            (*iter)->Release();
            hookList->erase(iter);
            return;
        }
    }
}

inline void SpecifyLog::UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *deleg)
{
    auto hookList = _beforeHook[level];
    if(!hookList)
        return;

    for(auto iter = hookList->begin(); iter != hookList->end(); ++iter)
    {
        if(*iter == deleg)
        {
            (*iter)->Release();
            hookList->erase(iter);
            return;
        }
    }
}

inline bool SpecifyLog::IsClose() const
{
    return _isClose.load();
}


KCP_CPP_END

#endif