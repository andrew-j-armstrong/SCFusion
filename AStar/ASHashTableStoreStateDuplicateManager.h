#pragma once

#include "../Core/Hashtable.h"

template<typename TNode, typename TState>
class CASHashTableStoreStateDuplicateManager
{
public:
	CASHashTableStoreStateDuplicateManager() {}

	TNode *GetDuplicate(const TState &state);

	void Insert(const TState &state, TNode *node);

protected:
	CHashtable<TState, const TState &, TNode *, TNode *> m_hashTable;
};

template<typename TNode, typename TState>
TNode *CASHashTableStoreStateDuplicateManager<TNode, TState>::GetDuplicate(const TState &state)
{
	TNode *pNode = NULL;;
	m_hashTable.Lookup(state, pNode);
	return pNode;
}

template<typename TNode, typename TState>
void CASHashTableStoreStateDuplicateManager<TNode, TState>::Insert(const TState &state, TNode *node)
{
	m_hashTable.SetAt(state, node);
}
