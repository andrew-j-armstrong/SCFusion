#pragma once

template<typename TNode, typename TCost>
class CASVectorOpenListManager
{
public:
	CASVectorOpenListManager() {}
	~CASVectorOpenListManager() { RemoveAllPointer(m_openList); }

	size_t size() const { return m_openList.size(); }
	TNode *pop_front() { return m_openList.pop_front(); }

	void Insert(TNode *node);
	void ReSort(TNode *node);

protected:
	CVector<TNode *> m_openList;
};

template<typename TNode, typename TCost>
void CASVectorOpenListManager<TNode, TCost>::Insert(TNode *node)
{
	size_t l = 0, r = m_openList.size(), i;

	while(r - l > 1)
	{
		i = (l + r)/2;
		if(m_openList[i]->GetCost() < node->GetCost())
			l = i + 1;
		else
			r = i;
	}

	m_openList.insert(l, node);
}

template<typename TNode, typename TCost>
void CASVectorOpenListManager<TNode, TCost>::ReSort(TNode *node)
{
	for(size_t i=0; i < m_openList.size(); i++)
	{
		if(m_openList[i] == node)
			m_openList.erase(i);
	}

	Insert(node);
}
