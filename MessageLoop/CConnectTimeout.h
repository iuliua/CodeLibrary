//+------------------------------------------------------------------+
//|                                       MetaTrader 5 TFEX Gateway  |
//|                       Copyright 2001-2015, MetaTrader Solutions  |
//|                              http://www.metatradersolutions.com  |
//+------------------------------------------------------------------+
#pragma once
class ITimeoutProvider
{
public:
    virtual HANDLE add(IMessageListener* msg_listener,UINT timeout, UINT message) = 0;
    virtual void remove(HANDLE handle_timer)=0;
};
class CConnectTimeout:public ITimeoutProvider
{

private:
    std::map<HANDLE, PVOID> m_memory_by_timer;
    struct TimerStruct :public CMemoryPoolObject < TimerStruct >
    {
        IMessageListener* m_msg_listener;
        UINT m_msg;
    };

private:
    static VOID CALLBACK TimerExpiredWrapper(PVOID param, BOOL flag)
    {
        TimerStruct &data = *((TimerStruct*)param);
        data.m_msg_listener->msg(data.m_msg);
        TimerStruct::Free(param);
    }

public:
    void remove(HANDLE hTimer) override
    {
        if (hTimer)
        {
            DeleteTimerQueueTimer(NULL, hTimer, INVALID_HANDLE_VALUE);
            TimerStruct::Free(m_memory_by_timer[hTimer]);
        }
    }
    HANDLE add(IMessageListener* msg_listener,UINT timeout, UINT msg) override
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