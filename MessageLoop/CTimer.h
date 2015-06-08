#pragma once
class ITimeoutHandler
{
public:
    virtual void timeout(UINT)=0;
};

class CTimer
{

private:
    std::map<HANDLE, PVOID> m_memory_by_timer;
    struct TimerStruct :public CMemoryPoolObject < TimerStruct >
    {
        ITimeoutHandler* m_msg_listener;
        UINT m_msg;
    };

private:
    static VOID CALLBACK TimerExpiredWrapper(PVOID param, BOOL flag)
    {
        TimerStruct &data = *((TimerStruct*)param);
        data.m_msg_listener->timeout(data.m_msg);
        TimerStruct::Free(param);
    }

public:
    void remove(HANDLE hTimer) 
    {
        if (hTimer)
        {
            DeleteTimerQueueTimer(NULL, hTimer, INVALID_HANDLE_VALUE);
            TimerStruct::Free(m_memory_by_timer[hTimer]);
        }
    }
    HANDLE add(ITimeoutHandler* msg_listener, UINT timeout, UINT msg=0)
    {
        HANDLE hTimer=NULL;
        TimerStruct *data=TimerStruct::Allocate();
        if (data && msg_listener)
        {
            data->m_msg_listener = msg_listener;
            data->m_msg = msg;
            if (CreateTimerQueueTimer(&hTimer, NULL, (WAITORTIMERCALLBACK)TimerExpiredWrapper, (PVOID)data, timeout * 1000, 0, 0))
                m_memory_by_timer[hTimer] = data;
        }
        return hTimer;
    }
};