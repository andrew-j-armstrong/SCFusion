#pragma once

template<typename TNode, typename TState>
class CASHashTableBuildStateDuplicateManager
{
public:
	CASHashTableBuildStateDuplicateManager();

	void Insert(TNode *node);
	TNode *GetDuplicate(const TState &state);

protected:
	Hashtable<long, const TState &, TNode *, TNode *> m_hashTable;
};
