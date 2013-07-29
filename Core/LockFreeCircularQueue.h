#pragma once

#include <atomic>

template<typename T>
class CLockFreeCircularQueue
{
public:
	CLockFreeCircularQueue(size_t capacity);
	~CLockFreeCircularQueue();

	bool push_back_unsafe(T val);
	bool pop_front_unsafe(T &popped);

	bool push_back(T val);
	bool pop_front(T &popped);
	bool is_empty() const;

protected:
	T *m_queue;
	size_t m_capacity;
	std::atomic<size_t> m_front;
	std::atomic<size_t> m_backSubmitted;
	std::atomic<size_t> m_backFree;
};

template<typename T>
CLockFreeCircularQueue<T>::CLockFreeCircularQueue(size_t capacity)
	: m_queue(new T[capacity])
	, m_capacity(capacity)
	, m_front(0)
	, m_backSubmitted(0)
	, m_backFree(0)
{
}

template<typename T>
CLockFreeCircularQueue<T>::~CLockFreeCircularQueue()
{
	delete[] m_queue;
}

template<typename T>
bool CLockFreeCircularQueue<T>::push_back_unsafe(T val)
{
	size_t nextFree = (m_backFree + 1) % m_capacity;

	if(nextFree == m_front)
		return false; // Full

	assert(m_backSubmitted == m_backFree);
	m_queue[m_backFree] = val;
	m_backFree = nextFree;
	m_backSubmitted = m_backFree;
	return true;
}

template<typename T>
bool CLockFreeCircularQueue<T>::pop_front_unsafe(T &popped)
{
	if(m_front == m_backSubmitted)
		return false; // Empty

	popped = m_queue[m_front++];
	m_front %= m_capacity;
	return true;
}

template<typename T>
bool CLockFreeCircularQueue<T>::push_back(T val)
{
	size_t currentReadIndex;
	size_t currentWriteIndex;

	do
	{
		currentWriteIndex = m_backFree;
		currentReadIndex  = m_front;
		if (currentWriteIndex - currentReadIndex == m_capacity)
			return false; // Full
	} while (!m_backFree.compare_exchange_weak(currentWriteIndex, currentWriteIndex + 1));

	// We know now that this index is reserved for us. Use it to save the data
	m_queue[currentWriteIndex % m_capacity] = val;

	// update the maximum read index after saving the data. It wouldn't fail if there is only one thread 
	// inserting in the queue. It might fail if there are more than 1 producer threads because this
	// operation has to be done in the same order as the previous CAS

	size_t tempIndex = currentWriteIndex;
	while (!m_backSubmitted.compare_exchange_weak(tempIndex, tempIndex + 1))
	{
		// this is a good place to yield the thread in case there are more
		// software threads than hardware processors and you have more
		// than 1 producer thread
		// have a look at sched_yield (POSIX.1b)
		tempIndex = currentWriteIndex;
		Sleep(0);
	}

	return true;
}

template<typename T>
bool CLockFreeCircularQueue<T>::pop_front(T &popped)
{
	size_t currentMaximumReadIndex;
	size_t currentReadIndex;

	do
	{
		// to ensure thread-safety when there is more than 1 producer thread
		// a second index is defined (m_backSubmitted)
		currentReadIndex        = m_front;
		currentMaximumReadIndex = m_backSubmitted;

		if (currentReadIndex == currentMaximumReadIndex)
			return false; // Empty

		// retrieve the data from the queue
		popped = m_queue[currentReadIndex % m_capacity];

		// try to perfrom now the CAS operation on the read index. If we succeed
		// a_data already contains what m_front pointed to before we 
		// increased it
		if (m_front.compare_exchange_weak(currentReadIndex, currentReadIndex + 1))
			return true;

		// it failed retrieving the element off the queue. Someone else must
		// have read the element stored at countToIndex(currentReadIndex)
		// before we could perform the CAS operation        

	} while(1); // keep looping to try again!

	// Something went wrong. it shouldn't be possible to reach here
	assert(0);

	// Add this return statement to avoid compiler warnings
	return false;
}

template<typename T>
bool CLockFreeCircularQueue<T>::is_empty() const
{
	return countToIndex(m_front) == countToIndex(m_backSubmitted);
}
