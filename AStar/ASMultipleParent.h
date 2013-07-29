#pragma once

template<typename TNode, typename TTransition, typename TCost>
class CASMultipleParent : public CMemPoolNode<CASMultipleParent<TNode, TTransition, TCost>>
{
public:
	CASMultipleParent() : m_minimumCost() {}
	CASMultipleParent(TNode *parent, const TCost &cost, const TTransition &transition) : m_minimumCost(cost) { m_parents.push_back(CEntry(parent, transition, m_cost)); }
	~CASMultipleParent() {}

	bool AddParent(TNode *parent, const TCost &cost, const TTransition &transition);
	const TCost &GetCost() const { return m_minimumCost; }

protected:
	struct CEntry
	{
		CEntry(TNode *parent, const TTransition &transition, const TCost &cost) : m_parent(parent), m_transition(transition), m_cost(cost) {}

		TNode *m_parent;
		TTransition m_transition;
		TCost m_cost;
	};

	CVector<CEntry> m_parents;
	TCost m_minimumCost;
};

template<typename TNode, typename TTransition, typename TCost>
bool CASMultipleParent<TNode, TTransition, TCost>::AddParent(TNode *parent, const TCost &cost, const TTransition &transition)
{
	m_parents.push_back(CEntry(parent, transition, m_cost));

	if(cost < m_minimumCost)
	{
		m_minimumCost = cost;
		return true;
	}
	else
		return false; // Cost hasn't changed
}
