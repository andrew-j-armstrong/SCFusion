#pragma once

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

// One-shot "the work is done" latch. Wait() as often as you like,
// before or after completion; it only ever unlatches once.
class CThreadTask
{
public:
	void Wait()
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		m_cv.wait(lock, [this]{ return m_done; });
	}

	void Signal()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_done = true;
		}
		m_cv.notify_all();
	}

private:
	std::mutex m_mutex;
	std::condition_variable m_cv;
	bool m_done = false;
};

typedef std::shared_ptr<CThreadTask> CThreadTaskPtr;

// Threads are reused rather than respawned because each worker owns
// thread-local memory pools that live as long as the thread does.
class CThreadPool
{
public:
	CThreadPool() {}
	~CThreadPool();

	static CThreadPool *Get();

	CThreadTaskPtr StartThread(void (*Func)(void *), void *parameter);

protected:
	class CWorker;

	static void WorkerLoop(CWorker *worker);
	void SetAvailable(CWorker *worker);

	std::mutex m_mutex;
	std::vector<CWorker *> m_availableWorkers;
	size_t m_busyCount = 0;
};
