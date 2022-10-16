
#ifndef __KCP_CPP_KCP_CPP_INCLUDE_KCP_DELEGATE_TASK_H__
#define __KCP_CPP_KCP_CPP_INCLUDE_KCP_DELEGATE_TASK_H__

#pragma once

#include <kcp/common/common.h>
#include <kcp/ITask.h>
#include <kcp/LibDelegate.h>

KCP_CPP_BEGIN

template<typename ObjType>
class DelegateTask : public ITask
{
public:
    DelegateTask(ObjType *obj, IDelegate<void, ObjType *> *callback);
    virtual ~DelegateTask();

    virtual void Run();
    virtual void Release();
    IDelegate<void, ObjType *> *PopCallback();

private:
    ObjType *_obj;
    IDelegate<void, ObjType *> *_callback;
};

template<typename ObjType>
inline DelegateTask<ObjType>::DelegateTask(ObjType *obj, IDelegate<void, ObjType *> *callback)
    :_obj(obj)
    ,_callback(callback)
{
    
}

template<typename ObjType>
inline DelegateTask<ObjType>::~DelegateTask()
{
    delete _callback;
}

template<typename ObjType>
inline void DelegateTask<ObjType>::Run()
{
    _callback->Invoke(_obj);
}

template<typename ObjType>
inline void DelegateTask<ObjType>::Release()
{
    delete this;
    // CRYSTAL_DELETE(this);
}

template<typename ObjType>
inline IDelegate<void, ObjType *> *DelegateTask<ObjType>::PopCallback()
{
    auto cb = _callback;
    _callback = NULL;
    return cb;
}

KCP_CPP_END

#endif