#include "stdafx.h"
#include "Application.h"
#include "MDIParent.h"
#include "winsparkle.h"

IMPLEMENT_APP(MyApp)

// Initialise this in OnInit, not statically
bool MyApp::OnInit()
{
	CMemoryPoolManager::InitialiseSingleton();
	CMemPoolNodePoolManager::Get()->InitialiseThread();

	if ( !wxApp::OnInit() )
		return false;

	// Set Priority
#ifndef _DEBUG
	if(!SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS))
	{
		// Just fail silently
		//DWORD dwError = GetLastError();
		//wxMessageBox(wxString::Format("Failed to set process priority (%d)", dwError));
	}
#endif

	// Create the main frame window
	MyFrame *frame = new MyFrame();
	frame->Show(true);
	SetTopWindow(frame);

	// Initialize WinSparkle as soon as the app itself is initialized, right
	// before entering the event loop:
#ifndef _DEBUG
	win_sparkle_set_appcast_url("http://www.superfusion.org/downloads/appcast.xml");
	win_sparkle_init();
#endif

	return true;
}

int MyApp::OnExit()
{
#ifndef _DEBUG
	win_sparkle_cleanup();
#endif
	return 0;
}