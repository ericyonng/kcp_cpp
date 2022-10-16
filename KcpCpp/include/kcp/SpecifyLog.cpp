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
 * Date: 2021-02-17 16:16:34
 * Author: Eric Yonng
 * Description: 
*/

#include <pch.h>
#include <kcp/IniFileDefs.h>
#include <kcp/LibFile.h>
#include <kcp/LibIniFile.h>
#include <kcp/LibLogFile.h>
#include <kcp/ConsoleConfigMgr.h>
#include <kcp/SpecifyLog.h>
#include <kcp/util/FileUtil.h>
#include <kcp/LibThread.h>
#include <kcp/LogData.h>
#include <kcp/LogDefs.h>

KCP_CPP_BEGIN

SpecifyLog::SpecifyLog()
:_isInit{false}
,_isStart{false}
,_isClose{false}
,_config(NULL)
,_logFile(NULL)
,_logData(new std::list<LogData *>)
,_swapData(new std::list<LogData *>)
,_wakeupFlush(NULL)
{

}

SpecifyLog::~SpecifyLog()
{
    Close();
}

Int32 SpecifyLog::Init(const LibString &rootDirName, const LogConfig *cfg)
{
    if(_isInit.exchange(true))
    {
        return Status::Success;
    }

    if(UNLIKELY(_logFile))
    {
        return Status::Repeat;
    }

    _config = cfg;

    // hook
    const auto refLevelCount = _config->_levelIdxRefCfg.size();
    _beforeHook.resize(refLevelCount);
    _afterHook.resize(refLevelCount);

    // 1.文件名
    LibString logName = rootDirName + _config->_logFileName + _config->_extName;

    // 2.创建日志文件
    _logFile = new LibLogFile();
    // CRYSTAL_TRACE("will create log file:%s", logName.c_str());
    bool isFileExist = true;
    if(!_logFile->Open(logName.c_str(), &isFileExist, true, "ab+", true))
    {
        return Status::Log_CreateLogFileFail;
    }

    // 3.备份旧日志
    // _logFile->PartitionFile(!isFileExist);
    _logFile->UpdateLastPassDayTime();

    return Status::Success;
}

Int32 SpecifyLog::Start()
{
    if(_isStart.exchange(true))
    {
        return Status::Success;
    }

    if(!_isInit.load())
    {
        return Status::NotInit;
    }
    
    return Status::Success;
}

void SpecifyLog::Close()
{
    if(_isClose.exchange(true))
        return;

    _isInit = false;
    _isStart = false;

    // 着盘
    Flush();

    // 关闭文件
    LibString logName;
    if (LIKELY(_logFile))
    {
        logName = _logFile->GetFileName();
        _logFile->Close();
    }

    // 清理资源
    for(auto iter = _beforeHook.begin(); iter != _beforeHook.end(); ++iter)
    {
        auto hookList = *iter;
        if(!hookList)
            continue;

        delete hookList;
    }
    for(auto iter = _afterHook.begin(); iter != _afterHook.end(); ++iter)
    {
        auto hookList = *iter;
        if (!hookList)
            continue;

        delete hookList;
    }

    // 日志文件
    if(LIKELY(_logFile))
        delete _logFile;

    _logFile = NULL;

    // 清理数据
    for(auto iter = _logData->begin(); iter != _logData->end();)
    {
        iter = _logData->erase(iter);
    }
    for(auto iter = _swapData->begin(); iter != _swapData->end();)
    {
        iter = _swapData->erase(iter);
    }

    delete _logData;
    delete _swapData;

}

void SpecifyLog::_OnThreadWriteLog()
{
    _logLck.Lock();
    if(_logData->empty())
    {
        // CRYSTAL_TRACE("%s have no log data to drop hardware.", _config->_logFileName.c_str());
        _logLck.Unlock();
        return;
    }

    // 只交换数据队列指针拷贝最少，最快，
    // 而且交换后_logDatas队列是空的相当于清空了数据 
    // 保证缓冲队列前几个都不为NULL, 碰到NULL表示结束
    // 由于主线程不会共享_logCaches所以是线程安全的（每个日志文件线程只有一个线程）
    // ,请保证外部其他线程不会调用本接口，需要立即写日志请调用flushall接口
    std::list<LogData *> *cache4RealLog = _logData;
    _logData = _swapData;
    _swapData = cache4RealLog;
    _logLck.Unlock();

    // 没有日志
    if(_swapData->empty())
    {
        // CRYSTAL_TRACE("%s have no swap data to drop hardware.", _config->_logFileName.c_str());
        return; 
    }

   // 2.写日志
   const Int64 maxFileSize = _config->_maxFileSize;
   for(auto iterLog = _swapData->begin(); iterLog != _swapData->end();)
   {
        // 1.跨天重新打开
        auto &logData = *iterLog;
        if(_logFile->IsDayPass(logData->_logTime))
        {
            // 跨天需要重新打开文件
            _logFile->Reopen(&logData->_logTime);
            _logFile->UpdateLastPassDayTime(&logData->_logTime);
        }

        // 2.文件过大转储到分立文件
        if(_logFile->IsTooLarge(maxFileSize))
            _logFile->PartitionFile(false, &logData->_logTime);

        // 3.写入文件
        _logFile->Write(logData->_logInfo.c_str(), logData->_logInfo.size());
        delete logData;

       iterLog = _swapData->erase(iterLog);
   }

    _logFile->Flush();
}

// void SpecifyLog::_OnThreadWork(LibThread *thread)
// {
//     const UInt64 intervalMs = static_cast<UInt64>(_config->_intervalMs);
//     CRYSTAL_TRACE("log file name thread start[%s] intervalMs[%llu]", _config->_logFileName.c_str(), intervalMs);
//     while (true)
//     {
//         if(UNLIKELY(thread->IsDestroy()))
//         {
//             _OnThreadWriteLog();
//             break;
//         }

//         // 写日志
//         _OnThreadWriteLog();

//         _wakeupLck.Lock();
//         _wakeupLck.TimeWait(intervalMs);
//         _wakeupLck.Unlock();

//         // CRYSTAL_TRACE("log file name thread wake up[%s]", _config->_logFileName.c_str());

//     }

//     CRYSTAL_TRACE("log thread close log file name:%s", _config->_logFileName.c_str());
// }

const LibString &SpecifyLog::GetLogName() const
{
    return _logFile->GetFileName();
}
KCP_CPP_END
