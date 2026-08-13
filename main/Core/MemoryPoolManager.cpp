#include "stdafx.h"
#include "Core/MemoryPoolManager.h"

CMemoryPoolManager::CMemoryPoolManager()
{
}

CMemoryPoolManager::~CMemoryPoolManager()
{
	for(CMemoryPoolMap::iterator iter = m_mapPools.begin(); iter != m_mapPools.end(); ++iter)
	{
		delete iter->second;
	}
}

thread_local CMemoryPoolManager *CMemoryPoolManager::m_singleton = NULL;