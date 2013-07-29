#pragma once

template <typename T>
struct PtrLess
{
public:
	bool operator()(const T &t1, const T &t2) { return (*t1) < (*t2); }
};

template <typename T>
struct Less
{
public:
	bool operator()(const T &t1, const T &t2) { return t1 < t2; }
};
