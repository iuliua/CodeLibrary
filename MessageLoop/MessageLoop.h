#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#include <fstream>
class IMessageProcessor
{
public:
    virtual BOOL ProcessMessage(UINT,LPVOID,BOOL) = 0;
};
class IMessageListener
{
public:
    virtual ~IMessageListener(){};
    virtual BOOL msg(UINT, LPVOID= 0, LPARAM=0) = 0;
    virtual BOOL add_processor(IMessageProcessor* proc) = 0;
    virtual void set_exiting() = 0;
    virtual void reset_exiting() = 0;
};

class CMessageLoop:public IMessageListener
{
public:
    enum{
        MSG_QUIT = WM_USER + 1,
        MSG_ADD_PROCESSOR,
        MSG_START
    };

private:
	DWORD m_thread_id;
    volatile long m_exiting;
	HANDLE m_thread_handle;
	HANDLE m_event_thread_create;
    std::vector<IMessageProcessor*> m_processors;
	static DWORD WINAPI ThreadProcWrapper(LPVOID);
	DWORD ThreadProc();
    virtual void MessageCleanup(UINT,LPVOID,LPARAM){};
    inline BOOL get_exiting() { return InterlockedAdd(&m_exiting, 0); }

public:
	CMessageLoop();
	~CMessageLoop();
    void set_exiting() { InterlockedExchange(&m_exiting, 1);}
    void reset_exiting() { InterlockedExchange(&m_exiting, 0); }
	void Stop();
	void WaitToFinish();
	virtual BOOL msg(UINT msg, LPVOID wparam = 0, LPARAM lparam = 0) override;
    virtual BOOL add_processor(IMessageProcessor* proc) override
    {
        return msg(MSG_ADD_PROCESSOR, proc);
    }
};

