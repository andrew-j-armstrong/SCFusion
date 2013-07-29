#include "stdafx.h"
#include "Core/LockFreeMemoryPool.h"

#include <fstream>

#define INITIAL_QUEUE_CAPACITY 1000
#define INITIAL_BLOCK_CAPACITY 1000

CLockFreeMemoryPool::CLockFreeMemoryPool(size_t size)
	: m_queue(NULL)
	, m_size(size)
	, m_nextBlockIndex(0)
	, m_nextQueueIndex(0)
{
	for(size_t i=0; i < 32; i++)
	{
		m_arrBlocks[i] = NULL;
		m_arrQueues[i] = NULL;
	}
	ExtendStack();
	AddBlock();
}

CLockFreeMemoryPool::~CLockFreeMemoryPool()
{
	for(size_t i=0; i < 32; i++)
	{
		delete[] m_arrBlocks[i];
		delete m_arrQueues[i];
	}
}

void CLockFreeMemoryPool::ExtendStack()
{
	size_t nextQueueIndex = m_nextQueueIndex;
	if(!m_nextQueueIndex.compare_exchange_strong(nextQueueIndex, nextQueueIndex + 1))
	{
		while(m_queue != m_arrQueues[m_nextQueueIndex - 1])
			Sleep(1);

		return;
	}

	CLockFreeCircularQueue<void *> *newQueue = new CLockFreeCircularQueue<void *>(INITIAL_QUEUE_CAPACITY * (1 << nextQueueIndex));
	m_arrQueues[nextQueueIndex] = newQueue;

	CLockFreeCircularQueue<void *> *queue;
	do 
	{
		nextQueueIndex = m_nextQueueIndex - 1;
		queue = m_arrQueues[nextQueueIndex];
	} while (nextQueueIndex != m_nextQueueIndex - 1);

	if(queue)
	{
		m_queue = queue;

		void *p;
		for(size_t i=0; i < nextQueueIndex; i++)
		{
			if(!m_arrQueues[i])
				continue;

			while(m_arrQueues[i]->pop_front(p))
			{
				while(!m_queue->push_back(p))
				{
					ExtendStack();
				}
			}
		}
	}
}

void CLockFreeMemoryPool::AddBlock()
{
	size_t nextBlockIndex = m_nextBlockIndex;
	if(!m_nextBlockIndex.compare_exchange_strong(nextBlockIndex, nextBlockIndex + 1))
	{
		while(0 == m_arrBlocks[m_nextBlockIndex - 1])
			Sleep(1);

		return;
	}

	size_t capacity = INITIAL_BLOCK_CAPACITY * (1 << nextBlockIndex);
	char *block = new char[m_size * capacity];

	char *p = block;
	for(size_t i=0; i < capacity; i++)
	{
		while(!m_queue->push_back(p))
			ExtendStack();

		p += m_size;
	}

	m_arrBlocks[nextBlockIndex] = block;
}
