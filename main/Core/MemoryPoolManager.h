#pragma once

#include <map>
#include "Core/MemoryPool.h"

using namespace std;

class CMemoryPoolManager
{
public:
	CMemoryPoolManager();
	~CMemoryPoolManager();

	void AddMemoryPool(size_t size)
	{
		size_t initialCapacity = MEMORYPOOL_INITIALMEMORYUSAGE / size;
		if(initialCapacity < MEMORYPOOL_MINIMUMCAPACITY)
			initialCapacity = MEMORYPOOL_MINIMUMCAPACITY;

		AddMemoryPool(size, initialCapacity);
	}
	void AddMemoryPool(size_t size, size_t initialCapacity)
	{
		if(m_mapPools.find(size) == m_mapPools.end())
			m_mapPools[size] = new CMemoryPool(size, initialCapacity);
	}

	CMemoryPool *GetMemoryPool(size_t size) { return m_mapPools[size]; }

	CMemoryPool *GetMemoryPoolAddAsNeeded(size_t size)
	{
		size_t initialCapacity = MEMORYPOOL_INITIALMEMORYUSAGE / size;
		if(initialCapacity < MEMORYPOOL_MINIMUMCAPACITY)
			initialCapacity = MEMORYPOOL_MINIMUMCAPACITY;

		return GetMemoryPoolAddAsNeeded(size, initialCapacity);
	}

	CMemoryPool *GetMemoryPoolAddAsNeeded(size_t size, size_t initialCapacity)
	{
		CMemoryPoolMap::iterator iter = m_mapPools.find(size);
		if(iter == m_mapPools.end())
		{
			CMemoryPool *pool = new CMemoryPool(size, initialCapacity);
			m_mapPools[size] = pool;
			return pool;
		}
		else
			return iter->second;
	}

	static void InitialiseSingleton() { if(NULL == m_singleton) m_singleton = new CMemoryPoolManager(); }
	static CMemoryPoolManager *Get() { return m_singleton; }

protected:
	typedef map<size_t, CMemoryPool *> CMemoryPoolMap;
	CMemoryPoolMap m_mapPools;

	__declspec(thread) static CMemoryPoolManager *m_singleton;
};
