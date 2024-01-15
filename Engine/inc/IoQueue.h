#pragma once

typedef struct _QUEUE_EVENT {
    LIST_ENTRY     Entry;
    SIZE_T         dataSize;
    UCHAR          data[ANYSIZE_ARRAY];
} QUEUE_EVENT, *PQUEUE_EVENT;

class CIoQueue
{
private:

    class CQueueInternal
    {
    public:
        LONG                 m_CacheSize;
        LIST_ENTRY           m_CachedEntry;

        LIST_ENTRY           m_QueueHead;
        CRITICAL_SECTION     m_lockQueue;
        HANDLE               m_queueReadyEvent;
        volatile BOOL        m_waited;
        volatile LONG        m_DataSize;

#ifdef _DEBUG
        LONG      m_queueSize;
#endif // _DEBUG

        CQueueInternal() throw()
        {
#ifdef _DEBUG
            m_queueSize = 0;
#endif // _DEBUG
            m_waited = 0;
            m_CacheSize = 0;
            m_DataSize = 0;
        }

        BOOL Init();
        VOID Finalize();

        VOID Lock() {

            ::EnterCriticalSection(&m_lockQueue);
        }

        VOID Unlock() {

            ::LeaveCriticalSection(&m_lockQueue);

        }
    };

    UCHAR                   m_QueueStorage[sizeof(CQueueInternal)];
    CQueueInternal          *m_pQueue;


    typedef struct _QUEUE_EVENT_PRIVATE {
        LIST_ENTRY     Entry;
        SIZE_T         realSize;
        QUEUE_EVENT    publicEvent;
    } QUEUE_EVENT_PRIVATE, *PQUEUE_EVENT_PRIVATE;

public:
    CIoQueue() throw()
    {

        m_pQueue = new(m_QueueStorage) CQueueInternal();
    }

    ~CIoQueue()
    {
    }

    //
    // copy queue
    //
    CIoQueue(CIoQueue &other)
    {
        memset(&this->m_QueueStorage, 0, sizeof(CQueueInternal));
        this->m_pQueue = other.m_pQueue;
    }

    BOOL Initialize();
    VOID Finalize();

    VOID Lock() {
        //assert(m_pQueue != NULL);

        m_pQueue->Lock();
    }

    VOID Unlock() {
        //assert(m_pQueue != NULL);

        m_pQueue->Unlock();

    }

    //
    // allocate new event
    //
    PQUEUE_EVENT AllocateEvent(__in SIZE_T size);
    //
    // add event to queue
    // 
    VOID AddEvent(__in PQUEUE_EVENT  entry);

    SIZE_T  Size() {

        return (SIZE_T)this->m_pQueue->m_DataSize;
    }

    //
    // get event from queue
    // 
    PQUEUE_EVENT GetEvent();
    //
    // wait for queue income event
    // 
    DWORD WaitForEvent(__in_opt HANDLE  hExitEvent = 0 OPTIONAL, __in_opt DWORD timeout = INFINITE OPTIONAL);

    //
    // free event to queue
    //
    VOID  ReleaseEvent(__in PQUEUE_EVENT event);

    VOID  Flush();
};

