#pragma once
#include <queue>
#include <thread>
#include <atomic>

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

class CThreadLoopAction
{
public:
    class IThreadLoopAction
    {
    public:
        virtual void Action() = 0;
        operator IThreadLoopAction*() { return this; }
    };

private:
    std::thread m_thread;
    std::atomic<bool> m_stop;
    std::atomic<bool> m_start;
    IThreadLoopAction *m_action;

public:
    CThreadLoopAction(IThreadLoopAction* action, int sleep_time=10)
        : m_action(action)
    {
        if (action!=nullptr)
            m_thread=std::thread(ThreadProcWrapper, this,sleep_time);
    }
    void Start()
    {
        m_start = true;
    }
    void Stop()
    {
        m_stop = true;
        if (m_thread.joinable())
            m_thread.join();
    }
    ~CThreadLoopAction()
    {
        Stop();
    }
private:
    static void ThreadProcWrapper(CThreadLoopAction* arg,int sleep_time)
    {
        arg->ThreadProc(sleep_time);
    }
    void ThreadProc(int sleep_time)
    {
        do std::this_thread::sleep_for(std::chrono::milliseconds(10)); while (!m_start);
        do
        {
            m_action->Action();
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
        } 
        while (!m_stop);
    }
};
