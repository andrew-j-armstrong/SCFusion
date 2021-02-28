#pragma once

#include <vector>
#include <map>

#include "Lock.h"

using namespace std;

#define MEMORYPOOL_INITIALMEMORYUSAGE	10000
#define MEMORYPOOL_MINIMUMCAPACITY		10	

class CMemoryPool
{
public:
	CMemoryPool(float fGrowFactor = 2.0, float fGrowAdditional = 0.0);
	CMemoryPool(size_t nSize, size_t nInitialCapacity, float fGrowFactor = 2.0, float fGrowAdditional = 0.0);
	~CMemoryPool();

	void Initialise(size_t nSize, size_t nInitialCapacity);

	inline void *Alloc();
	inline void Free(void *p);

	void *AllocArray(size_t size);
	void FreeArray(void *p);

	size_t GetSize() const { return m_nSize; }
	size_t GetStackCapacity() const { return m_nStackCapacity; }
	size_t GetAllocated() const { return m_nAllocatedCount; }
	size_t GetCurrentlyUsed() const { return m_nAllocatedCount - (m_pEndFree - m_parrFree); }

	static size_t GetUsedMemory() { return m_nUsedMemory; }

protected:
	class CArrayBlock
	{
	public:
		struct Less
		{
			bool operator()(const CArrayBlock* p1, const CArrayBlock* p2) const
			{
				return p1->size < p2->size;
			}
		};
		size_t size;
		bool bUsed;
	};

	void AddBlock(size_t nCapacity);
	void ExtendStack(size_t nCapacity);

	inline void AppendToFreeStack(void *p);

	size_t m_nSize;
	size_t m_nAllocatedCount;
	size_t m_nStackCapacity;
	size_t m_nLastBlockSize;
	size_t m_nLastStackIncrease;
	float m_fGrowFactor;
	float m_fGrowAdditional;

	void **m_parrFree;
	void **m_pEndFree;
	void **m_pEndStack;

	vector<char *> m_vecBlocks;
	typedef map<CArrayBlock *, CArrayBlock *, CArrayBlock::Less> ArrayBlockMap;
	ArrayBlockMap m_mapArrayBlocks;

	static size_t m_nUsedMemory;
};

void *CMemoryPool::Alloc()
{
	if(m_parrFree > m_pEndFree)
		AddBlock((size_t)(((float)m_nLastBlockSize) * m_fGrowFactor + m_fGrowAdditional));

	return (*(m_pEndFree--));
}

void CMemoryPool::Free(void *p)
{
	AppendToFreeStack(p);
}

void CMemoryPool::AppendToFreeStack(void *p)
{
	if(m_pEndFree == m_pEndStack)
		ExtendStack((size_t)(((float)m_nLastStackIncrease) * m_fGrowFactor + m_fGrowAdditional));

	(*(++m_pEndFree)) = p;
}

class CMemPoolNodePoolManager
{
public:
	CMemPoolNodePoolManager() { m_semaphore = CreateSemaphore(0, 1, 1, 0); }
	~CMemPoolNodePoolManager();

	static CMemPoolNodePoolManager *Get() { if(!m_singleton) m_singleton = new CMemPoolNodePoolManager(); return m_singleton; }

	void InitialiseThread();
	void AddPoolFunction(CMemoryPool *&(*func)(), size_t size) { CLock lock(m_semaphore); m_poolFunctions[func] = size; }

	template<typename T>
	class CAutoAddFunction
	{
	public:
		CAutoAddFunction()
		{
			CMemPoolNodePoolManager::Get()->AddPoolFunction(CMemPoolNode<T>::GetMemoryPool, sizeof(T));
		}
	};

protected:
	static CMemPoolNodePoolManager *m_singleton;

	HANDLE m_semaphore;
	map<CMemoryPool *&(*)(), size_t> m_poolFunctions;
	vector<CMemoryPool *> m_allPools;
};

template <typename T>
class CMemPoolNode
{
public:
	CMemPoolNode()
	{
		HasAutoAdd();
	}
	
	void *operator new(size_t)
	{
		return m_pool->Alloc();
	}

	void operator delete(void *p)
	{
		if(p)
			m_pool->Free(p);
	}

	void *operator new[](size_t size)
	{
		return m_pool->AllocArray(size);
	}

	void operator delete[](void *p)
	{
		if(p)
			m_pool->FreeArray(p);
	}

	static bool HasMemoryPool() { return m_pool == NULL; }
	static CMemoryPool *&GetMemoryPool() { return m_pool; }
	static bool SetMemoryPool(CMemoryPool *pool) { if(m_pool) return false; m_pool = pool; return true; }
	static bool HasAutoAdd() { return m_autoAddFunction != NULL; }

private:
	__declspec(thread) static CMemoryPool *m_pool;
	static CMemPoolNodePoolManager::CAutoAddFunction<T> *m_autoAddFunction;
};

template <typename T> CMemoryPool *CMemPoolNode<T>::m_pool = NULL;
template <typename T> CMemPoolNodePoolManager::CAutoAddFunction<T> *CMemPoolNode<T>::m_autoAddFunction = new CMemPoolNodePoolManager::CAutoAddFunction<T>();
