#ifndef _MDICHILD_H_
#define _MDICHILD_H_

#include <wx/notebook.h>
#include <wx/listctrl.h>
#include <wx/timer.h>
#include <wx/utils.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

#include "GA/GAEngine.h"
#include "SC2Engine.h"
#include "TimeValidator.h"

//template<typename TTarget, typename TState, typename TCommand, typename TEvent>
class MyChild : public wxMDIChildFrame
{
public:
	MyChild(wxMDIParentFrame *parent, CSC2Engine *engine, const char * const *xpmIcon, wxString fileName = wxEmptyString);
	virtual ~MyChild();

	bool LoadSettingsFromXML(wxXmlNode *node);
	bool LoadWaypointsFromXML(wxXmlNode *node);
	bool LoadBuildOrderFromXML(wxXmlNode *node);

	static unsigned GetChildrenCount() { return ms_numChildren; }

	bool DoSave();
	bool DoSaveAs();

	const wxString &GetFileName() const { return m_fileName; }

private:
	void InitToolBar(wxToolBar* toolBar);

	void OnAction(wxCommandEvent& event);
	void OnPropertyGridChange(wxPropertyGridEvent& event);
	void OnPropertyGridChanging(wxPropertyGridEvent& event);
	void OnNotebookPageChanged(wxNotebookEvent& event);
	void OnMaxTime(wxCommandEvent& event);
	void OnScoutingWorker(wxCommandEvent& event);
	void OnScoutingWorkerTime(wxCommandEvent& event);
	void OnScoutingWorkerDies(wxCommandEvent& event);
	void OnScoutingWorkerReturns(wxCommandEvent& event);
	void OnScoutingWorkerEndTime(wxCommandEvent& event);
	void OnMaxAPM(wxCommandEvent& event);
	void OnOutputURL(wxTextUrlEvent &event);
	void OnSave(wxCommandEvent& event);
	void OnSaveAs(wxCommandEvent& event);

	void AddWaypoint(wxCommandEvent & WXUNUSED(event));
	void RemoveWaypoint(wxCommandEvent & WXUNUSED(event));
	void Start(wxCommandEvent & WXUNUSED(event));
	void UpdateOutputFormat(wxCommandEvent & WXUNUSED(event));
	void UpdateInitialBuildOrder(wxCommandEvent & WXUNUSED(event));

	void UpdateScoutingCheckboxes();
	void UpdateOutputFormat();

private:
	wxString m_fileName;

	wxNotebook* m_notebookTargets;
	wxPropertyGrid* m_pgTarget;
	wxPanel *m_panelTarget;
	CVector<wxPropertyGrid *> m_pgWaypoints;
	CVector<wxPanel *> m_panelWaypoints;
	wxPanel* m_panelSettings;
	wxPropertyGrid* m_pgResult;
	wxButton* m_btnAddWaypoint;
	wxButton* m_btnRemoveWaypoint;
	wxButton* m_btnStart;
	wxStaticText* m_staticCompletionLikelihood;
	wxTextCtrl* m_txtCompletionLikelihood;
	wxListCtrl* m_listVillages;
	wxStaticText* m_staticText1;
	wxChoice* m_choiceOutput;
	wxTextCtrl* m_txtOutput;
	wxTimer* m_timer;

	wxTextCtrl *m_txtMaxTime;
	wxCheckBox *m_chkScoutingWorker;
	wxTextCtrl *m_txtScoutingWorkerTime;
	wxCheckBox *m_chkScoutingWorkerDies;
	wxCheckBox *m_chkScoutingWorkerReturns;
	wxTextCtrl *m_txtScoutingWorkerEndTime;

	wxChoice *m_choiceGasMicro;
	wxCheckBox *m_chkAllowTimeWaiting;
	wxCheckBox *m_chkMaxAPM;
	wxTextCtrl *m_txtMaxAPM;
	wxChoice *m_choiceInitialBuildOrder;
	wxTextCtrl *m_txtInitialBuildOrder;

	double m_scoutingWorkerTime;
	double m_scoutingWorkerEndTime;

	bool m_modified;

	CSC2Engine *m_engine;

	CSC2Engine::CPropertiesSet m_setDoubleProperties;
	CSC2Engine::CPropertiesSet m_setTimeProperties;
	CSC2Engine::CPropertiesSet m_setSizeTMinMaxProperties;
	CSC2Engine::CPropertiesSet m_setBoolMinMaxProperties;
	CSC2Engine::CPropertiesSet m_setBoolProperties;

	unsigned m_buildOrderNumber;

	bool StartEngine(CSC2Engine::EScoutingWorker scout, int scoutTime, int scoutEndTime, double minimumCommandDuration);
	void StopEngine();

	void PrintBestGame();

private:
	void OnActivate(wxActivateEvent& event);

	void OnClose(wxCommandEvent& event);
	void OnSize(wxSizeEvent& event);
	void OnMove(wxMoveEvent& event);
	void OnCloseWindow(wxCloseEvent& event);

#if wxUSE_CLIPBOARD
	void OnPaste(wxCommandEvent& event);
	void OnUpdatePaste(wxUpdateUIEvent& event);
#endif // wxUSE_CLIPBOARD

	void OnTimer(wxTimerEvent& event);

	DWORD m_startTickCount;
	void UpdateListBoxEntry(int nItem, size_t population, size_t evolution, size_t stagnationCount, unsigned long long gameCount, double bestFitness, DWORD timeDiff);
	void UpdateRemoveButton();
	bool UpdatePropertySummary(wxPGProperty *prop);
	bool InterpretProperty(wxPGProperty *prop);

	bool WriteToFile(wxString fileName);

	void AddWaypoint();

	void TestStuff();

	void SetModified(bool modified = true);
	void UpdateTitle();

	static unsigned ms_numChildren;

	// simple test event handler class
	class EventHandler : public wxEvtHandler
	{
	public:
		EventHandler(unsigned numChild) : m_numChild(numChild) { }

	private:
		void OnRefresh(wxCommandEvent& event)
		{
			wxLogMessage("Child #%u refreshed.", m_numChild);
			event.Skip();
		}

		const unsigned m_numChild;

		DECLARE_EVENT_TABLE()

		wxDECLARE_NO_COPY_CLASS(EventHandler);
	};

	DECLARE_EVENT_TABLE()
};

#endif // _MDICHILD_H_
