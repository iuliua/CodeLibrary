#pragma once
class CMessageLoop
{

protected:
	enum{
		MSG_QUIT = WM_USER + 1,
		MSG_START
	};
	BOOL post_message(UINT msg, WPARAM wparam = 0, LPARAM lparam = 0);
	virtual void ProcessMessage(MSG&) = 0;

private:
	DWORD m_thread_id;
	HANDLE m_thread_handle;
	HANDLE m_event_thread_create;
	static DWORD WINAPI ThreadProcWrapper(LPVOID);
	virtual DWORD ThreadProc();

public:
	CMessageLoop();
	~CMessageLoop();
	void Run();
	void Stop();
	bool isRunning();
	void WaitToFinish();
};

