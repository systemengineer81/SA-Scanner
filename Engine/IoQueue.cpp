
#include "pch.h"
#include "IoQueue.h"

BOOL
CIoQueue::CQueueInternal::Init()
{
    InitializeListHead(&m_QueueHead);

    m_queueReadyEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (m_queueReadyEvent == NULL)
    {
        return FALSE;
    }

    ::InitializeCriticalSection(&m_lockQueue);

    return TRUE;
}

VOID
CIoQueue::CQueueInternal::Finalize()
{

    ::DeleteCriticalSection(&m_lockQueue);
}

BOOL
CIoQueue::Initialize()
{
    assert(m_pQueue != 0);

    if (m_pQueue->Init() != TRUE)
    {
        return FALSE;
    }

    return TRUE;
}

VOID
CIoQueue::Finalize()
{

    this->m_pQueue->Finalize();
    this->m_pQueue = NULL;
}

VOID
CIoQueue::AddEvent(
    __in PQUEUE_EVENT  _event
    )
{

    //
    // get private event
    //
    QUEUE_EVENT_PRIVATE *privateEvent = CONTAINING_RECORD(
        _event, QUEUE_EVENT_PRIVATE, publicEvent);

#ifdef _DEBUG
    InterlockedIncrement(&m_pQueue->m_queueSize);
#endif // _DEBUG

    InterlockedAdd(&m_pQueue->m_DataSize, (LONG)privateEvent->publicEvent.dataSize);

    assert(m_pQueue->m_DataSize >= 0);

    InsertTailList(&m_pQueue->m_QueueHead, &privateEvent->Entry);

    if (this->m_pQueue->m_waited == TRUE)
    {
        SetEvent(m_pQueue->m_queueReadyEvent);
        this->m_pQueue->m_waited = FALSE;
    }
}

//
// get event from queue
// 
PQUEUE_EVENT
CIoQueue::GetEvent()
{

    if (!IsListEmpty(&this->m_pQueue->m_QueueHead))
    {
        PLIST_ENTRY entry = 0;


#ifdef _DEBUG
        InterlockedDecrement(&m_pQueue->m_queueSize);
#endif // _DEBUG

        entry = RemoveHeadList(&this->m_pQueue->m_QueueHead);

        PQUEUE_EVENT_PRIVATE  privateEvent = CONTAINING_RECORD(entry, QUEUE_EVENT_PRIVATE, Entry);

        InterlockedAdd(&m_pQueue->m_DataSize, -(LONG)(privateEvent->publicEvent.dataSize));

        assert(m_pQueue->m_DataSize >= 0);

        this->m_pQueue->Unlock();


        return &privateEvent->publicEvent;

    }

    return 0;
}

DWORD
CIoQueue::WaitForEvent(
    __in_opt HANDLE  hExitEvent,
    __in_opt DWORD  timeout
    )
{

    DWORD   dwWaitStatus;

    this->m_pQueue->Lock();

    //
    // queue is empty, wait for event
    //
    while (IsListEmpty(&this->m_pQueue->m_QueueHead))
    {
        this->m_pQueue->m_waited = TRUE;
        ResetEvent(this->m_pQueue->m_queueReadyEvent);

        if (hExitEvent != NULL)
        {
            HANDLE  waitEvents[2];

            waitEvents[0] = this->m_pQueue->m_queueReadyEvent;
            waitEvents[0] = hExitEvent;

            this->m_pQueue->Unlock();

            dwWaitStatus = WaitForMultipleObjects(2, waitEvents, FALSE, timeout);

        }
        else
        {
            this->m_pQueue->Unlock();

            dwWaitStatus = WaitForSingleObject(this->m_pQueue->m_queueReadyEvent, timeout);

        }

        if (dwWaitStatus == (WAIT_OBJECT_0 + 1))
        {
            return ERROR_DELETE_PENDING;
        }

        if (dwWaitStatus == WAIT_TIMEOUT)
        {
            return dwWaitStatus;
        }

        this->m_pQueue->Lock();
    }


    this->m_pQueue->Unlock();

    return ERROR_SUCCESS;
}

PQUEUE_EVENT
CIoQueue::AllocateEvent(
    __in SIZE_T size
    )
{
    //
    // perform data operations with released semaphore
    //
    QUEUE_EVENT_PRIVATE *privateEvent = reinterpret_cast<QUEUE_EVENT_PRIVATE *>(
        malloc(sizeof(QUEUE_EVENT_PRIVATE) - sizeof(UCHAR) + size));

    if (privateEvent == NULL) {

        return NULL;
    }

    privateEvent->realSize = size;
    privateEvent->publicEvent.dataSize = size;

    return &privateEvent->publicEvent;
}


VOID 
CIoQueue::ReleaseEvent(__in PQUEUE_EVENT _event)
{
    //
    // get private event
    //
    QUEUE_EVENT_PRIVATE *privateEvent = CONTAINING_RECORD(
        _event, QUEUE_EVENT_PRIVATE, publicEvent);

    free(privateEvent);
}

VOID 
CIoQueue::Flush()
{
    LIST_ENTRY  flushHead;

#if _DEBUG
    INT  flushes = 0;
#endif 
    InitializeListHead(&flushHead);

    this->m_pQueue->Lock();

    while (!IsListEmpty(&this->m_pQueue->m_QueueHead))
    {
        PLIST_ENTRY  entry;

        entry = RemoveHeadList(&this->m_pQueue->m_QueueHead);

#ifdef _DEBUG
        InterlockedDecrement(&m_pQueue->m_queueSize);
#endif // _DEBUG

        InsertTailList(&flushHead, entry);
    }

    this->m_pQueue->Unlock();

    while (!IsListEmpty(&flushHead))
    {
        PLIST_ENTRY  entry;

        entry = RemoveHeadList(&flushHead);

        QUEUE_EVENT_PRIVATE *Event = CONTAINING_RECORD(entry, QUEUE_EVENT_PRIVATE, Entry);

#if _DEBUG
        flushes++;
#endif 

        free(Event);
    }
}
