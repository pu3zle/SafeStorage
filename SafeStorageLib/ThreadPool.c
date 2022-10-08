#include "ThreadPool.h"

_Use_decl_annotations_
NTSTATUS
ThreadPoolEnqueue(
    _Inout_ PTHREAD_POOL ThreadPool,
    _In_ LPTHREAD_START_ROUTINE ThreadRoutine,
    _In_ LPTHREAD_START_ROUTINE NotProcessedRoutine,
    _In_opt_ PVOID Context
)
{
    // [1] Allocate a new THREAD_POOL_WORK_ITEM structure to be associated with the work item
    PTHREAD_POOL_WORK_ITEM workItem = (PTHREAD_POOL_WORK_ITEM)_aligned_malloc(sizeof(THREAD_POOL_WORK_ITEM), MEMORY_ALLOCATION_ALIGNMENT);
    if (NULL == workItem)
    {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    RtlSecureZeroMemory(workItem, sizeof(THREAD_POOL_WORK_ITEM));

    workItem->CallbackRoutine = ThreadRoutine;
    workItem->NotProcessedRoutine = NotProcessedRoutine;
    workItem->Context = Context;
    workItem->NextItem = NULL;

    // [2] Insert the work item in ItemsList as the new head of the list and signal the ItemEnqueuedEvent
    workItem->NextItem = ThreadPool->ItemsList;
    ThreadPool->ItemsList = workItem;
    SetEvent(ThreadPool->ItemEnqueuedEvent);

    return STATUS_SUCCESS;
}

_Must_inspect_result_
PTHREAD_POOL_WORK_ITEM
ThreadPoolDequeue(
    _Inout_ PTHREAD_POOL ThreadPool
)
{
    AcquireSRWLockShared(&ThreadPool->ItemsListLock);
    PTHREAD_POOL_WORK_ITEM crtItem = ThreadPool->ItemsList;
    if (NULL != crtItem)
    {
        ThreadPool->ItemsList = crtItem->NextItem;
    }
    ReleaseSRWLockShared(&ThreadPool->ItemsListLock);
    return crtItem;
}

static DWORD WINAPI
ThreadPoolWorkerCallback(
    _In_ LPVOID CallbackParameter
)
{
    PTHREAD_POOL threadPool = (PTHREAD_POOL)CallbackParameter;
    HANDLE events[2] = { threadPool->TerminationEvent, threadPool->ItemEnqueuedEvent };
    DWORD returnValue = 0;

    while (TRUE)
    {
        DWORD result = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
        
        if (result == WAIT_OBJECT_0)            // Termination event is signaled
        {
            returnValue = STATUS_SUCCESS;
            break;
        }
        else if (result == WAIT_OBJECT_0 + 1)   // ItemEnqueuedEvent is signaled
        {
            for (PTHREAD_POOL_WORK_ITEM workItem = ThreadPoolDequeue(threadPool); NULL != workItem; workItem = ThreadPoolDequeue(threadPool))
            {
                if (WAIT_OBJECT_0 == WaitForSingleObject(threadPool->TerminationEvent, 0))
                {
                    // Termination event is signaled. Stop processing work items
                    break;
                }

                // Termination event is not signaled, we can safely continue processing
                workItem->CallbackRoutine(workItem->Context);
                free(workItem);
            }
        }
        else                                    // An error occured. We stop the thread for safety
        {
            returnValue = result;
            break;
        }
    }

    return returnValue;
}


_Use_decl_annotations_
NTSTATUS
ThreadPoolInitialize(
    _Pre_invalid_ _Post_valid_ PTHREAD_POOL ThreadPool
)
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;

    // Ensure that all members of THREAD_POOL structure are zeroed out
    RtlSecureZeroMemory(ThreadPool, sizeof(THREAD_POOL));

    // [1] It properly initializes the singly linked list member to reflect the fact that it is empty
    ThreadPool->ItemsList = NULL;
    InitializeSRWLock(&ThreadPool->ItemsListLock);

    // [2] It creates the "ItemEnqueuedEvent" which will be used to signal the threads when a work item is available
    ThreadPool->ItemEnqueuedEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
    if (NULL == ThreadPool->ItemEnqueuedEvent)
    {
        status = STATUS_INVALID_HANDLE;
        goto CleanUp;
    }

    // [3] It creates the "TerminationEvent" which will be used to signal whether the ThreadPool is Uninitializing
    ThreadPool->TerminationEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
    if (NULL == ThreadPool->TerminationEvent)
    {
        status = STATUS_INVALID_HANDLE;
        goto CleanUp;
    }

    // [4] Creates the threads which automatically are going to wait for one of the events.
    for (int i = 0; i < ARRAYSIZE(ThreadPool->Threads); ++i)
    {
        ThreadPool->Threads[i] = CreateThread(NULL, 0, ThreadPoolWorkerCallback, ThreadPool, 0, NULL);
        if (NULL == ThreadPool->Threads[i])
        {
            status = STATUS_INVALID_HANDLE;
            goto CleanUp;
        }
    }

    // If we reached this point, we successfully initialized the threadpool
    status = STATUS_SUCCESS;

CleanUp:
    if (!NT_SUCCESS(status))
    {
        ThreadPoolUninitialize(ThreadPool);
    }
    return status;
}

_Use_decl_annotations_
VOID
ThreadPoolUninitialize(
    _Pre_valid_ _Post_invalid_ PTHREAD_POOL ThreadPool
)
{
    // [1] Signal threads to stop
    if (NULL != ThreadPool->TerminationEvent)
    {
        SetEvent(ThreadPool->TerminationEvent);
    }
    
    // [2] Wait for each thread to stop
    for (int i = 0; i < ARRAYSIZE(ThreadPool->Threads); ++i)
    {
        if (NULL != ThreadPool->Threads[i])
        {
            WaitForSingleObject(ThreadPool->Threads[i], INFINITE);
            CloseHandle(ThreadPool->Threads[i]);
        }
    }

    // [3] Empty the work items ItemsList and perform proper cleanup of the allocated resources
    for (PTHREAD_POOL_WORK_ITEM workItem = ThreadPoolDequeue(ThreadPool); NULL != workItem; workItem = ThreadPoolDequeue(ThreadPool))
    {
        workItem->NotProcessedRoutine(workItem->Context);
        free(workItem);
    }

    // [4] Destroy the events
    if (NULL != ThreadPool->ItemEnqueuedEvent)
    {
        CloseHandle(ThreadPool->ItemEnqueuedEvent);
    }
    if (NULL != ThreadPool->TerminationEvent)
    {
        CloseHandle(ThreadPool->TerminationEvent);
    }

    // Ensure that all members of THREAD_POOL structure are zeroed out
    RtlSecureZeroMemory(ThreadPool, sizeof(THREAD_POOL));
}