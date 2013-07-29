#include "stdafx.h"
#include "FastString.h"

CFastString::CFastString(const CFastString &str)
: m_capacity(0), m_length(0), m_data(0)
{
	push_back(str);
}

CFastString::CFastString(const char *str)
: m_capacity(0), m_length(0), m_data(0)
{
	push_back(str);
}

CFastString::~CFastString()
{
	if(m_data)
		CMemoryPoolManager::Get()->GetMemoryPool(m_capacity)->Free(m_data);
}

void CFastString::capacity(size_t size)
{
	if(m_capacity >= size + 1)
		return;

	size_t newCapacity = m_capacity > 0 ? m_capacity : 16;
	while(newCapacity < size + 1)
		newCapacity <<= 1;

	char *newData = (char *)CMemoryPoolManager::Get()->GetMemoryPoolAddAsNeeded(newCapacity)->Alloc();
	if(m_data)
	{
		memcpy(newData, m_data, m_capacity);
		CMemoryPoolManager::Get()->GetMemoryPool(m_capacity)->Free(m_data);
	}
	else
		newData[0] = '\0';
	m_data = newData;
	m_capacity = newCapacity;
}

void CFastString::insert(size_t index, char c)
{
	capacity(m_length + 1);
	memmove(&m_data[index+1], &m_data[index], m_length + 1 - index);
	m_data[index] = c;
	m_length++;
}

void CFastString::erase(size_t index, size_t count)
{
	memmove(&m_data[index], &m_data[index+count], m_length + 1 - index - count);
	m_length -= count;
}


void CFastString::push_back(const CFastString &str, size_t length /* = str.length() */)
{
	if(str.length() < length)
		length = str.length();

	push_back(str.data(), length);
}

void CFastString::push_back(const char *str, size_t length /* = strlen(str) */)
{
	if(length <= 0)
		return;

	capacity(m_length + length);
	memmove(&m_data[m_length], str, length + 1);
	m_length += length;
}

bool CFastString::operator==(const CFastString &str) const
{
	if(str.length() != m_length)
		return false;
	else if(0 == m_length)
		return true;
	else
		return 0 == strcmp(m_data, str.data());
}

bool CFastString::operator <(const CFastString &str) const
{
	if(0 == m_length)
		return 0 != str.length();
	else if(0 == str.length())
		return false;
	else
		return strcmp(m_data, str.data()) < 0;
}

void CFastString::operator =(const CFastString &str)
{
	capacity(str.length());
	m_length = str.length();
	memcpy(m_data, str.data(), m_length + 1);
}

void CFastString::push_back(char c)
{
	capacity(m_length + 1);
	m_data[m_length] = c;
	m_data[m_length + 1] = '\0';
	m_length++;
}

ostream& operator<< (ostream& out, const CFastString &str )
{
	if(str.length() > 0)
		out << str.data();
	
	return out;
}
