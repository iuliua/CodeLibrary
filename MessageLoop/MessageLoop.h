#pragma once

class IMessageListener
{
public:
    virtual ~IMessageListener(){};
    virtual BOOL msg(UINT, WPARAM = 0, LPARAM = 0) = 0;
    virtual BOOL add_timer(DWORD due_time, DWORD period, UINT msg_type) = 0;
};

class IMessageProcessor
{
public:
    virtual BOOL ProcessMessage(MSG &) = 0;
};
class CMessageLoop:public IMessageListener,
                   public IMessageProcessor
{
public:
    enum{
        MSG_QUIT = WM_USER + 1,
        MSG_TIMER,
        MSG_START
    };

private:
    struct TimerStruct
    {
        CMessageLoop *loop;
        UINT msg_type;
        BOOL onetimetimer;
    };
	DWORD m_thread_id;
	HANDLE m_thread_handle;
	HANDLE m_event_thread_create;
    HANDLE m_timer;
    TimerStruct data;
    std::vector<IMessageProcessor*> m_processors;

    static VOID CALLBACK MsgLoop_TimerExpired(PVOID param, BOOL flag);
	static DWORD WINAPI ThreadProcWrapper(LPVOID);
	virtual DWORD ThreadProc();

    

public:
	CMessageLoop();
	~CMessageLoop();
	void Run();
	void Stop();
	bool isRunning();
	void WaitToFinish();
    virtual BOOL add_timer(DWORD due_time,DWORD period,UINT msg_type) override;
	virtual BOOL msg(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0) override;
    void add_processor(IMessageProcessor* proc)
    {
        m_processors.push_back(proc);
    }
};

