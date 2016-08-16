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

class CThreadObject
{
public:
    class IAction
    {
    public:
        virtual BOOL Action() = 0;
        operator IAction*() { return this; }
    };
private:
    HANDLE m_thread;
    CSyncedFlag m_exiting;
    DWORD m_sleep_time;
    IAction *m_action;
public:
    CThreadObject(DWORD SleepTime, IAction *action)
        :m_action(action),
        m_sleep_time(SleepTime)
    {
    }
    BOOL Init()
    {
        if (m_action == nullptr)
            return FALSE;
        m_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcRequestsWrapper, this, 0, NULL);
        return (m_thread == INVALID_HANDLE_VALUE);
    }
    ~CThreadObject()
    {
        m_exiting.set();
        WaitForSingleObject(m_thread, 30000);
    }
    static DWORD ThreadProcRequestsWrapper(LPVOID data)
    {
        return static_cast<CThreadObject*>(data)->ThreadProc();
    }
    DWORD ThreadProc()
    {
        while (!m_exiting)
        {
            if (m_action == nullptr)
                return 0;
            else
                if (!m_action->Action())
                    Sleep(m_sleep_time);
        }
        return 0;
    }
};
