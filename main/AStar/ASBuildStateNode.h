#pragma once

template<typename TConfig, typename TState, template <typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost>
class CASBuildStateNode : public CMemPoolNode<CASBuildStateNode<TConfig, TState, TParent, TTransition, TCost>>
{
public:
	typedef TParent<CASBuildStateNode<TConfig, TState, TParent, TTransition, TCost>, TTransition, TCost> CASParent;

	CASBuildStateNode(const TState &state, const TCost &expectedCost) : m_expectedCost(expectedCost) {}

	const CASParent &GetParent() const { return m_parent; }
	const TCost &GetCost() const { return m_parent.GetCost(); }
	const TCost &GetExpectedCost() const { return m_expectedCost; }
	void GetState(TState &state, const TConfig &config, const CVector<TState> &initialStates) const;
	bool AddParent(CASBuildStateNode<TConfig, TState, TParent, TTransition, TCost> *parent, const TCost &cost, const TTransition &transition) { return m_parent.AddParent(parent, cost, transition); }

	bool operator<(const CASBuildStateNode<TConfig, TState, TParent, TTransition, TCost> &node) const { return m_parent.GetCost() + m_expectedCost < node.GetCost() + node.GetExpectedCost(); }

protected:
	CASParent m_parent;
	TCost m_expectedCost;
};

template<typename TConfig, typename TState, template <typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost>
void CASBuildStateNode<TConfig, TState, TParent, TTransition, TCost>::GetState(TState &state, const TConfig &config, const CVector<TState> &initialStates) const
{
	if(!m_parent.GetLastTransition())
		state.Init(config);
	else
	{
		m_parent.GetParentNode()->GetState(state, config, initialStates);
		state.ApplyTransition(config, m_parent.GetParentNode()->GetCost(), m_parent.GetLastTransition());
	}
}
