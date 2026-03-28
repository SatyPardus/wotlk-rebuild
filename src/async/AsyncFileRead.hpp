#ifndef ASYNC_ASYNC_FILE_READ_HPP
#define ASYNC_ASYNC_FILE_READ_HPP

#include "async/CAsyncQueue.hpp"
#include "async/CAsyncThread.hpp"
#include <common/Prop.hpp>
#include <storm/Thread.hpp>
#include <storm/array/TSGrowableArray.hpp>

#define NUM_ASYNC_QUEUES 3

class CAsyncObject;
typedef void (*CALLBACK_FUNC)(float, void*);
typedef void (*POLL_FUNC)();
typedef int32_t (*STATUS_FUNC)();

class AsyncFileRead {
    public:
        // Static variables
        static uint32_t s_threadSleep;
        static uint32_t s_handlerTimeout;
        static CAsyncObject* s_asyncWaitObject;
        static CALLBACK_FUNC s_progressCallback;
        static void* s_progressParam;
        static int32_t s_progressCount;
        static void* s_ingameProgressCallback;
        static void* s_ingameStartCallback;
        static HPROPCONTEXT s_propContext;
        static SEvent s_shutdownEvent;
        static const char* s_asyncQueueNames[];
        static CAsyncQueue* s_asyncQueues[];
        static SCritSect s_queueLock;
        static SCritSect s_userQueueLock;
        static TSList<CAsyncQueue, TSGetLink<CAsyncQueue>> s_asyncQueueList;
        static TSList<CAsyncThread, TSGetLink<CAsyncThread>> s_asyncThreadList;
        static STORM_EXPLICIT_LIST(CAsyncObject, link) s_asyncFileReadPostList;
        static STORM_EXPLICIT_LIST(CAsyncObject, link) s_asyncFileReadFreeList;
        static TSGrowableArray<POLL_FUNC> s_asyncPollHandlers;
        static TSGrowableArray<STATUS_FUNC> s_asyncStatusHandlers;
        static int32_t s_waiting;
};

CAsyncQueue* AsyncFileReadCreateQueue();

void AsyncFileReadCreateThread(CAsyncQueue* queue, const char* queueName);

void AsyncFileReadLinkObject(CAsyncObject* object, int32_t a2);

int32_t AsyncFileReadPollHandler(const void* a1, void* a2);

uint32_t AsyncFileReadThread(void* thread);

bool AsyncFileReadIsReading();

void AsyncFileReadWait(CAsyncObject* object);

void AsyncFileReadWaitAll();

void AsyncFileReadAddPollHandler(POLL_FUNC method);

void AsyncFileReadAddStatusHandler(STATUS_FUNC method);

#endif
