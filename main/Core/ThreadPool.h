#pragma once

#include "Core/Vector.h"

class CThreadPool
{
public:
	CThreadPool();
	~CThreadPool();

	static CThreadPool *Get();

	HANDLE StartThread(void (*Func)(void *), void *parameter);
	void SetAvailable(HANDLE handle);

protected:
	class CThreadInfo
	{
	public:
		CThreadInfo() : m_threadHandle(0), m_eventHandle(0), m_Func(NULL), m_parameter(NULL), m_dummyHandle(NULL) {}
		~CThreadInfo() {}

		HANDLE m_threadHandle;
		HANDLE m_eventHandle;
		HANDLE m_dummyHandle;
		void (*m_Func)(void *);
		void *m_parameter;
	};

	HANDLE m_semaphore;
	CVector<CThreadInfo *> m_busyThreads;
	CVector<CThreadInfo *> m_availableThreads;

	static DWORD WINAPI ThreadExecute(LPVOID input);
};
