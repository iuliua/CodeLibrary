#include "MessageLoop.h"

CMessageLoop::CMessageLoop() :m_thread_id(0), 
                              m_thread_handle(INVALID_HANDLE_VALUE), 
							  m_event_thread_create(INVALID_HANDLE_VALUE),
                              m_time_to_stop(0)
{}

CMessageLoop::~CMessageLoop()
{
    CloseHandle(m_event_thread_create);
    m_event_thread_create = INVALID_HANDLE_VALUE;
    Stop();
    WaitToFinish();
    m_processors.clear();
}

void CMessageLoop::Run()
{ 
	if (m_thread_handle == INVALID_HANDLE_VALUE)
	{
		m_event_thread_create = CreateEvent(NULL, TRUE, FALSE, NULL);
		m_thread_handle=CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, 0, &m_thread_id);
        OnStart();
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
        case MSG_ADD_PROCESSOR:
            m_processors.push_back((IMessageProcessor*)(msg.wParam));
            break;
		default:
            {
                BOOL res = true;
                for (auto it : m_processors)
                {
                    res = res&&it->ProcessMessage(msg.message,(LPVOID)msg.wParam,InterlockedAdd(&m_time_to_stop,0));
                } 
                MessageCleanup((LPVOID)msg.wParam);
            }
		}
	}
	return 0;
}

BOOL CMessageLoop::msg(UINT msg, LPVOID wparam, LPARAM lparam)
{
	if (m_event_thread_create != INVALID_HANDLE_VALUE)
	{
		WaitForSingleObject(m_event_thread_create, INFINITE);
		return PostThreadMessage(m_thread_id, msg,(WPARAM)wparam, lparam);
	}
	return FALSE;
}
