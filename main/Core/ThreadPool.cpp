#include "stdafx.h"
#include "Core/ThreadPool.h"
#include "Core/Lock.h"

CThreadPool m_threadPool;

CThreadPool *CThreadPool::Get()
{
	return &m_threadPool;
}

CThreadPool::CThreadPool()
{
	m_semaphore = CreateSemaphore(NULL, 1, 1, NULL);
}

CThreadPool::~CThreadPool()
{
	while(m_busyThreads.size() > 0)
		Sleep(200); // Just have to keep waiting for it

	while(m_availableThreads.size() > 0)
	{
		CThreadInfo *info = NULL;
		{
			CLock lock(m_semaphore);
			if(m_availableThreads.size() > 0)
			{
				info = m_availableThreads.pop();
				info->m_Func = NULL;
				SetEvent(info->m_eventHandle);
			}
		}

		if(info)
		{
			WaitForSingleObject(info->m_threadHandle, INFINITE);
			CloseHandle(info->m_threadHandle);
			delete info;
		}
	}
}

HANDLE CThreadPool::StartThread(void (*Func)(void *), void *parameter)
{
	CLock lock(m_semaphore);

	CThreadInfo *info;
	if(m_availableThreads.size() <= 0)
	{
		info = new CThreadInfo();
		info->m_eventHandle = CreateEvent(NULL, FALSE, FALSE, NULL);
		info->m_threadHandle = CreateThread(NULL, 0, ThreadExecute, info, 0, NULL);
		SetThreadPriority(info->m_threadHandle, THREAD_PRIORITY_BELOW_NORMAL);
	}
	else
	{
		info = m_availableThreads.pop();
	}

	info->m_dummyHandle = CreateSemaphore(NULL, 0, 1, NULL); // Calling code's responsibility to close it

	m_busyThreads.push_back(info);
	info->m_Func = Func;
	info->m_parameter = parameter;
	SetEvent(info->m_eventHandle);

	return info->m_dummyHandle;
}

void CThreadPool::SetAvailable(HANDLE threadHandle)
{
	CLock lock(m_semaphore);

	CThreadInfo *info;

	for(size_t i=0; i < m_busyThreads.size(); i++)
	{
		if(m_busyThreads[i]->m_threadHandle == threadHandle)
		{
			info = m_busyThreads[i];
			m_busyThreads.erase(i);
			break;
		}
	}

	m_availableThreads.push_back(info);
}

DWORD WINAPI CThreadPool::ThreadExecute(LPVOID input)
{
	CMemoryPoolManager::InitialiseSingleton();
	CMemPoolNodePoolManager::Get()->InitialiseThread();

	CThreadInfo *info = (CThreadInfo *)input;

	while(true)
	{
		WaitForSingleObject(info->m_eventHandle, INFINITE);

		if(!info->m_Func)
			break; // ThreadPool being destroyed

		info->m_Func(info->m_parameter);

		ReleaseSemaphore(info->m_dummyHandle, 1, 0);

		CThreadPool::Get()->SetAvailable(info->m_threadHandle);
	}

	return 0;
}
