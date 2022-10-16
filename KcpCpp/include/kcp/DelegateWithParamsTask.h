
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_DELEGATE_TASK_WITH_PARAMS_TASK_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_DELEGATE_TASK_WITH_PARAMS_TASK_H__

#pragma once


#include <kcp/common/common.h>
#include <kcp/ITask.h>
#include <kcp/LibDelegate.h>

KCP_CPP_BEGIN

template<typename ObjType>
class DelegateWithParamsTask : public ITask
{

public:
    DelegateWithParamsTask(ObjType *obj, IDelegate<void, ObjType *, UInt64> *callback, UInt64 param);
    virtual ~DelegateWithParamsTask();

    virtual void Run();
    virtual void Release();
    IDelegate<void, ObjType *, UInt64> *PopCallback();

private:
    ObjType *_obj;
    IDelegate<void, ObjType *, UInt64> *_callback;
    UInt64 _param;
};

template<typename ObjType>
inline DelegateWithParamsTask<ObjType>::DelegateWithParamsTask(ObjType *obj, IDelegate<void, ObjType *, UInt64> *callback, UInt64 param)
    :_obj(obj)
    ,_callback(callback)
    ,_param(param)
{
    
}

template<typename ObjType>
inline DelegateWithParamsTask<ObjType>::~DelegateWithParamsTask()
{
    if(_callback)
     delete _callback;
    _callback = NULL;
}

template<typename ObjType>
inline void DelegateWithParamsTask<ObjType>::Run()
{
    _callback->Invoke(_obj, _param);
}

template<typename ObjType>
inline void DelegateWithParamsTask<ObjType>::Release()
{
    delete this;
}

template<typename ObjType>
inline IDelegate<void, ObjType *, UInt64> *DelegateWithParamsTask<ObjType>::PopCallback()
{
    auto cb = _callback;
    _callback = NULL;
    return cb;
}

KCP_CPP_END

#endif
