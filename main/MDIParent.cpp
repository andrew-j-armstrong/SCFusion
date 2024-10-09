#include "stdafx.h"
#include "MDIParent.h"

#include <wx/wxprec.h>
#include <wx/dir.h>

#ifndef WX_PRECOMP
	#include <wx/wx.h>
	#include <wx/mdi.h>
#endif

#include "MDIChild.h"

#include "bitmaps/SF32.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/print.xpm"
#include "bitmaps/help.xpm"

#include "RaceChoiceDlg.h"
#include "Factorio/FVersion.h"

#include "winsparkle.h"

#define wxID_CHECKFORUPDATES		(wxID_HIGHEST + 1)
#define wxID_TOOLBAR_GAME			(wxID_HIGHEST + 4)
#define wxID_TOOLBAR_VERSION		(wxID_HIGHEST + 5)
#define wxID_REPORT_ISSUE		    (wxID_HIGHEST + 6)

BEGIN_EVENT_TABLE(MyFrame, wxMDIParentFrame)
	EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
	EVT_MENU(wxID_REPORT_ISSUE, MyFrame::OnReportIssue)
	EVT_MENU(wxID_NEW, MyFrame::OnNewWindow)
	EVT_MENU(wxID_OPEN, MyFrame::OnOpen)
	EVT_MENU(wxID_SAVE, MyFrame::OnSave)
	EVT_MENU(wxID_SAVEAS, MyFrame::OnSaveAs)
	EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
	EVT_MENU(wxID_CHECKFORUPDATES, MyFrame::OnCheckForUpdates)

	EVT_MENU(wxID_CLOSE_ALL, MyFrame::OnCloseAll)

	EVT_CHOICE(wxID_TOOLBAR_GAME, MyFrame::OnGameSelected)
	EVT_CHOICE(wxID_TOOLBAR_VERSION, MyFrame::OnVersionSelected)

	EVT_CLOSE(MyFrame::OnClose)
END_EVENT_TABLE()

#include "FEngine.h"

// Define my frame constructor
MyFrame::MyFrame()
: wxMDIParentFrame(NULL, wxID_ANY, "Super Fusion for Factorio", wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxMAXIMIZE)
, m_version(NULL)
{
	wxIcon icon(SF32Icon);
	SetIcon(icon);

	// Make a menubar
#if wxUSE_MENUS
	// Associate the menu bar with the frame
	SetMenuBar(CreateMainMenubar());


	// This shows that the standard window menu may be customized:
	wxMenu * const windowMenu = GetWindowMenu();
	if ( windowMenu )
	{
		// we can change the labels of standard items (which also means we can
		// set up accelerators for them as they're part of the label)
		windowMenu->SetLabel(wxID_MDI_WINDOW_TILE_HORZ,
			"&Tile horizontally\tCtrl-Shift-H");
		windowMenu->SetLabel(wxID_MDI_WINDOW_TILE_VERT,
			"&Tile vertically\tCtrl-Shift-V");

		// we can also change the help string
		windowMenu->SetHelpString(wxID_MDI_WINDOW_CASCADE,
			"Arrange windows in cascade");

		// we can remove some items
		windowMenu->Delete(wxID_MDI_WINDOW_ARRANGE_ICONS);

		// and we can add completely custom commands -- but then we must handle
		// them ourselves, see OnCloseAll()
		windowMenu->AppendSeparator();
		windowMenu->Append(wxID_CLOSE_ALL, "&Close all windows\tCtrl-Shift-C",
			"Close all open windows");

		SetWindowMenu(windowMenu);
	}
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
	CreateStatusBar();
#endif // wxUSE_STATUSBAR

	LoadVersions();

#if wxUSE_TOOLBAR
	CreateToolBar(wxTB_DEFAULT_STYLE);
	InitToolBar(GetToolBar());
#endif // wxUSE_TOOLBAR

#if wxUSE_ACCEL
	// Accelerators
	wxAcceleratorEntry entries[3];
	entries[0].Set(wxACCEL_CTRL, (int) 'N', wxID_NEW);
	entries[1].Set(wxACCEL_CTRL, (int) 'O', wxID_OPEN);
	entries[2].Set(wxACCEL_CTRL, (int) 'X', wxID_EXIT);
	wxAcceleratorTable accel(3, entries);
	SetAcceleratorTable(accel);
#endif // wxUSE_ACCEL

	// connect it only now, after creating m_textWindow
	Connect(wxEVT_SIZE, wxSizeEventHandler(MyFrame::OnSize));

	Maximize();
}

MyFrame::~MyFrame()
{
	// and disconnect it to prevent accessing already deleted m_textWindow in
	// the size event handler if it's called during destruction
	Disconnect(wxEVT_SIZE, wxSizeEventHandler(MyFrame::OnSize));
}

#if wxUSE_MENUS
/* static */
wxMenuBar *MyFrame::CreateMainMenubar()
{
	wxMenu *menuFile = new wxMenu;

	menuFile->Append(wxID_OPEN, "&Open Build Order\tCtrl-O", "Open a build order from a file");
	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT, "E&xit\tAlt-X", "Quit the program");

	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT, "&About\tF1");
	menuHelp->Append(wxID_REPORT_ISSUE, "&Report an Issue");
	menuHelp->Append(wxID_CHECKFORUPDATES, "Check for &Updates");

	wxMenuBar *mbar = new wxMenuBar;
	mbar->Append(menuFile, "&File");
	mbar->Append(menuHelp, "&Help");

	return mbar;
}
#endif // wxUSE_MENUS

void MyFrame::OnClose(wxCloseEvent& event)
{
	unsigned numChildren = MyChild::GetChildrenCount();
	if ( event.CanVeto() )
	{
		if(false)
		{
			wxString msg;
			msg.Printf("%d windows still open, close anyhow?", numChildren);
			if ( wxMessageBox(msg, "Please confirm",
				wxICON_QUESTION | wxYES_NO) != wxYES )
			{
				event.Veto();
	
				return;
			}
		}
	}

	event.Skip();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	Close();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event) )
{
	(void)wxMessageBox("Super Fusion\n"
		"v2.2.2\n"
		"Author: Andrew J. Armstrong\n"
		"Maintainer: Mihai Raducanu"
		, "About Super Fusion");
}

void MyFrame::OnReportIssue(wxCommandEvent& WXUNUSED(event))
{
	(void)wxLaunchDefaultBrowser("https://github.com/andrew-j-armstrong/SCFusion/issues");
}

void MyFrame::OnOpen(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog *openDialog = new wxFileDialog(this, wxT("Choose a file to open"), wxEmptyString, wxEmptyString, wxT("XML files (*.xml)|*.xml|All Files (*.*)|*.*"), wxFD_OPEN);

	wxString fileName;
	if (openDialog->ShowModal() == wxID_OK)
		fileName = openDialog->GetPath();

	openDialog->Destroy();

	if(fileName.IsEmpty())
		return;

	const wxWindowList &children = GetChildren();
	for(size_t i=0; i < children.size(); i++)
	{
		if(fileName == ((MyChild *)children[i])->GetFileName())
		{
			((MyChild *)children[i])->Activate();
			return;
		}
	}

	wxXmlDocument doc;

	if(!doc.Load(fileName))
	{
		wxMessageBox(wxString::Format("Could not open file '%s'", fileName));
		return;
	}

	wxXmlNode *node = doc.GetRoot()->GetChildren();
	if(node->GetName() != wxT("XMLFileVersion"))
	{
		wxMessageBox(wxString::Format("Error in file '%s'", fileName));
		return;
	}

	node = node->GetNext();
	if(node->GetName() != wxT("GameLongDescription"))
	{
		wxMessageBox(wxString::Format("Error in file '%s'", fileName));
		return;
	}

	wxGameHashTable::const_iterator iterGame = m_gamesByLongDescription.find(node->GetNodeContent());
	if(iterGame == m_gamesByLongDescription.end())
	{
		wxMessageBox(wxString::Format("Could not find version info for '%s'", node->GetNodeContent()));
		return;
	}

	const wxVersionOrderedMap *versionMap = iterGame->second;

	node = node->GetNext();
	if(node->GetName() != wxT("GameMinVersion"))
	{
		wxMessageBox(wxString::Format("Error in file '%s'", fileName));
		return;
	}

	const FVersion *version = NULL;
	for(wxVersionOrderedMap::const_iterator iterVersion = versionMap->begin(); iterVersion != versionMap->end(); ++iterVersion)
	{
		if(iterVersion->second->GetMinGameVersion() < node->GetNodeContent())
			version = iterVersion->second;
	}

	if(version == NULL)
		version = versionMap->rbegin()->second;

	node = node->GetNext();
	if(node->GetName() != wxT("Race"))
	{
		wxMessageBox(wxString::Format("Error in file '%s'", fileName));
		return;
	}

	MyChild *subframe;
	/*if(node->GetNodeContent() == wxT("Protoss"))
	{
		const CSC2RaceInfo *protossInfo = version->GetRace(wxT("Protoss"));
		subframe = new MyChild(this, new CSC2Engine(version, eProtoss, protossInfo), Protoss_xpm, fileName);
	}
	else if(node->GetNodeContent() == wxT("Terran"))
	{
		const CSC2RaceInfo *terranInfo = version->GetRace(wxT("Terran"));
		subframe = new MyChild(this, new CSC2Engine(version, eTerran, terranInfo), Terran_xpm, fileName);
	}
	else if(node->GetNodeContent() == wxT("Zerg"))
	{
		const CSC2RaceInfo *zergInfo = version->GetRace(wxT("Zerg"));
		subframe = new MyChild(this, new CSC2Engine(version, eZerg, zergInfo), Zerg_xpm, fileName);
	}
	else
	{
		wxMessageBox(wxString::Format("Unknown race '%s'", node->GetNodeContent()));
		return;
	}*/

	node = node->GetNext();

	while(node)
	{
		if(node->GetName() == wxT("Settings"))
		{
			if(!subframe->LoadSettingsFromXML(node->GetChildren()))
			{
				subframe->Destroy();
				return;
			}
		}
		else if(node->GetName() == wxT("Waypoints"))
		{
			if(!subframe->LoadWaypointsFromXML(node->GetChildren()))
			{
				subframe->Destroy();
				return;
			}
		}
		else if(node->GetName() == wxT("BuildOrder"))
		{
			if(!subframe->LoadBuildOrderFromXML(node->GetChildren()))
			{
				subframe->Destroy();
				return;
			}
		}
		else
		{
			wxMessageBox(wxString::Format("Error in file '%s'", fileName));
			subframe->Destroy();
			return;
		}

		node = node->GetNext();
	}

	subframe->Show(true);
}

void MyFrame::OnSave(wxCommandEvent& WXUNUSED(event))
{
	MyChild *activeChild = (MyChild *)GetActiveChild();

	if(activeChild)
		activeChild->DoSave();
}

void MyFrame::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	MyChild *activeChild = (MyChild *)GetActiveChild();

	if(activeChild)
		activeChild->DoSaveAs();
}

void MyFrame::OnNewWindow(wxCommandEvent& WXUNUSED(event) )
{
	MyChild *subframe;
	subframe = new MyChild(this, new FEngine(m_version), Protoss_xpm);
	subframe->Show(true);
}

void MyFrame::OnFullScreen(wxCommandEvent& event)
{
	ShowFullScreen(event.IsChecked());
}

void MyFrame::OnCloseAll(wxCommandEvent& WXUNUSED(event))
{
	for ( wxWindowList::const_iterator i = GetChildren().begin();
		i != GetChildren().end();
		++i )
	{
		if ( wxDynamicCast(*i, wxMDIChildFrame) )
			(*i)->Close();
	}
}

void MyFrame::OnSize(wxSizeEvent& event)
{
	int w, h;
	GetClientSize(&w, &h);

	GetClientWindow()->SetSize(0, 0, w, h);

	// FIXME: On wxX11, we need the MDI frame to process this
	// event, but on other platforms this should not
	// be done.
#ifdef __WXUNIVERSAL__
	event.Skip();
#else
	wxUnusedVar(event);
#endif
}

void MyFrame::LoadVersions()
{
	wxDir dir("Versions");

	wxArrayString arrFiles;
	wxDir::GetAllFiles(wxT("Versions"), &arrFiles, wxT("*.xml"));

	for(size_t i=0; i < arrFiles.size(); i++)
	{
		FVersion *version = new FVersion();
		if(!version->Load(arrFiles[i]))
		{
			wxMessageBox(wxString::Format(wxT("Error loading version from file \"%s\"."), arrFiles[i]));
			delete version;
			continue;
		}

		wxVersionOrderedMap *versionOrderedMap;
		wxGameHashTable::iterator iter = m_gamesByShortDescription.find(version->GetGameShortDescription());
		if(iter != m_gamesByShortDescription.end())
			versionOrderedMap = iter->second;
		else
		{
			versionOrderedMap = new wxVersionOrderedMap();
			m_gamesByShortDescription[version->GetGameShortDescription()] = versionOrderedMap;
			m_gamesByLongDescription[version->GetGameLongDescription()] = versionOrderedMap;
		}

		(*versionOrderedMap)[version->GetMinGameVersion()] = version;
	}
}

#if wxUSE_TOOLBAR
void MyFrame::InitToolBar(wxToolBar* toolBar)
{
	wxBitmap bitmaps[6];

	bitmaps[0] = wxBitmap( Protoss_xpm );
	bitmaps[1] = wxBitmap( Terran_xpm );
	bitmaps[2] = wxBitmap( Zerg_xpm );
	bitmaps[3] = wxBitmap( open_xpm );
	bitmaps[4] = wxBitmap( save_xpm );
	bitmaps[5] = wxBitmap( help_xpm );

	wxArrayString arrOutputChoices;
	for(wxGameHashTable::const_iterator iterGames = m_gamesByShortDescription.begin(); iterGames != m_gamesByShortDescription.end(); ++iterGames)
		arrOutputChoices.Add(iterGames->first);
	m_choiceGame = new wxChoice(toolBar, wxID_TOOLBAR_GAME, wxDefaultPosition, wxDefaultSize, arrOutputChoices, 0);
	m_choiceGame->SetSelection(0);
	toolBar->AddControl(m_choiceGame, wxT("Game"));

	arrOutputChoices.clear();
	wxVersionOrderedMap *versionHashTable = m_gamesByShortDescription[m_choiceGame->GetStringSelection()];
	if(versionHashTable)
	{
		for(wxVersionOrderedMap::const_iterator iterVersions = versionHashTable->begin(); iterVersions != versionHashTable->end(); ++iterVersions)
			arrOutputChoices.Add(iterVersions->first);
	}
	m_choiceVersion = new wxChoice(toolBar, wxID_TOOLBAR_VERSION, wxDefaultPosition, wxDefaultSize, arrOutputChoices, 0);
	m_choiceVersion->SetSelection(0);
	toolBar->AddControl(m_choiceVersion, wxT("Version"));

	DoVersionSelected();

	toolBar->AddTool(wxID_OPEN, "Open", bitmaps[3], "Open file");
	toolBar->AddTool(wxID_SAVE, "Save", bitmaps[4], "Save file");
	toolBar->AddSeparator();
	toolBar->AddTool(wxID_ABOUT, "About", bitmaps[5], "Help");

	toolBar->Realize();
}
#endif // wxUSE_TOOLBAR

void MyFrame::OnGameSelected(wxCommandEvent& event)
{
	DoGameSelected();
}

void MyFrame::DoGameSelected()
{
	m_choiceVersion->Clear();

	wxGameHashTable::const_iterator iterGame = m_gamesByShortDescription.find(m_choiceGame->GetStringSelection());
	if(m_gamesByShortDescription.end() != iterGame)
	{
		wxVersionOrderedMap *versionHashTable = iterGame->second;
		if(versionHashTable)
		{
			for(wxVersionOrderedMap::const_iterator iterVersions = versionHashTable->begin(); iterVersions != versionHashTable->end(); ++iterVersions)
				m_choiceVersion->Append(iterVersions->first);
		}
	}

	m_choiceVersion->SetSelection(0);
	DoVersionSelected();
}

void MyFrame::OnVersionSelected(wxCommandEvent& event)
{
	DoVersionSelected();
}

void MyFrame::DoVersionSelected()
{
	wxGameHashTable::const_iterator iterGame = m_gamesByShortDescription.find(m_choiceGame->GetStringSelection());
	if(m_gamesByShortDescription.end() != iterGame)
	{
		wxVersionOrderedMap *versionHashTable = iterGame->second;
		if(versionHashTable)
		{
			wxVersionOrderedMap::const_iterator iterVersion = versionHashTable->find(m_choiceVersion->GetStringSelection());
			if(versionHashTable->end() != iterVersion)
				m_version = iterVersion->second;
		}
	}
}

void MyFrame::OnCheckForUpdates(wxCommandEvent& event)
{
	win_sparkle_check_update_with_ui();
}
