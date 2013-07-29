#include "stdafx.h"
#include "Core/MemoryPool.h"
#include "Core/MemoryPoolManager.h"

#include <fstream>

size_t CMemoryPool::m_nUsedMemory(0);

CMemoryPool::CMemoryPool(float fGrowFactor /* = 2.0 */, float fGrowAdditional /* = 0.0 */)
: m_parrFree(NULL)
, m_pEndFree(NULL)
, m_pEndStack(NULL)
, m_nSize(0)
, m_nAllocatedCount(0)
, m_nStackCapacity(0)
, m_nLastBlockSize(0)
, m_nLastStackIncrease(0)
, m_fGrowFactor(fGrowFactor)
, m_fGrowAdditional(fGrowAdditional)
{
}

CMemoryPool::CMemoryPool(size_t nSize, size_t nInitialCapacity /* = MEMORYPOOL_INITIALCAPACITY */, float fGrowFactor /* = 2.0 */, float fGrowAdditional /* = 0.0 */)
: m_parrFree(NULL)
, m_pEndFree(NULL)
, m_pEndStack(NULL)
, m_nSize(0)
, m_nAllocatedCount(0)
, m_nStackCapacity(0)
, m_nLastBlockSize(0)
, m_nLastStackIncrease(0)
, m_fGrowFactor(fGrowFactor)
, m_fGrowAdditional(fGrowAdditional)
{
	Initialise(nSize, nInitialCapacity);
}

void CMemoryPool::Initialise(size_t nSize, size_t nInitialCapacity /* = MEMORYPOOL_INITIALCAPACITY */)
{
	m_nSize = nSize;
	ExtendStack(nInitialCapacity);
	AddBlock(nInitialCapacity);
}

CMemoryPool::~CMemoryPool()
{
	delete[] m_parrFree;
	while(m_vecBlocks.size())
	{
		delete[] m_vecBlocks.back();
		m_vecBlocks.pop_back();
	}

	for(ArrayBlockMap::iterator iter = m_mapArrayBlocks.begin(); iter != m_mapArrayBlocks.end(); ++iter)
		delete[] iter->second;
}

void CMemoryPool::ExtendStack(size_t nCapacity)
{
	void **pArray, **pEnd;

	m_nStackCapacity += nCapacity;
	pArray = new void*[m_nStackCapacity];
	pEnd = pArray - 1;

	if(m_parrFree)
	{
		size_t nExistingCount = (m_pEndFree - m_parrFree) + 1;
		if(nExistingCount)
		{
			memcpy(pArray, m_parrFree, nExistingCount * sizeof(void *));
			pEnd += nExistingCount;
		}

		delete[] m_parrFree;
	}

	m_pEndStack = pArray + m_nStackCapacity - 1;
	m_pEndFree = pEnd;
	m_parrFree = pArray;
	m_nLastStackIncrease = nCapacity;
	m_nUsedMemory += nCapacity * sizeof(void *);
}

void CMemoryPool::AddBlock(size_t nCapacity)
{
	size_t nBlockSize = m_nSize * nCapacity;
	m_nUsedMemory += nBlockSize;
	char *pBlock = new char[nBlockSize];
	m_vecBlocks.push_back(pBlock);
	m_nLastBlockSize = nCapacity;
	m_nAllocatedCount += nCapacity;

	for(; nCapacity > 0; nCapacity--)
	{
		AppendToFreeStack(pBlock);
		pBlock += m_nSize;
	}
}

void *CMemoryPool::AllocArray(size_t size)
{
	ArrayBlockMap::iterator iter = m_mapArrayBlocks.begin();
	while(iter != m_mapArrayBlocks.end())
	{
		if(!iter->second->bUsed && iter->second->size >= size)
			break;
		++iter;
	}

	CArrayBlock *pBlock;
	if(iter != m_mapArrayBlocks.end())
		pBlock = iter->second;
	else
	{
		pBlock = (CArrayBlock *)new char[sizeof(CArrayBlock) + size];
		pBlock->size = size;
		m_mapArrayBlocks[pBlock] = pBlock;
	}

	pBlock->bUsed = true;
	return (void *)(((char *)pBlock) + sizeof(CArrayBlock));
}

void CMemoryPool::FreeArray(void *p)
{
	CArrayBlock *pBlock = (CArrayBlock *)(((char *)p) - sizeof(CArrayBlock));
	m_mapArrayBlocks[pBlock]->bUsed = false;
}

CMemPoolNodePoolManager *CMemPoolNodePoolManager::m_singleton = NULL;

void CMemPoolNodePoolManager::InitialiseThread()
{
	CLock lock(m_semaphore);

	map<size_t, CMemoryPool *> poolMap;

	for(map<CMemoryPool *&(*)(), size_t>::iterator iter = m_poolFunctions.begin(); iter != m_poolFunctions.end(); ++iter)
	{
		if((*iter->first)() != NULL)
			continue;

		map<size_t, CMemoryPool *>::iterator iterPool = poolMap.find(iter->second);
		CMemoryPool *pool;
		if(poolMap.end() == iterPool)
		{
			pool = CMemoryPoolManager::Get()->GetMemoryPoolAddAsNeeded(iter->second);
			m_allPools.push_back(pool);
			poolMap[iter->second] = pool;
		}
		else
			pool = iterPool->second;

		(*iter->first)() = pool;
	}
}

CMemPoolNodePoolManager::~CMemPoolNodePoolManager()
{
	for(vector<CMemoryPool *>::iterator iter = m_allPools.begin(); iter != m_allPools.end(); ++iter)
		delete *iter;
}
