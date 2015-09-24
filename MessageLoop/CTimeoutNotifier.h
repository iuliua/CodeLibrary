#pragma once

class ITimeoutNofifier
{
public:
    virtual HANDLE notify_request(HANDLE target,UINT seconds) = 0;
    virtual void cancel_request(HANDLE request) = 0;
};

class CTimeoutNotifier:public ITimeoutNofifier
{
    IMessageListener *m_msg_listener;
    DWORD m_thread_id;
    HANDLE m_thread_handle;

public:
    CTimeoutNotifier(IMessageListener* msg_listener) :m_msg_listener(msg_listener)
    {
        m_thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, 0, &m_thread_id);
    }
    ~CTimeoutNotifier()
    {
        WaitForSingleObject(m_thread_handle, INFINITE);
        CloseHandle(m_thread_handle);
    }
    virtual HANDLE notify_request(HANDLE target,UINT seconds) override
    {
        return CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, 0, &m_thread_id);
    }

    virtual void cancel_request(HANDLE request) override
    {
    }
private:
    static DWORD ThreadProcWrapper(LPVOID arg)
    {
       return ((CTimeoutNotifier*)arg)->ThreadProc();
    }

    DWORD ThreadProc()
    {
    }
};