#pragma once

#include <iostream>

#include "MemoryPoolManager.h"
#include "MemoryPool.h"

class CFastString
{
public:
	CFastString() : m_capacity(0), m_length(0), m_data(0) { }
	CFastString(const CFastString &str);
	CFastString(const char *str);
	virtual ~CFastString();

	const char &operator[](size_t index) const { return m_data[index]; }
	char &operator[](size_t index) { return m_data[index]; }
	void operator+=(const CFastString &str) { push_back(str); }
	void operator+=(char c) { push_back(c); }
	void operator =(const CFastString &str);
	bool operator==(const CFastString &str) const;
	bool operator <(const CFastString &str) const;

	void push_back(const CFastString &str) { push_back(str, str.length()); }
	void push_back(const CFastString &str, size_t length);
	void push_back(const char *str) { push_back(str, strlen(str)); }
	void push_back(const char *str, size_t length);
	void push_back(char c);

	size_t capacity() const { return m_capacity; }
	void capacity(size_t size);

	size_t length() const { return m_length; }
	void length(size_t size) { capacity(size); m_data[size] = '\0'; m_length = size; }
	const char *data() const { return m_data; }
	const char *mid(size_t index) const { if(index > m_length) return 0; return &m_data[index]; }
	void insert(size_t index, char c);
	void erase(size_t index, size_t count);

	void LargestCommonSubsequence(const CFastString &s1, const CFastString &s2) const;

protected:
	size_t m_capacity;
	size_t m_length;
	char *m_data;
};

ostream& operator<< (ostream& out, const CFastString &str );