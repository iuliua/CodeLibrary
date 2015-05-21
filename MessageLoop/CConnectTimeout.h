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
    }

public:
    void remove(HANDLE hTimer) override
    {
        if (hTimer)
            DeleteTimerQueueTimer(NULL, hTimer, INVALID_HANDLE_VALUE);
    }
    HANDLE add(IMessageListener* msg_listener,UINT timeout, UINT msg) override
    {
        HANDLE hTimer;
        TimerStruct *data=TimerStruct::Allocate();
        data->m_msg_listener = msg_listener;
        data->m_msg = msg;
        CreateTimerQueueTimer(&hTimer, NULL, (WAITORTIMERCALLBACK)TimerExpiredWrapper, (PVOID)data, timeout * 1000, 0, 0);
        return hTimer;
    }
};