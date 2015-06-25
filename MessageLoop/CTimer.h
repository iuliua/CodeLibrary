#pragma once
#include <map>
class ITimeoutHandler
{
public:
    virtual void timeout(UINT)=0;
};

class CTimer
{

private:
    std::map<HANDLE, PVOID> m_memory_by_timer;
    struct TimerStruct
    {
        TimerStruct(ITimeoutHandler* t, UINT m) :m_msg_listener(t), m_msg(m){};
        ITimeoutHandler* m_msg_listener;
        UINT m_msg;
    };

private:
    static VOID CALLBACK TimerExpiredWrapper(PVOID param, BOOL flag)
    {
        TimerStruct &data = *((TimerStruct*)param);
        data.m_msg_listener->timeout(data.m_msg);
        delete (TimerStruct*)param;
    }

public:
    void remove(HANDLE hTimer) 
    {
        if (hTimer)
        {
            DeleteTimerQueueTimer(NULL, hTimer, INVALID_HANDLE_VALUE);
            delete (TimerStruct*)(m_memory_by_timer[hTimer]);
        }
    }
    HANDLE add(ITimeoutHandler* msg_listener, UINT timeout, UINT msg=0)
    {
        if (msg_listener)
        {
            HANDLE hTimer = NULL;
            TimerStruct *data = new (std::nothrow)TimerStruct(msg_listener, msg);
            if (data)
            {
                if (CreateTimerQueueTimer(&hTimer, NULL, (WAITORTIMERCALLBACK)TimerExpiredWrapper, (PVOID)data, timeout * 1000, 0, 0))
                    m_memory_by_timer[hTimer] = data;
            }
            return hTimer;
        }
        else
            return INVALID_HANDLE_VALUE;
    }
};