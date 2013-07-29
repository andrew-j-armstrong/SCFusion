#pragma once

#include <windows.h>
#include <iostream>

class CLock
{
public:
	CLock(HANDLE gh)
		: m_ghSemaphore(gh)
	{
		while(WAIT_OBJECT_0 != WaitForSingleObject(m_ghSemaphore, 0));
	}

	~CLock()
	{
		if(!ReleaseSemaphore(m_ghSemaphore, 1, 0))
		{
			DWORD dwErr = GetLastError();
			std::cerr << dwErr << std::endl;
		}
	}

protected:
	HANDLE m_ghSemaphore;
};
