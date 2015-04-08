#include "stdafx.h"
#include "MessageLoop.h"


CMessageLoop::CMessageLoop() :m_thread_id(0), 
                              m_thread_handle(INVALID_HANDLE_VALUE), 
							  m_event_thread_create(INVALID_HANDLE_VALUE)
{}

CMessageLoop::~CMessageLoop()
{
    Stop();
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
	post_message(MSG_QUIT, 0, 0);
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
		CloseHandle(m_event_thread_create);
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
			ProcessMessage(msg);
			break;
		}
	}
	return 0;
}

BOOL CMessageLoop::post_message(UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (m_event_thread_create != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_event_thread_create, INFINITE);
		return PostThreadMessage(m_thread_id, msg, wparam, lparam);
	}
	return FALSE;
}
