#pragma once

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
