#pragma once
#include <map>
class ITimeoutHandler
{
public:
    virtual void timeout(std::string)=0;
};
class CMeasureTime
{
    LARGE_INTEGER start1, finish, freq;
    double last_meas;
public:
    CMeasureTime() { QueryPerformanceFrequency(&freq); }
    void start()
    {
        QueryPerformanceCounter(&start1);
    }
    void stop()
    {
        QueryPerformanceCounter(&finish);
        last_meas = (finish.QuadPart - start1.QuadPart) / (double)freq.QuadPart;
    }
    inline double get()
    {
        return last_meas;
    }

};
class CTimer
{

private:
    std::map<HANDLE, PVOID> m_memory_by_timer;
    struct TimerStruct
    {
        TimerStruct(ITimeoutHandler* t, std::string m) :m_msg_listener(t), m_msg(m){};
        ITimeoutHandler* m_msg_listener;
        std::string m_msg;
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
    HANDLE add(ITimeoutHandler* msg_listener, UINT timeout, std::string msg="")
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