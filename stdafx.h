#pragma once

#define _CRT_SECURE_NO_WARNINGS

#pragma warning (disable : 4100)

#include <wx/wx.h>
#include <wx/string.h>
#include <wx/valgen.h>

#include "Core/FastRand.h"
#include "Core/MemoryPool.h"

template<typename T> const T &mymin(const T &a, const T &b) { return (a < b) ? a : b; }
template<typename T> const T &mymax(const T &a, const T &b) { return (a > b) ? a : b; }

WX_DECLARE_STRING_HASH_MAP(size_t, wxHashMapStringToSizeT );

#define NOOP ((void)0)