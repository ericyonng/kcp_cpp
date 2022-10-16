
#include <pch.h>
#include <kcp/LibThread.h>
#include <kcp/util/SystemUtil.h>

KCP_CPP_BEGIN

void LibThread::Release()
{
    delete this;
}

void LibThread::LibThreadHandlerLogic(void *param)
{
    LibThread *libThread = static_cast<LibThread *>(param);
    ConditionLocker &condLck = libThread->_condLck;
    ConditionLocker &quitLck = libThread->_quitLck;
    SpinLock &taskLck = libThread->_taskLck;
    std::atomic_bool &isBusy = libThread->_isBusy;
    std::atomic_bool &isWorking = libThread->_isWorking;
    std::atomic_bool &isDestroy = libThread->_isDestroy;
    std::atomic_bool &enableAddTask = libThread->_enableAddTask;
    std::list<ITask *> &taskList = libThread->_tasks;
    libThread->_threadId = SystemUtil::GetCurrentThreadId();

    isWorking = true;

    bool isEmpty = false;
    while(!isDestroy.load() || !isEmpty)
    {
        taskLck.Lock();
        if(LIKELY(!taskList.empty()))
        {
            isBusy = true;
            auto task = taskList.front();
            taskList.pop_front();
            taskLck.Unlock();

            isEmpty = false;
            task->Run();
            task->Release();
            continue;
        }
        else
        {
            taskLck.Unlock();
        }

        isBusy = false;
        condLck.Lock();
        condLck.Wait();
        condLck.Unlock();

        taskLck.Lock();
        isEmpty = taskList.empty();
        taskLck.Unlock();
    }

    try
    {

        quitLck.Lock();

        isWorking = false;
        isBusy = false;
        enableAddTask = false;
        quitLck.Unlock();

        quitLck.Sinal();
    }
    catch(...)
    {
        std::cout << "LibThreadHandlerLogic quitLck crash" << std::endl;
    }
}


#ifdef _WIN32
unsigned __stdcall LibThread::ThreadHandler(void *param)
{
    LibThreadHandlerLogic(param);
    _endthreadex(0L);

    return 0L;
}

#else

void *LibThread::ThreadHandler(void *param)
{
    // 线程分离
    pthread_detach(::pthread_self());

    LibThreadHandlerLogic(param);

    pthread_exit((void *)0);
}

#endif

KCP_CPP_END

