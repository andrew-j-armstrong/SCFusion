#pragma once

#include "Core/MemoryPool.h"
#include "Core/MemoryPoolManager.h"

#define VECTOR_MEMORYPOOL_CAPACITY	40

template<typename T>
class CVector : public CMemPoolNode<CVector<T>>
{
public:
	CVector();
	CVector(const CVector<T> &vec);
	~CVector();

	size_t size() const { return m_size; }
	size_t capacity() const { return m_capacity; }

	void append(const CVector &vector) { append(vector.data(), vector.size()); }
	void append(const T *vals, size_t count);
	void push_back(const T &val);
	void push_back_uninitialised(size_t count = 1);
	void insert(size_t index, const T &val);
	void clear() { truncate(0); }
	void erase(size_t index, size_t count = 1);
	void capacity(size_t capacity);
	void push(const T &val) { push_back(val); }
	T pop() { return m_data[--m_size]; }
	T pop_front() { T val = m_data[0]; erase(0, 1); return val; }

	void truncate(size_t size) { if(size >= m_size) return; m_size = size; }

	size_t binarySearch(T value) const;

	const T &operator[](size_t index) const { return m_data[index]; }
	T &operator[](size_t index) { return m_data[index]; }
	const T *data() const { return m_data; }

	bool operator==(const CVector<T> &vector) const;
	bool operator!=(const CVector<T> &vector) const { return !(*this == vector); }
	bool operator <(const CVector<T> &vector) const;

	void operator =(const CVector<T> &vector);

protected:
	size_t m_size;
	size_t m_capacity;
	size_t m_capacityIndex;
	T *m_data;
	CMemoryPool *m_memoryPool;
	__declspec(thread) static CMemoryPool **m_memoryPools;
};

template<typename TData>
void initArray(TData *data, size_t from, size_t to);

template<typename T>
CMemoryPool **CVector<T>::m_memoryPools = 0;

template<typename T>
CVector<T>::CVector()
: m_size(0), m_capacity(0), m_capacityIndex(0), m_data(0), m_memoryPool(0)
{
}

template<typename T>
CVector<T>::CVector(const CVector<T> &vector)
: m_size(0), m_capacity(0), m_capacityIndex(0), m_data(0), m_memoryPool(0)
{
	capacity(vector.size());
	T *data = m_data;
	const T *vecData = vector.data();
	for(size_t index = 0; index < vector.size(); index++)
		*(data++) = *(vecData++);
	m_size = vector.size();
}

template<typename T>
CVector<T>::~CVector()
{
	if(m_data)
		m_memoryPool->Free(m_data);
}

template<typename T>
void CVector<T>::append(const T *vals, size_t count)
{
	capacity(m_size + count);
	T *data = &m_data[m_size];
	for(size_t index = 0; index < count; index++)
		*(data++) = *(vals++);
	m_size += count;
}

template<typename T>
void CVector<T>::push_back(const T &val)
{
	capacity(m_size + 1);
	m_data[m_size++] = val;
}

template<typename T>
void CVector<T>::push_back_uninitialised(size_t count /* = 0 */)
{
	m_size += count;
	capacity(m_size);
}

template<typename T>
void CVector<T>::insert(size_t index, const T &val)
{
	capacity(m_size + 1);
	memmove(&m_data[index + 1], &m_data[index], (m_size - index) * sizeof(T));
	m_data[index] = val;
	m_size++;
}

template<typename T>
void CVector<T>::erase(size_t index, size_t count /* = 1 */)
{
	memmove(&m_data[index], &m_data[index + count], (m_size - count - index) * sizeof(T));
	m_size -= count;
}

template<typename T>
void CVector<T>::capacity(size_t capacity)
{
	if(m_capacity >= capacity)
		return;

	size_t newCapacity = m_capacity > 0 ? m_capacity : 128;
	size_t newCapacityIndex = m_capacityIndex;
	while(newCapacity < capacity + 1)
	{
		newCapacity <<= 1;
		newCapacityIndex++;
	}

	if(!m_memoryPools)
	{
		m_memoryPools = new CMemoryPool *[VECTOR_MEMORYPOOL_CAPACITY];
		memset(m_memoryPools, 0, VECTOR_MEMORYPOOL_CAPACITY * sizeof(CMemoryPool *));
	}

	CMemoryPool *&memoryPool = m_memoryPools[newCapacityIndex];
	if(!memoryPool)
		memoryPool = CMemoryPoolManager::Get()->GetMemoryPoolAddAsNeeded(newCapacity * sizeof(T));

	T *newData = (T *)memoryPool->Alloc();
	if(m_data)
	{
		memcpy(newData, m_data, m_capacity * sizeof(T));
		m_memoryPool->Free(m_data);
	}

	// Initialise values
	initArray(newData, m_capacity, newCapacity);

	m_data = newData;
	m_capacity = newCapacity;
	m_capacityIndex = newCapacityIndex;
	m_memoryPool = memoryPool;
}

template<typename T>
void initArray(T **data, size_t from, size_t to)
{
}

template<typename T>
void initArray(T *data, size_t from, size_t to)
{
	for(size_t i = from; i < to; i++)
		new (data++) T();
}

template<>
void initArray(bool *data, size_t from, size_t to);

template<>
void initArray(short *data, size_t from, size_t to);

template<>
void initArray(int *data, size_t from, size_t to);

template<>
void initArray(size_t *data, size_t from, size_t to);

template<>
void initArray(double *data, size_t from, size_t to);

template<typename T>
bool CVector<T>::operator==(const CVector<T> &vector) const
{
	if(m_size != vector.size())
		return false;

	const T *data = m_data;
	const T *vecData = vector.data();
	for(size_t index=0; index < m_size; index++, data++, vecData++)
	{
		if(*data != *vecData)
			return false;
	}

	return true;
}

template<typename T>
bool CVector<T>::operator<(const CVector<T> &vector) const
{
	const T *data = m_data;
	const T *vecData = vector.data();
	for(size_t index=0; index < m_size && index < vector.size(); index++, data++, vecData++)
	{
		if(*data < *vecData)
			return true;
		else if(*vecData < *data)
			return false;
	}

	return m_size < vector.size();
}

template<typename T>
void CVector<T>::operator=(const CVector<T> &vector)
{
	capacity(vector.size());
	T *data = m_data;
	const T *vecData = vector.data();
	for(size_t index = 0; index < vector.size(); index++)
		*(data++) = *(vecData++);
	m_size = vector.size();
}

template<typename T>
size_t CVector<T>::binarySearch(T value) const
{
	size_t l = 0, r = m_size, n;
	while(r > l)
	{
		n = (l+r)/2;
		if(m_data[n] < value)
			l = n + 1;
		else
			r = n;
	}

	return l;
}

template<typename T>
void RemoveAllPointer(CVector<T *> &vec)
{
	for(size_t i=0; i < vec.size(); i++)
		delete vec[i];
	vec.clear();
}
