#pragma once

#include "Core/Vector.h"
#include "Core/Compare.h"

template<typename T, typename Compare = Less<T>>
class CPriorityQueue
{
public:
	CPriorityQueue() {}
	CPriorityQueue(const CPriorityQueue<T, Compare> &queue) { m_vecQueue = queue.getQueue(); }
	~CPriorityQueue() {}

	inline size_t size() const { return m_vecQueue.size(); }

	inline void add(const T &val);
	inline void clear() { m_vecQueue.truncate(0); }
	inline T pop(int index = 0);

	const CVector<T> &getQueue() const { return m_vecQueue; }

	inline const T &operator[](size_t index) const { return m_vecQueue[index]; }
	inline T &operator[](size_t index) { return m_vecQueue[index]; }

	inline bool operator==(const CPriorityQueue<T, Compare> &queue) const { return m_vecQueue == queue.getQueue(); }
	inline bool operator!=(const CPriorityQueue<T, Compare> &queue) const { return m_vecQueue != queue.getQueue(); }

	inline void operator =(const CPriorityQueue<T, Compare> &queue) { m_vecQueue = queue.getQueue(); }

protected:
	CVector<T> m_vecQueue;
};

template<typename T, typename Compare>
void CPriorityQueue<T, Compare>::add(const T &val)
{
	m_vecQueue.push_back_uninitialised(1);

	// Sift up
	Compare comp;
	size_t index = m_vecQueue.size() - 1;
	while(index > 0)
	{
		size_t parentIndex = (index - 1) / 2;

		if(comp(m_vecQueue[parentIndex], val))
			break;

		m_vecQueue[index] = m_vecQueue[parentIndex];

		index = parentIndex;
	}
	m_vecQueue[index] = val;
}

template<typename T, typename Compare>
T CPriorityQueue<T, Compare>::pop(int index /* = 0 */)
{
	if(m_vecQueue.size() > 1)
	{
		Compare comp;

		T popped = m_vecQueue[index];
		T val = m_vecQueue.pop();

		if(index > 0 && comp(val, popped))
		{
			// Sift up
			while(index > 0)
			{
				size_t parentIndex = (index - 1) / 2;

				if(comp(m_vecQueue[parentIndex], val))
					break;

				m_vecQueue[index] = m_vecQueue[parentIndex];

				index = parentIndex;
			}
			m_vecQueue[index] = val;
		}
		else
		{
			// Sift down
			size_t parentIndex = index;
			size_t childIndex = 2 * index + 1;
			while(childIndex < m_vecQueue.size())
			{
				if(childIndex + 1 < m_vecQueue.size() && comp(m_vecQueue[childIndex + 1], m_vecQueue[childIndex]))
					childIndex++;

				if(!comp(m_vecQueue[childIndex], val))
					break;

				m_vecQueue[parentIndex] = m_vecQueue[childIndex];

				parentIndex = childIndex;
				childIndex = (childIndex * 2) + 1;
			}

			m_vecQueue[parentIndex] = val;
		}

		return popped;
	}
	else
		return m_vecQueue.pop();
}
