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
};

class CMessageLoop:public IMessageListener
{
public:
    enum{
        MSG_QUIT = WM_USER + 1,
        MSG_ADD_PROCESSOR,
        MSG_START
    };

protected:
	DWORD m_thread_id;
    volatile long m_time_to_stop;
	HANDLE m_thread_handle;
	HANDLE m_event_thread_create;
    std::vector<IMessageProcessor*> m_processors;

	static DWORD WINAPI ThreadProcWrapper(LPVOID);
	virtual DWORD ThreadProc();
    virtual void MessageCleanup(LPVOID,LPARAM){};
protected:
    virtual std::string message_text(UINT msg1)
    {  
        switch (msg1)
        {
        case MSG_QUIT:
            return "MSG_QUIT";
        case MSG_ADD_PROCESSOR:
            return "MSG_ADD_PROCESSOR";
        }
        return "";
    };
public:
	CMessageLoop();
	~CMessageLoop();
    void PrepareToStop() { InterlockedExchange(&m_time_to_stop, 1); }
	void Run();
	void Stop();
	bool isRunning();
	void WaitToFinish();
	virtual BOOL msg(UINT msg, LPVOID wparam = 0, LPARAM lparam = 0) override;
    virtual void OnStart(){};
    virtual BOOL add_processor(IMessageProcessor* proc) override
    {
        return msg(MSG_ADD_PROCESSOR, proc);
    }
};

