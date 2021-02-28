#pragma once

#include <atomic>

#include "LockFreeCircularQueue.h"
#include "Hashtable.h"

class CLockFreeMemoryPool
{
public:
	CLockFreeMemoryPool(size_t size);
	~CLockFreeMemoryPool();

	inline void *Alloc();
	inline void Free(void *p);

protected:
	void AddBlock();
	void ExtendStack();

	CLockFreeCircularQueue<void *> *m_queue;

	size_t m_size;
	std::atomic<size_t> m_nextBlockIndex;
	char *m_arrBlocks[32]; // Enough room for allocation up to (2^32) * initial size
	std::atomic<size_t> m_nextQueueIndex;
	CLockFreeCircularQueue<void *> *m_arrQueues[32];
};

void *CLockFreeMemoryPool::Alloc()
{
	void *p;
	while(!m_queue->pop_front(p))
	{
		AddBlock();
	}

	return p;
}

void CLockFreeMemoryPool::Free(void *p)
{
	while(!m_queue->push_back(p))
	{
		ExtendStack();
	}
}

template<size_t size>
class CLockFreeMemoryPoolFixedSize
{
public:
	static CLockFreeMemoryPool m_pool;
};

template<size_t size>
CLockFreeMemoryPool CLockFreeMemoryPoolFixedSize<size>::m_pool(size);

template <typename T>
class CLFMemPoolNode
{
public:
	void *operator new(size_t)
	{
		return m_pool.Alloc();
	}

	void operator delete(void *p)
	{
		if(p)
			m_pool.Free(p);
	}

	void *operator new[](size_t size)
	{
		//return GetMemoryPool(size * sizeof(T))->Alloc();
		return NULL;
	}

	void operator delete[](void *p)
	{
		// Hmm...
	}

	static const CLockFreeMemoryPool &GetMemoryPool() { return m_pool; }

private:
	static CLockFreeMemoryPool &m_pool;
};

template <typename T> CLockFreeMemoryPool &CLFMemPoolNode<T>::m_pool = CLockFreeMemoryPoolFixedSize<sizeof(T)>::m_pool;

