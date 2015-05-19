#include "stdafx.h"
#include "MessageLoop.h"


CMessageLoop::CMessageLoop() :m_thread_id(0), 
                              m_thread_handle(INVALID_HANDLE_VALUE), 
							  m_event_thread_create(INVALID_HANDLE_VALUE),
                              m_timer(NULL)
{}

CMessageLoop::~CMessageLoop()
{
    m_processors.clear();
    CloseHandle(m_event_thread_create);
    DeleteTimerQueueTimer(NULL,m_timer,NULL);
    Stop();
    WaitToFinish();
}

void CMessageLoop::Run()
{ 
	if (m_thread_handle == INVALID_HANDLE_VALUE)
	{
		m_event_thread_create = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_thread_handle=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, 0, &m_thread_id);
	}
}

void CMessageLoop::Stop()
{
    this->msg(MSG_QUIT);
}

bool CMessageLoop::isRunning()
{
	if (m_thread_handle != INVALID_HANDLE_VALUE)
	{
		DWORD exit_code = 0;
		BOOL ret = GetExitCodeThread(m_thread_handle, &exit_code);
		return (exit_code == STILL_ACTIVE);
	}
	else
		return false;
}

void CMessageLoop::WaitToFinish()
{
	if (m_thread_handle != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_thread_handle, INFINITE);
		CloseHandle(m_thread_handle);
		m_thread_handle = INVALID_HANDLE_VALUE;
		m_thread_id = 0;
	}
}

DWORD CMessageLoop::ThreadProcWrapper(LPVOID arg)
{
	MSG msg;
	CMessageLoop *msgloop = (CMessageLoop *)arg;
	PeekMessage(&msg, 0, 0, 0, PM_NOREMOVE);
	SetEvent(msgloop->m_event_thread_create);
	return msgloop->ThreadProc();
}

DWORD CMessageLoop::ThreadProc()
{
	MSG msg;
	while (GetMessage(&msg, 0, 0, 0))
	{
		switch (msg.message)
		{
		case MSG_QUIT:
			PostQuitMessage(0);
			break;
		default:
            {
                BOOL res = true;
                for (auto it : m_processors)
                {
                    res = res&&it->ProcessMessage(msg);
                } 
            }
		}
	}
	return 0;
}

BOOL CMessageLoop::msg(UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (m_event_thread_create != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_event_thread_create, INFINITE);
		return PostThreadMessage(m_thread_id, msg, wparam, lparam);
	}
	return FALSE;
}


BOOL CMessageLoop::add_timer(DWORD due_time, DWORD period,UINT msg_type)
{
    if (m_timer == NULL)
    {
        data.loop = this;
        data.msg_type = msg_type;
        data.onetimetimer = (period == 0) ? TRUE : FALSE;
        return CreateTimerQueueTimer(&m_timer, NULL, (WAITORTIMERCALLBACK)MsgLoop_TimerExpired, &data, due_time, period, 0);
    }
    else
        return FALSE;
}
VOID CALLBACK CMessageLoop::MsgLoop_TimerExpired(PVOID param, BOOL flag)
{
    TimerStruct &data = *((TimerStruct*)param);
    data.loop->msg(data.msg_type);
    if (data.onetimetimer)
    {
        data.loop->m_timer = NULL;
        data.onetimetimer = FALSE;
    }
}