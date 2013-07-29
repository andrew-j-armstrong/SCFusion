#pragma once

template<typename TConfig, typename TState, template <typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost>
class CASStoreStateNode : public CMemPoolNode<CASStoreStateNode<TConfig, TState, TParent, TTransition, TCost> /*, 100000, 1 */>
{
public:
	typedef TParent<CASStoreStateNode<TConfig, TState, TParent, TTransition, TCost>, TTransition, TCost> CASParent;

	CASStoreStateNode(const TState &state, const TCost &expectedCost) : m_state(state), m_expectedCost(expectedCost) {}

	const CASParent &GetParent() const { return m_parent; }
	const TCost &GetCost() const { return m_parent.GetCost(); }
	void GetState(TState &state, const TConfig &config, const CVector<TState> &initialStates) const { state = m_state; }
	bool AddParent(CASStoreStateNode<TConfig, TState, TParent, TTransition, TCost> *parent, const TCost &cost, const TTransition &transition) { return m_parent.AddParent(parent, cost, transition); }

protected:
	CASParent m_parent;
	TCost m_expectedCost;
	TState m_state;
};
