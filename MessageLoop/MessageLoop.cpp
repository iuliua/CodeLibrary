#include "MessageLoop.h"

CMessageLoop::CMessageLoop() :m_thread_id(0), 
                              m_thread_handle(INVALID_HANDLE_VALUE), 
							  m_event_thread_create(INVALID_HANDLE_VALUE),
                              m_exiting(0)
{
    m_event_thread_create = CreateEvent(NULL, TRUE, FALSE, NULL);
    m_thread_handle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadProcWrapper, this, 0, &m_thread_id);
}

CMessageLoop::~CMessageLoop()
{
    Stop();
    CloseHandle(m_event_thread_create);
    m_event_thread_create = INVALID_HANDLE_VALUE;
    WaitToFinish();
    m_processors.clear();
}

void CMessageLoop::Stop()
{
    this->msg(MSG_QUIT);
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
    SetEvent(this->m_event_thread_create);
    Sleep(1000);
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
                for (auto &consumer : m_processors)
                    if (!consumer->ProcessMessage(msg.message, (LPVOID)msg.wParam, get_exiting()))
                        break;
                MessageCleanup(msg.message,(LPVOID)msg.wParam,msg.lParam);
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
