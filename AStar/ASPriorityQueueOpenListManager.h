#pragma once
#include "../Core/PriorityQueue.h"

template<typename TNode, typename TCost>
class CASPriorityQueueOpenListManager
{
public:
	class CASPriorityQueueOpenListManager() {}

	size_t size() const { return m_openList.size(); }
	TNode *pop_front() { return m_openList.pop(); }

	void Insert(TNode *node);
	void ReSort(TNode *node);

protected:
	CPriorityQueue<TNode *, PtrLess<TNode *>> m_openList;
};

template<typename TNode, typename TCost>
void CASPriorityQueueOpenListManager<TNode, TCost>::Insert(TNode *node)
{
	m_openList.add(node);
}

template<typename TNode, typename TCost>
void CASPriorityQueueOpenListManager<TNode, TCost>::ReSort(TNode *node)
{
	for(size_t i=0; i < m_openList.size(); i++)
	{
		if(m_openList[i] == node)
			m_openList.pop(i);
	}

	Insert(node);
}
