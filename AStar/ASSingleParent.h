#pragma once

template<typename TNode, typename TTransition, typename TCost>
class CASSingleParent : public CMemPoolNode<CASSingleParent<TNode, TTransition, TCost>>
{
public:
	CASSingleParent() : m_parent(NULL), m_lastTransition(), m_cost() {}
	CASSingleParent(TNode *parent, const TCost &cost, const TTransition &transition) : m_parent(parent), m_lastTransition(transition), m_cost(cost) {}
	~CASSingleParent() {}

	bool AddParent(TNode *parent, const TCost &cost, const TTransition &transition);
	const TCost &GetCost() const { return m_cost; }
	const TTransition &GetLastTransition() const { return m_lastTransition; }
	const TNode *GetParentNode() const { return m_parent; }

protected:
	TNode *m_parent;
	TTransition m_lastTransition;
	TCost m_cost;
};

template<typename TNode, typename TTransition, typename TCost>
bool CASSingleParent<TNode, TTransition, TCost>::AddParent(TNode *parent, const TCost &cost, const TTransition &transition)
{
	if(m_parent && !(cost < m_cost))
		return false;

	m_parent = parent;
	m_lastTransition = transition;
	m_cost = cost;

	return true;
}
