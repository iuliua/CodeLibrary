#pragma once
#include <queue>

class CSync
{
public:
    class CriticalSection
    {
        CRITICAL_SECTION m_cs;
    public:
        CriticalSection()
        {
            InitializeCriticalSection(&m_cs);
        }
        ~CriticalSection()
        {
            DeleteCriticalSection(&m_cs);
        }
        operator CRITICAL_SECTION&() { return m_cs; }
    };
    CRITICAL_SECTION &m_cs;
public:
    CSync(CRITICAL_SECTION &cs)
        :m_cs(cs)
    {
        EnterCriticalSection(&m_cs);
    }
    ~CSync()
    {
        LeaveCriticalSection(&m_cs);
    }
};

class CSyncedFlag
{
    volatile long m_flag;
public:
    CSyncedFlag() :m_flag(0)
    {

    }
    inline void set()
    {
        InterlockedExchange(&m_flag, 1);
    }
    inline void reset()
    {
        InterlockedExchange(&m_flag, 0);
    }
    inline BOOL get()
    {
        return InterlockedAdd(&m_flag, 0);
    }
    inline operator BOOL()
    {
        return get();
    }
};

template <class T>
class CSyncedQueue
{
    std::queue<T> m_queue;
    CSync::CriticalSection m_cs;
public:
    void Push(T& val)
    {
        CSync sync(m_cs);
        m_queue.push(val);
    }
    size_t Size()
    {
        CSync sync(m_cs);
        return m_queue.size();
    }
    T& Front()
    {
        CSync sync(m_cs);
        return m_queue.front();
    }
    void Pop()
    {
        CSync sync(m_cs);
        m_queue.pop();
    }
};

class CThreadX567
{
public:
    class IThreadLoopAction
    {
    public:
        virtual BOOL Action() = 0;
        operator IThreadLoopAction*() { return this; }
    };

private:
    HANDLE m_hThread;
    CSyncedFlag m_exiting;
    IThreadLoopAction *m_action;
    int m_sleep_time;
public:
    CThreadX567(IThreadLoopAction* action, int sleep_time=10)
        :m_action(action),
        m_sleep_time(sleep_time)
    {
        if (m_action == nullptr)
            return;
        m_hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, CREATE_SUSPENDED, NULL);
    }
    BOOL Start()
    {
        if (m_hThread == INVALID_HANDLE_VALUE)
            return FALSE;
        if (ResumeThread(m_hThread) == (DWORD)-1)
            return FALSE;
        else
            return TRUE;
    }
    void Stop()
    {
        m_exiting.set();
        WaitForSingleObject(m_hThread, 10000);
    }
    ~CThreadX567()
    {
        Stop();
    }
private:
    static DWORD WINAPI ThreadProcWrapper(LPVOID arg)
    {
        return ((CThreadX567*)arg)->ThreadProc();
    }
    DWORD ThreadProc()
    {
        while (!m_exiting)
        {
            if (!m_action->Action())
                Sleep(m_sleep_time);
        }
        return 0;
    }
};
