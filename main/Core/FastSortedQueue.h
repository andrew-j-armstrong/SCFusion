#pragma once

#include "MemoryPool.h"
#include "MemoryPoolManager.h"

template<typename T>
class CFastSortedQueue : public CMemPoolNode<CFastSortedQueue<T>>
{
public:
	CFastSortedQueue() : m_size(0), m_capacity(0), m_dataStart(0), m_dataEnd(0), m_queueFront(0), m_queueEnd(0) {}
	CFastSortedQueue(const CFastSortedQueue<T> &queue);
	~CFastSortedQueue();

	size_t size() const { return m_size; }
	size_t capacity() const { return m_capacity; }

	void erase(size_t index, size_t count = 1);
	void capacity(size_t capacity);

	void insert(const T &val);
	void push_back(const T &val);
	T &pop_front() { m_size--; return *(m_queueFront++); }

	void truncate(size_t size) { if(size >= m_size) return; m_size = size; }

	const T &operator[](size_t index) const { return m_data[index]; }
	T &operator[](size_t index) { return m_data[index]; }

	const T *front() const { return m_queueFront; }
	const T *end() const { return m_queueEnd - 1; }

	T *front() { return m_queueFront; }
	T *end() { return m_queueEnd - 1; }

protected:
	size_t m_size;
	size_t m_capacity;
	T *m_dataStart;
	T *m_dataEnd;
	T *m_queueFront;
	T *m_queueEnd;
};

template<typename T>
CFastSortedQueue<T>::CFastSortedQueue(const CFastSortedQueue<T> &queue)
: m_size(0), m_capacity(0), m_data(0)
{
	capacity(queue.size());
	T *data = m_dataStart;
	const T *vecData = queue.front();
	for(size_t index = 0; index < queue.size(); index++)
		*(data++) = *(vecData++);
	m_size = queue.size();
}

template<typename T>
CFastSortedQueue<T>::~CFastSortedQueue()
{
	if(m_dataStart)
		m_memoryPoolManager->GetMemoryPool(m_capacity * sizeof(T))->Free(m_dataStart);
}

template<typename T>
void CFastSortedQueue<T>::push_back(const T &val)
{
	capacity(m_size + 1);
	if(m_queueEnd == m_dataEnd)
		memmove(m_dataStart, m_queueStart, m_size * sizeof(T));
	*(m_queueEnd++) = val;
}

template<typename T>
void CFastSortedQueue<T>::insert(const T &val)
{
	capacity(m_size + 1);

	// Binary search to find correct index
	T *start = m_queueFront;
	T *end = m_queueEnd;
	while(start != end)
	{
		T *mid = start + (end - start)/2;
		if(*mid < val)
			start = mid + 1;
		else
			end = mid;
	}

	if(m_queueFront > m_dataStart)
	{
		// Move first half backwards
		memmove(m_queueFront - 1, m_queueFront, (start - m_queueFront) * sizeof(T));
		start--;
		m_queueFront--;
	}
	else
	{
		// Move second half forwards
		memmove(start + 1, start, (m_queueEnd - start) * sizeof(T));
		m_queueEnd++;
	}

	*start = val;
	m_size++;
}

template<typename T>
void CFastSortedQueue<T>::erase(size_t index, size_t count /* = 1 */)
{
	memmove(&m_queueFront[index], &m_queueFront[index + count], (m_size - count - index) * sizeof(T));
	m_size -= count;
	m_queueEnd -= count;
}

template<typename T>
void CFastSortedQueue<T>::capacity(size_t capacity)
{
	if(m_capacity > capacity)
		return;

	size_t newCapacity = m_capacity > 0 ? m_capacity : 128;
	while(newCapacity < capacity + 1)
		newCapacity <<= 1;

	T *newData = (T *)CMemoryPoolManager::Get()->GetMemoryPoolAddAsNeeded(newCapacity * sizeof(T))->Alloc();
	if(m_dataStart)
	{
		memcpy(newData, m_queueFront, m_size * sizeof(T));
		m_memoryPoolManager->GetMemoryPool(m_capacity * sizeof(T))->Free(m_dataStart);
	}

	// Initialise values
	T *init = &newData[m_capacity];
	for(size_t index = m_capacity; index < newCapacity; index++)
		new (init++) T();

	m_capacity = newCapacity;
	m_dataStart = newData;
	m_dataEnd = m_dataStart + m_capacity;
	m_queueFront = m_dataStart;
	m_queueEnd = m_dataStart + m_size;
}
