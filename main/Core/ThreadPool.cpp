#include "stdafx.h"
#include "Core/ThreadPool.h"
#include "Core/Lock.h"
#include "Core/MemoryPoolManager.h"

#include <chrono>

CThreadPool m_threadPool;

CThreadPool *CThreadPool::Get()
{
	return &m_threadPool;
}

// A worker: one thread, its own little mailbox, and dreams of build orders.
class CThreadPool::CWorker
{
public:
	std::thread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	void (*m_Func)(void *) = nullptr;
	void *m_parameter = nullptr;
	CThreadTaskPtr m_task;
	bool m_hasWork = false;
	bool m_exit = false;
};

CThreadPool::~CThreadPool()
{
	// Politely wait for anyone still crunching numbers
	for(;;)
	{
		{
			CLock lock(m_mutex);
			if(m_busyCount == 0)
				break;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

	std::vector<CWorker *> workers;
	{
		CLock lock(m_mutex);
		workers.swap(m_availableWorkers);
	}

	for(CWorker *worker : workers)
	{
		{
			CLock lock(worker->m_mutex);
			worker->m_exit = true;
		}
		worker->m_cv.notify_one();
		worker->m_thread.join();
		delete worker;
	}
}

CThreadTaskPtr CThreadPool::StartThread(void (*Func)(void *), void *parameter)
{
	CThreadTaskPtr task = std::make_shared<CThreadTask>();
	CWorker *worker;

	{
		CLock lock(m_mutex);

		if(m_availableWorkers.empty())
		{
			worker = new CWorker();
			worker->m_thread = std::thread(WorkerLoop, worker);
		}
		else
		{
			worker = m_availableWorkers.back();
			m_availableWorkers.pop_back();
		}

		m_busyCount++;
	}

	{
		CLock lock(worker->m_mutex);
		worker->m_Func = Func;
		worker->m_parameter = parameter;
		worker->m_task = task;
		worker->m_hasWork = true;
	}
	worker->m_cv.notify_one();

	return task;
}

void CThreadPool::SetAvailable(CWorker *worker)
{
	CLock lock(m_mutex);
	m_availableWorkers.push_back(worker);
	m_busyCount--;
}

void CThreadPool::WorkerLoop(CWorker *worker)
{
	// Every thread gets its own private memory-pool empire.
	CMemoryPoolManager::InitialiseSingleton();
	CMemPoolNodePoolManager::Get()->InitialiseThread();

	for(;;)
	{
		void (*Func)(void *);
		void *parameter;
		CThreadTaskPtr task;

		{
			std::unique_lock<std::mutex> lock(worker->m_mutex);
			worker->m_cv.wait(lock, [worker]{ return worker->m_hasWork || worker->m_exit; });

			if(worker->m_exit)
				return;

			worker->m_hasWork = false;
			Func = worker->m_Func;
			parameter = worker->m_parameter;
			task = std::move(worker->m_task);
		}

		Func(parameter);

		task->Signal();

		CThreadPool::Get()->SetAvailable(worker);
	}
}
