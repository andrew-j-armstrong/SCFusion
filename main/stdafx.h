#pragma once

#define _CRT_SECURE_NO_WARNINGS

#ifdef _MSC_VER
#pragma warning (disable : 4100)
#endif

#ifdef SCF_HEADLESS
// Headless build: wxBase only. No windows — the program, not the OS.
#include <wx/string.h>
#include <wx/arrstr.h>
#include <wx/hashmap.h>
#include <wx/tokenzr.h>
#include <wx/log.h>

#include <cstdio>

// Stand-in for wxMessageBox: with nobody to click OK, errors go to stderr.
inline int wxMessageBox(const wxString &message, const wxString &caption = wxT("Message"), long style = 0)
{
	fprintf(stderr, "[%s] %s\n", (const char *)caption.utf8_str(), (const char *)message.utf8_str());
	return 0;
}
#else
#include <wx/wx.h>
#include <wx/string.h>
#include <wx/valgen.h>
#endif

#include <cfloat> // DBL_MAX and friends (MSVC hands this out for free; GCC makes you ask)

#include "Core/FastRand.h"
#include "Core/MemoryPool.h"

template<typename T> const T &mymin(const T &a, const T &b) { return (a < b) ? a : b; }
template<typename T> const T &mymax(const T &a, const T &b) { return (a > b) ? a : b; }

WX_DECLARE_STRING_HASH_MAP(size_t, wxHashMapStringToSizeT );

#define NOOP ((void)0)