#pragma once

#include <fstream>

template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost> class TNode, typename TTransition, typename TCost, template<typename TNode, typename TState> class TDuplicateManager, template<typename TNode, typename TCost> class TOpenListManager>
class CASEngine
{
public:
	typedef TNode<TConfig, TState, TParent, TTransition, TCost> CASNode;
	typedef TParent<CASNode, TTransition, TCost> CASParent;
	typedef TDuplicateManager<CASNode, TState> CASDuplicateManager;
	typedef TOpenListManager<CASNode, TCost> CASOpenListManager;

	CASEngine(const TConfig &config);
	~CASEngine() { RemoveAllPointer(m_closedNodes); }

	void AddInitialState(const TState &state) { m_initialStates.push_back(state); }

	void Reset();
	void Run();
	void Stop() { m_stop = true; }

	const CASNode *GetFinalNode() const { return m_finalNode; }

protected:
	const TConfig &m_config;
	CASDuplicateManager m_duplicateManager;
	CASOpenListManager m_openNodes;
	CVector<TState> m_initialStates;
	CVector<CASNode *> m_closedNodes;
	bool m_stop;
	CASNode *m_finalNode;
};

template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost> class TNode, typename TTransition, typename TCost, template<typename TNode, typename TState> class TDuplicateManager, template<typename TNode, typename TCost> class TOpenListManager>
CASEngine<TConfig, TState, TParent, TNode, TTransition, TCost, TDuplicateManager, TOpenListManager>::CASEngine(const TConfig &config)
: m_config(config), m_duplicateManager(), m_openNodes(), m_initialStates(), m_closedNodes(), m_stop(false), m_finalNode(NULL)
{
}

template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost> class TNode, typename TTransition, typename TCost, template<typename TNode, typename TState> class TDuplicateManager, template<typename TNode, typename TCost> class TOpenListManager>
void CASEngine<TConfig, TState, TParent, TNode, TTransition, TCost, TDuplicateManager, TOpenListManager>::Reset()
{
	m_stop = false;
	for(size_t i=0; i < m_initialStates.size(); i++)
		m_openNodes.Insert(new CASNode(m_initialStates[i], m_initialStates[i].CalculateExpectedCost(m_config)));
}

template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, template<typename TConfig, typename TState, template<typename TNode, typename TTransition, typename TCost> class TParent, typename TTransition, typename TCost> class TNode, typename TTransition, typename TCost, template<typename TNode, typename TState> class TDuplicateManager, template<typename TNode, typename TCost> class TOpenListManager>
void CASEngine<TConfig, TState, TParent, TNode, TTransition, TCost, TDuplicateManager, TOpenListManager>::Run()
{
	ofstream output("temp.txt");

	CVector<TTransition> transitions;
	while(!m_stop && m_openNodes.size() > 0)
	{
		CASNode *node = m_openNodes.pop_front();
		m_closedNodes.push_back(node);

		const CASNode *parentSearch = node;
		output << m_openNodes.size() << ", " << node->GetCost().GetTime() << ", " << node->GetExpectedCost().GetTime() << ": ";
		while(parentSearch)
		{
			output << tostring(eOutputFormatDetailed, parentSearch->GetParent().GetLastTransition());
			output << ",";

			parentSearch = parentSearch->GetParent().GetParentNode();
		}
		output << endl;

		TState state;
		node->GetState(state, m_config, m_initialStates);

		if(state.IsCompleted(m_config))
		{
			m_finalNode = node;
			break;
		}

		transitions.clear();
		state.GetTransitions(m_config, transitions);

		for(size_t i=0; i < transitions.size(); i++)
		{
			const TTransition &transition = transitions[i];
			TState newState = state;
			TCost newCost = newState.ApplyTransition(m_config, node->GetCost(), transition);
			CASNode *duplicateNode = m_duplicateManager.GetDuplicate(newState);
			if(duplicateNode)
			{
				if(duplicateNode->AddParent(node, newCost, transition))
				{
					m_openNodes.ReSort(duplicateNode);
				}
			}
			else
			{
				CASNode *newNode = new CASNode(newState, newState.CalculateExpectedCost(m_config));
				newNode->AddParent(node, newCost, transition);
				m_openNodes.Insert(newNode);
				m_duplicateManager.Insert(newState, node);
			}
		}
	}
}
