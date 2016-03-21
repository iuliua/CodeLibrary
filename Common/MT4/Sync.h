#pragma once

class Sync
{
private:
	CRITICAL_SECTION m_CS;

public:
	Sync()
	{
		ZeroMemory(&m_CS, sizeof(m_CS));
		InitializeCriticalSection(&m_CS);
	}
	~Sync()
	{
		DeleteCriticalSection(&m_CS);
		ZeroMemory(&m_CS, sizeof(m_CS));
	}
	inline void Lock()
	{
		EnterCriticalSection(&m_CS);
	}
	inline void Unlock()
	{
		LeaveCriticalSection(&m_CS);
	}
};
