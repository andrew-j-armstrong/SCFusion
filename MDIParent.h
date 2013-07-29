#ifndef _MDIPARENT_H_
#define _MDIPARENT_H_

#include <map>
#include <wx/hashmap.h>

#include "SC2/SC2Version.h"

// Define a new frame
class MyFrame : public wxMDIParentFrame
{
public:
	MyFrame();
	virtual ~MyFrame();

	static wxMenuBar *CreateMainMenubar();

private:
	void InitToolBar(wxToolBar* toolBar);

	void OnSize(wxSizeEvent& event);
	void OnAbout(wxCommandEvent& event);
	void OnNewWindow(wxCommandEvent& event);
	void OnNewProtoss(wxCommandEvent& event);
	void OnNewTerran(wxCommandEvent& event);
	void OnNewZerg(wxCommandEvent& event);
	void OnOpen(wxCommandEvent& event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);
	void OnFullScreen(wxCommandEvent& event);
	void OnQuit(wxCommandEvent& event);
	void OnCloseAll(wxCommandEvent& event);

	void OnClose(wxCloseEvent& event);

	void OnGameSelected(wxCommandEvent& event);
	void OnVersionSelected(wxCommandEvent& event);

	void OnCheckForUpdates(wxCommandEvent& event);

	DECLARE_EVENT_TABLE()

	void DoGameSelected();
	void DoVersionSelected();

	void LoadVersions();

	typedef std::map<wxString, CSC2Version *> wxVersionOrderedMap;
	WX_DECLARE_HASH_MAP(wxString, wxVersionOrderedMap *, wxStringHash, wxStringEqual, wxGameHashTable);

	wxGameHashTable m_gamesByShortDescription;
	wxGameHashTable m_gamesByLongDescription;

	CSC2Version *m_version;
	wxChoice *m_choiceGame;
	wxChoice *m_choiceVersion;
};

#endif // _MDIPARENT_H_
