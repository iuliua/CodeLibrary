#pragma once
#include <windows.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <common\MT4\Sync.h>
class IMessageProcessor
{
public:
    virtual BOOL ProcessMessage(UINT,LPVOID,BOOL) = 0;
    operator IMessageProcessor*(){ return this; }
};
class IMessageListener
{
public:
    virtual ~IMessageListener(){};
    virtual BOOL msg(UINT, LPVOID= 0, LPARAM=0) = 0;
    virtual void add_processor(IMessageProcessor* proc) = 0;
    virtual void set_exiting() = 0;
    virtual void reset_exiting() = 0;
    operator IMessageListener*(){ return this; }
};

class CMessageLoop:public IMessageListener
{
public:
    enum{
        MSG_QUIT = WM_USER + 1,
        MSG_START
    };
    class IMessageCustom
    {
    public:
        virtual void before_msg(UINT msg, LPVOID &wparam, LPARAM &lparam) = 0;
        virtual void clear_msg(UINT msg, LPVOID wparam, LPARAM lparam) = 0;
        operator CMessageLoop::IMessageCustom*() { return this; }
    };

private:
	DWORD m_thread_id;
    IMessageCustom* m_custom;
    std::atomic<bool> m_exiting;
	HANDLE m_thread_handle;
	HANDLE m_event_thread_create;
    std::vector<IMessageProcessor*> m_processors;
	static DWORD WINAPI ThreadProcWrapper(LPVOID);
	DWORD ThreadProc();

public:
    CMessageLoop(IMessageCustom* custom);
    ~CMessageLoop();
	void Stop();
	void WaitToFinish();
	virtual BOOL msg(UINT msg, LPVOID wparam = 0, LPARAM lparam = 0) override;
    virtual void add_processor(IMessageProcessor* proc) override
    {
        m_processors.push_back(proc);
    }

    virtual void set_exiting() override
    {
        m_exiting = true;
    }

    virtual void reset_exiting() override
    {
        m_exiting = false;
    }
};

