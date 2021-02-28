#pragma once

template<typename TNode, typename TState>
class CASNoDuplicateManager
{
public:
	CASNoDuplicateManager() {}

	TNode *GetDuplicate(const TState &state) { return NULL; }

	void Insert(const TState &state, TNode *node) {}
};
