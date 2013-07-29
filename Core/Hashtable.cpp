#include "stdafx.h"
#include "Core/Hashtable.h"

size_t GetNextPrimeSize(size_t nDesiredSize)
{
	const static size_t arrPrimes[] = {17, 101, 251, 503, 1009, 2003, 4001, 8009, 12007, 16001, 32003, 64007, 128021, 256019, 512009, 1024021, 2048003, 4096013, 8192003, 16384001, 32768011, 65536043, 131072003};

	int nCount = sizeof(arrPrimes)/sizeof(arrPrimes[0]);
	for(int n = 0 ; n < nCount ; ++n)
	{
		if(arrPrimes[n] > nDesiredSize)
			return arrPrimes[n];
	}

	size_t nSize = arrPrimes[nCount-1];

	// Pretty crude but should be alright
	while(nSize < nDesiredSize)
		nSize = 2*nSize + 1;

	return nSize;
}
