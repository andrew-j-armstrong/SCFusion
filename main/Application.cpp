#include "stdafx.h"
#include "Application.h"
#include "MDIParent.h"
#ifdef __WXMSW__
#include "winsparkle.h"
#else
#include <unistd.h>	// nice(), the POSIX way of asking politely
#endif

IMPLEMENT_APP(MyApp)

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
	CMemoryPoolManager::InitialiseSingleton();
	CMemPoolNodePoolManager::Get()->InitialiseThread();

	if ( !wxApp::OnInit() )
		return false;

	// Distro wxWidgets builds ship with debug logging on, which lets
	// wxPropertyGrid narrate its focus anxieties to stderr ("lost focus even
	// though it didn't have it", nine times per click). Keep warnings and
	// errors; drop the diary.
	wxLog::SetLogLevel(wxLOG_Warning);

	// Set Priority — the GA will happily eat every core, so step out of the
	// way of whatever game the user is theorycrafting for.
#ifndef _DEBUG
#ifdef __WXMSW__
	if(!SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS))
	{
		// Just fail silently
		//DWORD dwError = GetLastError();
		//wxMessageBox(wxString::Format("Failed to set process priority (%d)", dwError));
	}
#else
	if(nice(5) == -1)
	{
		// Just fail silently (nice() can legitimately return -1; we don't care)
	}
#endif
#endif

	// Create the main frame window
	MyFrame *frame = new MyFrame();
	frame->Show(true);
	SetTopWindow(frame);

	// Initialize WinSparkle as soon as the app itself is initialized, right
	// before entering the event loop:
	// (WinSparkle is, as the name gently hints, Windows-only.)
#if defined(__WXMSW__) && !defined(_DEBUG)
	win_sparkle_set_appcast_url("https://andrew-j-armstrong.github.io/SCFusion/releases/appcast.xml");
	win_sparkle_init();
#endif

	return true;
}

int MyApp::OnExit()
{
#if defined(__WXMSW__) && !defined(_DEBUG)
	win_sparkle_cleanup();
#endif
	return 0;
}