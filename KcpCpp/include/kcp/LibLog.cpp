
#include <pch.h>
#include <kcp/LibLog.h>
#include <kcp/SpecifyLog.h>
#include <kcp/LogIniCfgMgr.h>
#include <kcp/util/SystemUtil.h>
#include <kcp/util/DirectoryUtil.h>
#include <kcp/util/FileUtil.h>
#include <kcp/LogConfigTemplate.h>
#include <kcp/LibThread.h>
#include <kcp/util/CrashUtil.h>

KCP_CPP_NS::ILog *g_Log = NULL;

KCP_CPP_BEGIN

LibLog::LibLog()
    :_isInit{false}
    ,_isStart{false}
    ,_isFinish{false}
    ,_logConfigMgr(NULL)
    ,_curCacheBytes{0}
    ,_logTimerIntervalMs(0)
{
    
}

LibLog::~LibLog()
{
    Close();
}

bool LibLog::Init(const Byte8 *logConfigFile, const Byte8 *logCfgDir)
{
    if (_isInit.exchange(true))
    {
        return true;
    }

    // 获取目录
    LibString iniDir;
    const LibString progDir = SystemUtil::GetCurProgRootPath();

    if(!logCfgDir)
    {
        iniDir = progDir + ROOT_DIR_INI_SUB_DIR;
    }
    else
    {
        iniDir = logCfgDir;
    }

    // 路径校验
    LibString cfgFile = iniDir;
    cfgFile += logConfigFile;

    // 配置文件不存在则自动创建
    if(UNLIKELY(!FileUtil::IsFileExist(cfgFile.c_str())))
    {
        // 创建目录
        if(!DirectoryUtil::CreateDir(iniDir))
            return false;

        auto fp = FileUtil::OpenFile(cfgFile.c_str(), true);
        if(!fp)
        {
            return false;
        }
        
        const auto &content = LogConfigTemplate::GetLogConfigIniContent();
        FileUtil::WriteFile(*fp, content.data(), content.size());
        FileUtil::FlushFile(*fp);
        FileUtil::CloseFile(*fp);
    }

    // 初始化配置
    _logConfigMgr = new LogIniCfgMgr();
    if(!_logConfigMgr->Init(cfgFile.c_str()))
    {
        return false;
    }

    // 是否需要开启日志
    if(!_logConfigMgr->IsEnableLog())
    {
        return true;
    }

    _logTimerIntervalMs = _logConfigMgr->GetIntervalMs();
    LibString specifyLogFileDir = _logConfigMgr->GetLogPath();

    // 初始化日志对象
    auto &logCfgs = _logConfigMgr->GetLogFileCfgs();
    const Int32 logCount = static_cast<Int32>(logCfgs.size());
    _fileIdIdxRefLog.resize(_logConfigMgr->GetMaxLogFileId() + 1);
    for(Int32 i = 0; i < logCount; ++i)
    {
        auto cfg = logCfgs[i];
        if(cfg && cfg->_isEnable)
        {
            auto newSpecifyLog = new SpecifyLog();
            const Int32 status = newSpecifyLog->Init(specifyLogFileDir, cfg);
            if(status != Status::Success)
            {
                delete newSpecifyLog;
                newSpecifyLog = NULL;
                continue;
            }

            // 日志着盘线程绑定关系
            if(_flushThreads.size() <= newSpecifyLog->GetThreadRelationId())
                _flushThreads.resize(newSpecifyLog->GetThreadRelationId() + 1);
            if(_threadRelationLogs.size() <= newSpecifyLog->GetThreadRelationId())
                _threadRelationLogs.resize(newSpecifyLog->GetThreadRelationId() + 1);
            if(_flushLocks.size() <= newSpecifyLog->GetThreadRelationId())
                _flushLocks.resize(newSpecifyLog->GetThreadRelationId() + 1);
            auto flushThread = _flushThreads[newSpecifyLog->GetThreadRelationId()];
            if(UNLIKELY(!flushThread))
            {
                flushThread = new LibThread;
                _flushThreads[newSpecifyLog->GetThreadRelationId()] = flushThread;
                _flushLocks[newSpecifyLog->GetThreadRelationId()] = new ConditionLocker;
            }
            auto threadBindLogs = _threadRelationLogs[newSpecifyLog->GetThreadRelationId()];
            if(UNLIKELY(!threadBindLogs))
            {
                threadBindLogs = new std::vector<SpecifyLog *>;
                _threadRelationLogs[newSpecifyLog->GetThreadRelationId()] = threadBindLogs;
            }
            threadBindLogs->push_back(newSpecifyLog);

            newSpecifyLog->BindWakeupFlush(_flushLocks[newSpecifyLog->GetThreadRelationId()]);
            _fileIdIdxRefLog[cfg->_logFileId] = newSpecifyLog;
        }
    }

    _rootDirName = specifyLogFileDir;

    return true;
}

void LibLog::Start()
{
    if(UNLIKELY(!_isInit.load() || _isFinish.load()))
    {
        return;
    }

    if(_isStart.exchange(true))
    {
        return;
    }

    for(auto &log:_fileIdIdxRefLog)
    {
        if(log)
        {
            auto status = log->Start();
            if(status != Status::Success)
            {
            }
        }
    }

    const Int32 threadCount = static_cast<Int32>(_flushThreads.size());
    for(Int32 idx = 0; idx < threadCount; ++idx)
    {
        auto t = _flushThreads[idx];
        if(t)
        {
            t->AddTask2(this, &LibLog::_OnLogThreadFlush, idx);
            t->Start();
        }
    }
}

void LibLog::Close()
{
    if(_isFinish.exchange(true))
        return;

    _isInit = false;
    _isStart = false;

    // 关闭线程
    const Int32 threadCount = static_cast<Int32>(_flushThreads.size());
    for(Int32 idx = 0; idx < threadCount; ++idx)
    {
        auto t = _flushThreads[idx];
        if(LIKELY(t))
        {
            t->HalfClose();
            _flushLocks[idx]->Sinal();
        }
    }
    for(Int32 idx = 0; idx < threadCount; ++idx)
    {
        auto t = _flushThreads[idx];
        if(LIKELY(t))
            t->FinishClose();
    }
    
    for(auto &log:_fileIdIdxRefLog)
    {
        if(log)
        {
            log->Close();
            delete log;
        }
    }

    _flushThreads.clear();
    _flushLocks.clear();
    _threadRelationLogs.clear();
    
    _fileIdIdxRefLog.clear();

    _logConfigMgr->Close();
    delete _logConfigMgr;
}

void LibLog::FlushAll()
{
    for(auto &lck:_flushLocks)
    {
        if(LIKELY(lck))
            lck->Sinal();
    }
}


const LibString &LibLog::GetLogRootPath() const
{
    return _rootDirName;
}

bool LibLog::IsStart() const
{
    return _isStart;
}

bool LibLog::IsLogOpen() const
{
    return _logConfigMgr->IsEnableLog();
}

void LibLog::UnInstallAfterLogHookFunc(Int32 level, const IDelegate<void> *delegate)
{
    if(UNLIKELY(!IsLogOpen()))
        return;

    if(UNLIKELY(_isStart.load()))
    {
        return;
    }

    // 能够调用uninstall 说明已经install过了
    auto logCfg = _logConfigMgr->GetLogCfg(level);
    auto specifyLog = _fileIdIdxRefLog[logCfg->_logFileId];
    specifyLog->UnInstallAfterLogHookFunc(level, delegate);
}

void LibLog::UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *delegate)
{
    if(UNLIKELY(!IsLogOpen()))
        return;

    if(UNLIKELY(_isStart.load()))
    {
        return;
    }

    // 能够调用uninstall 说明已经install过了
    auto logCfg = _logConfigMgr->GetLogCfg(level);
    auto specifyLog = _fileIdIdxRefLog[logCfg->_logFileId];
    specifyLog->UnInstallBeforeLogHookFunc(level, delegate);
}
    
void LibLog::_InstallAfterLogHookFunc(const LogLevelCfg *levelCfg, IDelegate<void> *delegate)
{
    // 外部已排除是否开启日志
    auto specifyLog = _fileIdIdxRefLog[levelCfg->_fileId];
    specifyLog->InstallAfterHook(levelCfg->_level, delegate);
}

void LibLog::_InstallBeforeLogHookFunc(const LogLevelCfg *levelCfg, IDelegate<void, LogData *> *delegate)
{
    // 外部已排除是否开启日志
    auto specifyLog = _fileIdIdxRefLog[levelCfg->_fileId];
    specifyLog->InstallBeforeHook(levelCfg->_level, delegate);
}

const LogLevelCfg *LibLog::_GetLevelCfg(Int32 level) const
{
    return _logConfigMgr->GetLevelCfg(level);
}

void LibLog::_WriteLog(const LogLevelCfg *levelCfg, LogData *logData)
{
    auto specifyLog = _fileIdIdxRefLog[levelCfg->_fileId];

    // 开启打印堆栈信息
    if(levelCfg->_printStackTraceBack)
        logData->_logInfo << "\nStack Traceback:\n" << CrashUtil::CrystalCaptureStackBackTrace();

    Int64 curAllBytes = _curCacheBytes += logData->CalcBytes();
    specifyLog->WriteLog(*levelCfg, logData);

    // 缓存超过阈值强制着盘
    if(UNLIKELY(curAllBytes >= _logConfigMgr->GetMaxLogCacheBytes()))
    {
        _curCacheBytes = 0;
        FlushAll();
    }
}

void LibLog::_OnLogThreadFlush(LibThread *t, UInt64 param)
{
    const Int32 idx =  static_cast<Int32>(param);
    auto logs = _threadRelationLogs[idx];
    auto lck = _flushLocks[idx];
    const Int32 logCount = static_cast<Int32>(logs->size());
    LibString logNameList;
    for(auto &log:*logs)
    {
        logNameList.AppendFormat("%s|", log->GetLogName().c_str());
    }

    const UInt64 intervalMs = static_cast<UInt64>(_logTimerIntervalMs);

    while (true)
    {
        if(UNLIKELY(t->IsDestroy()))
        {
            _OnLogFlush(*logs, logCount);
            break;
        }

        // 写日志
        _OnLogFlush(*logs, logCount);

        lck->Lock();
        lck->TimeWait(intervalMs);
        lck->Unlock();
        // CRYSTAL_TRACE("log file name thread wake up[%s]", _config->_logFileName.c_str());
    }

}

void LibLog::_OnLogFlush(std::vector<SpecifyLog *> &logs, Int32 logCount)
{
    for(Int32 i = 0; i < logCount; ++i)
    {
        auto log = logs[i];
        log->_OnThreadWriteLog();
    }
}

KCP_CPP_END

