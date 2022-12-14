
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_INI_FILE_DEFS_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_INI_FILE_DEFS_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/LibString.h>
#include <kcp/LogDefs.h>
#include <kcp/LibDelegate.h>
#include <kcp/util/RttiUtil.h>
#include <kcp/util/SystemUtil.h>

// 函数与行号便利宏
#undef _FUNC_LINE_ARGS_
#define _FUNC_LINE_ARGS_ __FILE__, __FUNCTION__, __LINE__

#pragma region // 获取tag

// 类实例获取tag
#undef LOG_OBJ_TAG
#define LOG_OBJ_TAG() KCP_CPP_NS::RttiUtil::GetByType<decltype(*this)>()
// 非类实例获取tag
#undef LOG_NON_OBJ_TAG
#define LOG_NON_OBJ_TAG(classType) KCP_CPP_NS::RttiUtil::GetByType<classType>()
#pragma endregion

// 带修饰的log宏
#undef LOGFMT_DETAIL
#define LOGFMT_DETAIL(tag, x) tag, _FUNC_LINE_ARGS_, x

#pragma region // 带tag宏,除了Net接口特殊外都可用

// 类实例tag
#undef LOGFMT_OBJ_TAG
#define LOGFMT_OBJ_TAG(x) LOGFMT_DETAIL(LOG_OBJ_TAG(), x)

// 非类实例tag
#undef LOGFMT_NON_OBJ_TAG
#define LOGFMT_NON_OBJ_TAG(classType, x) LOGFMT_DETAIL(LOG_NON_OBJ_TAG(classType), x)
#pragma endregion

KCP_CPP_BEGIN

class ILog
{
public:
    // static ILog *GetDefaultInstance();

    virtual bool Init(const Byte8 *logConfigFile = "LogCfg.ini", const Byte8 *logCfgDir = NULL) = 0;
    virtual void Start() = 0;
    virtual void Close() = 0;
    virtual void FlushAll() = 0;
    virtual const LibString &GetLogRootPath() const = 0;
    virtual bool IsStart() const = 0;

public:
    #pragma region // 进程日志 文件名使用进程名 使用通用日志宏
    void Info(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...);
    void Debug(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...);
    void Warn(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...);
    void Error(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...);
    #pragma endregion

    // 崩溃日志 文件名字:Crash
    void Crash(const char *fmt, ...) ;
    
    // 网络日志 文件名: Net
    void Net(const Byte8 *tag, const char *fmt, ...) ;
    void NetDebug(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...) ;
    void NetWarn(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...) ;
    void NetInfo(const Byte8 *tag, const char *fileName,  const char *funcName, Int32 codeLine, const char *fmt, ...) ;
    void NetError(const Byte8 *tag, const char *fileName,  const char *funcName, Int32 codeLine, const char *fmt, ...) ;
    void NetTrace(const Byte8 *tag, const char *fileName,  const char *funcName, Int32 codeLine, const char *fmt, ...) ;

    // 系统日志
    void Sys(const Byte8 *tag, const char *fileName,  const char *funcName, Int32 codeLine, const char *fmt, ...) ;
    // 内存监视器
    void MemMonitor(const char *fmt, ...) ;

    // 自定义 文件名:Custom
    void Custom(const char *fmt, ...) ;

    // 追踪日志
    void Trace(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...) ;

    #pragma region // hook函数安装与卸载 // 线程不安全 原则上不可在多线程环境下使用 必须在单线程情况下设置与卸载
public:
    template<typename ObjType>
    const IDelegate<void> * InstallAfterLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*cb)());
    const IDelegate<void> * InstallAfterLogHookFunc(Int32 level, void (*cb)());
    template<typename ObjType>
    const IDelegate<void, LogData *> * InstallBeforeLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*cb)(LogData *logData));
    const IDelegate<void, LogData *> * InstallBeforeLogHookFunc(Int32 level, void(*cb)(LogData *logData));
    
    virtual void UnInstallAfterLogHookFunc(Int32 level, const IDelegate<void> *delegate) = 0;
    virtual void UnInstallBeforeLogHookFunc(Int32 level, const IDelegate<void, LogData *> *delegate) = 0;

protected:
    virtual void _InstallAfterLogHookFunc(const LogLevelCfg *levelCfg, IDelegate<void> *delegate) = 0;
    virtual void _InstallBeforeLogHookFunc(const LogLevelCfg *levelCfg, IDelegate<void, LogData *> *delegate) = 0;
    #pragma endregion

public:
    bool IsEnable(Int32 level) const;
    virtual bool IsLogOpen() const = 0;

protected:
    virtual const LogLevelCfg *_GetLevelCfg(Int32 level) const = 0;
    virtual void _WriteLog(const LogLevelCfg *levelCfg, LogData *logData) = 0;

    // 继承后可调用的写日志模版
    void _Common1(const Byte8 *tag, Int32 levelId, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, va_list va);
    void _Common2(const Byte8 *tag, Int32 levelId, const char *fmt, va_list va);
    // 不带前缀单纯输出数据
    void _Common3(Int32 levelId, const char *fmt, va_list va);
    // 带log time 与日志级别
    void _Common4(Int32 levelId, const char *fmt, va_list va);
};

inline void ILog::Info(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::Info, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::Debug(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::Debug, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::Warn(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::Warn, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::Error(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::Error, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::Crash(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common4(LogLevel::Crash, fmt, va);
    va_end(va);
}

inline void ILog::Net(const Byte8 *tag, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common2(tag, LogLevel::Net, fmt, va);
    va_end(va);
}

inline void ILog::NetDebug(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::NetDebug, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::NetWarn(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::NetWarn, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::NetInfo(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::NetInfo, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::NetError(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::NetError, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::NetTrace(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::NetTrace, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::Sys(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::Sys, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

inline void ILog::MemMonitor(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common4(LogLevel::MemMonitor, fmt, va);
    va_end(va);
}

inline void ILog::Custom(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common3(LogLevel::Custom, fmt, va);
    va_end(va);
}

// 追踪日志
inline void ILog::Trace(const Byte8 *tag, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    _Common1(tag, LogLevel::Trace, fileName, funcName, codeLine, fmt, va);
    va_end(va);
}

template<typename ObjType>
inline const IDelegate<void> *ILog::InstallAfterLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*cb)())
{
    if(UNLIKELY(!IsLogOpen()))
        return NULL;
 
    auto levelCfg = _GetLevelCfg(level);
    if(UNLIKELY(!levelCfg || !levelCfg->_enable))
        return NULL;

    if(UNLIKELY(IsStart()))
    {
        CRYSTAL_TRACE("log has already start InstallAfterLogHookFunc");
        return NULL;
    }

    auto newDelegate = DelegateFactory::Create(obj, cb);
    _InstallAfterLogHookFunc(levelCfg, newDelegate);
    return newDelegate;
}

inline const IDelegate<void> *ILog::InstallAfterLogHookFunc(Int32 level, void (*cb)())
{
    if(UNLIKELY(!IsLogOpen()))
        return NULL;
 
    auto levelCfg = _GetLevelCfg(level);
    if(UNLIKELY(!levelCfg || !levelCfg->_enable))
        return NULL;

    if(UNLIKELY(IsStart()))
    {
        CRYSTAL_TRACE("log has already start InstallAfterLogHookFunc");
        return NULL;
    }

    auto newDelegate = DelegateFactory::Create(cb);
    _InstallAfterLogHookFunc(levelCfg, newDelegate);
    return newDelegate;
}

template<typename ObjType>
inline const IDelegate<void, LogData *> *ILog::InstallBeforeLogHookFunc(Int32 level, ObjType *obj, void (ObjType::*cb)(LogData *logData))
{
    if(UNLIKELY(!IsLogOpen()))
        return NULL;
 
    auto levelCfg = _GetLevelCfg(level);
    if(UNLIKELY(!levelCfg || !levelCfg->_enable))
        return NULL;

    if(UNLIKELY(IsStart()))
    {
        CRYSTAL_TRACE("log has already start InstallBeforeLogHookFunc");
        return NULL;
    }

    auto newDelegate = DelegateFactory::Create(obj, cb);
    _InstallBeforeLogHookFunc(levelCfg, newDelegate);
    return newDelegate;
}

inline const IDelegate<void, LogData *> *ILog::InstallBeforeLogHookFunc(Int32 level, void(*cb)(LogData *logData))
{
    if(UNLIKELY(!IsLogOpen()))
        return NULL;
 
    auto levelCfg = _GetLevelCfg(level);
    if(UNLIKELY(!levelCfg || !levelCfg->_enable))
        return NULL;

    if(UNLIKELY(IsStart()))
    {
        CRYSTAL_TRACE("log has already start InstallBeforeLogHookFunc");
        return NULL;
    }
    
    auto newDelegate = DelegateFactory::Create(cb);
    _InstallBeforeLogHookFunc(levelCfg, newDelegate);
    return newDelegate;
}

inline bool ILog::IsEnable(Int32 level) const
{
    auto levelCfg = _GetLevelCfg(level);
    if(UNLIKELY(!levelCfg || !levelCfg->_enable))
        return false;

    return true;
}


inline void ILog::_Common1(const Byte8 *tag, Int32 levelId, const char *fileName, const char *funcName, Int32 codeLine, const char *fmt, va_list va)
{
    if(UNLIKELY(!IsLogOpen()))
        return;
 
    auto levelCfg = _GetLevelCfg(levelId);
    if(UNLIKELY(!levelCfg))
    {
        CRYSTAL_TRACE("log level[%d] cfg not found", levelId);
        return;
    }

    // 是否需要输出日志
    if(UNLIKELY(!levelCfg->_enable))
        return;

    // 构建日志数据
    LogData *newLogData = new LogData();
    newLogData->_logTime.UpdateTime();
    newLogData->_logInfo.AppendFormat("%s<%s>[%s][%s][line:%d]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , levelCfg->_levelName.c_str()
                                   , (tag ? tag : "")
                                   , funcName
                                   , codeLine)
                            .AppendFormatWithVaList(fmt, va)
                            .AppendEnd();

    _WriteLog(levelCfg, newLogData);
}

inline void ILog::_Common2(const Byte8 *tag, Int32 levelId, const char *fmt, va_list va)
{
    if(UNLIKELY(!IsLogOpen()))
        return;
 
    auto levelCfg = _GetLevelCfg(levelId);
    if(UNLIKELY(!levelCfg))
    {
        CRYSTAL_TRACE("log level[%d] cfg not found", levelId);
        return;
    }

    // 是否需要输出日志
    if(UNLIKELY(!levelCfg->_enable))
        return;

    // 构建日志数据
    LogData *newLogData = new LogData();
    newLogData->_logTime.UpdateTime();
    newLogData->_logInfo.AppendFormat("%s<%s>[%s]: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , levelCfg->_levelName.c_str()
                                   , (tag ? tag : ""))
                        .AppendFormatWithVaList(fmt, va)
                        .AppendEnd();

    _WriteLog(levelCfg, newLogData);
}

inline void ILog::_Common3(Int32 levelId, const char *fmt, va_list va)
{
    if(UNLIKELY(!IsLogOpen()))
        return;
 
    auto levelCfg = _GetLevelCfg(levelId);
    if(UNLIKELY(!levelCfg))
    {
        CRYSTAL_TRACE("log level[%d] cfg not found", levelId);
        return;
    }

    // 是否需要输出日志
    if(UNLIKELY(!levelCfg->_enable))
        return;

    // 构建日志数据
    LogData *newLogData = new LogData();
    newLogData->_logTime.UpdateTime();
    newLogData->_logInfo.AppendFormatWithVaList(fmt, va)
                        .AppendEnd();

    _WriteLog(levelCfg, newLogData);
}

inline void ILog::_Common4(Int32 levelId, const char *fmt, va_list va)
{
    if(UNLIKELY(!IsLogOpen()))
        return;
 
    auto levelCfg = _GetLevelCfg(levelId);
    if(UNLIKELY(!levelCfg))
    {
        CRYSTAL_TRACE("log level[%d] cfg not found", levelId);
        return;
    }

    // 是否需要输出日志
    if(UNLIKELY(!levelCfg->_enable))
        return;

    // 构建日志数据
    LogData *newLogData = new LogData();
    newLogData->_logTime.UpdateTime();
    newLogData->_logInfo.AppendFormat("%s<%s>: "
                                   , newLogData->_logTime.ToString().c_str()
                                   , levelCfg->_levelName.c_str())
                            .AppendFormatWithVaList(fmt, va) 
                            .AppendEnd();

    _WriteLog(levelCfg, newLogData);
}
    
KCP_CPP_END

extern KCP_CPP_NS::ILog *g_Log;

#endif
