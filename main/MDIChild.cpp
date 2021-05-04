#include "stdafx.h"

#include <wx/xml/xml.h>
#include <wx/print.h>

#include "bitmaps/save.xpm"

#include "MDIChild.h"
#include "MDIParent.h"
#include "ChartPanel.h"
#include "OutputPrintout.h"
#include "Core/HashFunction.h"
#include "AStar/ASEngine.h"
#include "AStar/ASBuildStateNode.h"
#include "AStar/ASStoreStateNode.h"
#include "AStar/ASSingleParent.h"
#include "AStar/ASNoDuplicateManager.h"
#include "AStar/ASHashTableStoreStateDuplicateManager.h"
#include "AStar/ASVectorOpenListManager.h"
#include "AStar/ASPriorityQueueOpenListManager.h"
#include "SC2/SC2Version.h"
#include "SC2/SC2FitnessCalc.h"
#include "SC2/SC2Waypoint.h"
#include "SC2/SC2State.h"
#include "SC2/SC2Command.h"
#include "SC2/SC2RaceInfo.h"
#include "SC2/SC2Race.h"

#define CONTROL_BORDER 3
#define WAYPOINT_GRID_SPLIT_POSITION 210

#define wxID_TIMER						(wxID_HIGHEST + 1)
#define wxID_MAXTIME					(wxID_HIGHEST + 2)
#define wxID_SCOUTINGWORKER				(wxID_HIGHEST + 3)
#define wxID_SCOUTINGWORKERTIME			(wxID_HIGHEST + 4)
#define wxID_SCOUTINGWORKER_DIES		(wxID_HIGHEST + 5)
#define wxID_SCOUTINGWORKER_RETURNS		(wxID_HIGHEST + 6)
#define wxID_SCOUTINGWORKERENDTIME		(wxID_HIGHEST + 7)
#define wxID_MAXAPM						(wxID_HIGHEST + 8)
#define wxID_INITIALBUILDORDER_CHOICE	(wxID_HIGHEST + 9)
#define wxID_OUTPUTFORMAT				(wxID_HIGHEST + 10)
#define wxID_OUTPUT						(wxID_HIGHEST + 11)
#define wxID_COMPLETIONLIKELIHOOD		(wxID_HIGHEST + 12)
#define wxID_EXPORT_SVG					(wxID_HIGHEST + 13)
#define wxID_LEVEL						(wxID_HIGHEST + 14)
#define wxID_PRINT	    				(wxID_HIGHEST + 15)

unsigned MyChild::ms_numChildren = 0;

BEGIN_EVENT_TABLE(MyChild, wxMDIChildFrame)
	EVT_MENU(wxID_CLOSE, MyChild::OnClose)

#if wxUSE_CLIPBOARD
	EVT_MENU(wxID_PASTE, MyChild::OnPaste)
	EVT_UPDATE_UI(wxID_PASTE, MyChild::OnUpdatePaste)
#endif // wxUSE_CLIPBOARD

	EVT_MENU(wxID_SAVE, MyChild::OnSave)
	EVT_MENU(wxID_SAVEAS, MyChild::OnSaveAs)
	EVT_MENU(wxID_EXPORT_SVG, MyChild::OnExportSVG)
	EVT_MENU(wxID_PRINT, MyChild::OnPrintButtonClicked)

	EVT_SIZE(MyChild::OnSize)
	EVT_MOVE(MyChild::OnMove)

	EVT_CLOSE(MyChild::OnCloseWindow)

	EVT_PG_CHANGED(-1, MyChild::OnPropertyGridChange)
	EVT_PG_CHANGING(-1, MyChild::OnPropertyGridChanging)
	EVT_NOTEBOOK_PAGE_CHANGED(-1, MyChild::OnNotebookPageChanged)

	EVT_CHECKBOX(wxID_SCOUTINGWORKER, MyChild::OnScoutingWorker)
	//EVT_KILL_FOCUS(wxID_SCOUTINGWORKERTIME, MyChild::OnScoutingWorkerTime)
	EVT_CHECKBOX(wxID_SCOUTINGWORKER_DIES, MyChild::OnScoutingWorkerDies)
	EVT_CHECKBOX(wxID_SCOUTINGWORKER_RETURNS, MyChild::OnScoutingWorkerReturns)
	//EVT_KILL_FOCUS(wxID_SCOUTINGWORKERENDTIME, MyChild::OnScoutingWorkerEndTime)

	EVT_CHECKBOX(wxID_MAXAPM, MyChild::OnMaxAPM)

	EVT_TIMER(wxID_TIMER, MyChild::OnTimer)

	EVT_TEXT_URL(wxID_OUTPUT, MyChild::OnOutputURL)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyChild::EventHandler, wxEvtHandler)
END_EVENT_TABLE()

MyChild::MyChild(wxMDIParentFrame *parent, CSC2Engine *engine, const char * const *xpmIcon, wxString fileName /* = wxEmptyString */)
	: wxMDIChildFrame(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE)
	, m_fileName(fileName)
	, m_timer(NULL)
	, m_engine(engine)
	, m_notebookTargets(NULL)
	, m_pgTarget(NULL)
	, m_panelTarget(NULL)
	, m_panelSettings(NULL)
	, m_pgResult(NULL)
	, m_btnAddWaypoint(NULL)
	, m_btnRemoveWaypoint(NULL)
	, m_btnStart(NULL)
	, m_btnExportSVG(NULL)
	, m_btnPrint(NULL)
	, m_staticCompletionLikelihood(NULL)
	, m_txtCompletionLikelihood(NULL)
	, m_listVillages(NULL)
	, m_staticText1(NULL)
	, m_choiceOutput(NULL)
	, m_choiceLevel(NULL)
	, m_txtOutput(NULL)
	, m_visualOutput(NULL)
	, m_gridOutput(NULL)
	, m_txtMaxTime(NULL)
	, m_choiceInitialBuildOrder(NULL)
	, m_txtInitialBuildOrder(NULL)
	, m_scoutingWorkerTime(0)
	, m_scoutingWorkerEndTime(0)
	, m_txtMaxAPM(NULL)
	, m_modified(false)
	, m_buildOrderNumber(++ms_numChildren)
	, m_gridOptionsSizer(NULL)
{
	wxIcon icon(xpmIcon);
	SetIcon(icon);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));

	// create our menu bar: it will be shown instead of the main frame one when
	// we're active
#if wxUSE_MENUS
	wxMenuBar *mbar = MyFrame::CreateMainMenubar();
	mbar->GetMenu(0)->Insert(4, wxID_SAVE, "&Save build order\tCtrl-S", "Save the build order");
	mbar->GetMenu(0)->Insert(5, wxID_SAVEAS, "&Save build order as...\tCtrl-Shift-S", "Save the build order as...");
	mbar->GetMenu(0)->Insert(6, wxID_CLOSE, "&Close build order\tCtrl-Del", "Close this build order");
	mbar->GetMenu(0)->InsertSeparator(7);
	mbar->GetMenu(0)->Insert(8, wxID_EXPORT_SVG, "Export to SVG", "Export to SVG");

	wxMenu *menuEdit = new wxMenu;

#if wxUSE_CLIPBOARD
	menuEdit->Append(wxID_COPY, "Copy output to clipboard\tCtrl-C");
#endif // wxUSE_CLIPBOARD

	mbar->Insert(1, menuEdit, "&Edit");

	// Associate the menu bar with the frame
	SetMenuBar(mbar);
#endif // wxUSE_MENUS

	// test that event handlers pushed on top of MDI children do work (this
	// used to be broken, see #11225)
	PushEventHandler(new EventHandler(ms_numChildren));

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer *bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer *bSizer9 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* btnSizer = new wxBoxSizer(wxHORIZONTAL);

	m_btnAddWaypoint = new wxButton(this, wxID_ADD, "Add Milestone");
	btnSizer->Add(m_btnAddWaypoint, 0, wxALL, CONTROL_BORDER);

	m_btnRemoveWaypoint = new wxButton(this, wxID_REMOVE, "Remove Milestone");
	btnSizer->Add(m_btnRemoveWaypoint, 0, wxALL, CONTROL_BORDER);
	m_btnRemoveWaypoint->Disable();

	bSizer9->AddSpacer(6);
	bSizer9->Add(btnSizer, 0, wxALL|wxALIGN_RIGHT, 0);

	m_notebookTargets = new wxNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_LEFT);
	m_notebookTargets->SetMinSize(wxSize(360,-1));

	m_panelTarget = new wxPanel(m_notebookTargets, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer *bSizer5 = new wxBoxSizer(wxVERTICAL);

	m_pgTarget = new wxPropertyGrid(m_panelTarget, -1, wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED);
	bSizer5->Add(m_pgTarget, 1, wxALL|wxEXPAND, 0);

	m_panelTarget->SetSizer(bSizer5);
	m_panelTarget->Layout();
	bSizer5->Fit(m_panelTarget);
	m_notebookTargets->AddPage(m_panelTarget, wxT("Target"), true);

	m_panelSettings = new wxPanel(m_notebookTargets, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer *bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer *bSizer60 = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText *staticMaxTime = new wxStaticText(m_panelSettings, wxID_ANY, "Maximum Time:");
	bSizer60->Add(staticMaxTime, 0, wxALIGN_CENTER_VERTICAL|wxALL, CONTROL_BORDER);
	bSizer60->Add(new wxBoxSizer(wxHORIZONTAL), 1, wxEXPAND, 0);
	m_txtMaxTime = new wxTextCtrl(m_panelSettings, wxID_MAXTIME, "20:00", wxDefaultPosition, wxSize(50, -1));
	bSizer60->Add(m_txtMaxTime, 0, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer6->Add(bSizer60, 0, wxEXPAND, 0);

	wxBoxSizer *bSizer61 = new wxBoxSizer(wxHORIZONTAL);
	m_chkScoutingWorker = new wxCheckBox(m_panelSettings, wxID_SCOUTINGWORKER, "Scouting Worker");
	bSizer61->Add(m_chkScoutingWorker, 0, wxALIGN_CENTER_VERTICAL|wxALL, CONTROL_BORDER);
	bSizer61->Add(new wxBoxSizer(wxHORIZONTAL), 1, wxEXPAND, 0);
	m_txtScoutingWorkerTime = new wxTextCtrl(m_panelSettings, wxID_SCOUTINGWORKERTIME, "0:00", wxDefaultPosition, wxSize(50, -1));
	bSizer61->Add(m_txtScoutingWorkerTime, 0, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer6->Add(bSizer61, 0, wxEXPAND, 0);

	wxBoxSizer *bSizer62 = new wxBoxSizer(wxHORIZONTAL);
	m_chkScoutingWorkerDies = new wxCheckBox(m_panelSettings, wxID_SCOUTINGWORKER_DIES, "Dies");
	bSizer62->Add(m_chkScoutingWorkerDies, 0, wxALIGN_CENTER_VERTICAL|wxALL, CONTROL_BORDER);
	m_chkScoutingWorkerReturns = new wxCheckBox(m_panelSettings, wxID_SCOUTINGWORKER_RETURNS, "Returns");
	bSizer62->Add(m_chkScoutingWorkerReturns, 0, wxALIGN_CENTER_VERTICAL|wxALL, CONTROL_BORDER);
	bSizer62->Add(new wxBoxSizer(wxHORIZONTAL), 1, wxEXPAND, 0);
	m_txtScoutingWorkerEndTime = new wxTextCtrl(m_panelSettings, wxID_SCOUTINGWORKERENDTIME, "0:00", wxDefaultPosition, wxSize(50, -1));
	bSizer62->Add(m_txtScoutingWorkerEndTime, 0, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer6->Add(bSizer62, 0, wxEXPAND, 0);

	wxBoxSizer *bSizer63 = new wxBoxSizer(wxHORIZONTAL);
	bSizer63->Add(new wxStaticText(m_panelSettings, wxID_ANY, "Gas Micro:", wxDefaultPosition, wxDefaultSize, 0), 0, wxALIGN_CENTER_VERTICAL|wxALL, CONTROL_BORDER);
	wxArrayString arrGasMicroChoices;
	arrGasMicroChoices.Add(wxT("Full micro"));
	arrGasMicroChoices.Add(wxT("Three back & forth"));
	arrGasMicroChoices.Add(wxT("One to gas only"));
	arrGasMicroChoices.Add(wxT("Three to gas only"));
	arrGasMicroChoices.Add(wxT("Three when geyser ready"));
	m_choiceGasMicro = new wxChoice(m_panelSettings, wxID_ANY, wxDefaultPosition, wxDefaultSize, arrGasMicroChoices, 0);
	m_choiceGasMicro->SetSelection(3);
	bSizer63->Add(m_choiceGasMicro, 1, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer6->Add(bSizer63, 0, wxEXPAND, 0);

	m_chkAllowTimeWaiting = new wxCheckBox(m_panelSettings, wxID_ANY, "Allow Wait Commands");
	bSizer6->Add(m_chkAllowTimeWaiting, 0, wxALL|wxEXPAND, CONTROL_BORDER);

	wxBoxSizer *bSizer65 = new wxBoxSizer(wxHORIZONTAL);
	m_chkMaxAPM = new wxCheckBox(m_panelSettings, wxID_MAXAPM, "Max APM:");
	bSizer65->Add(m_chkMaxAPM, 0, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer65->Add(new wxBoxSizer(wxHORIZONTAL), 1, wxEXPAND, 0);
	m_txtMaxAPM = new wxTextCtrl(m_panelSettings, wxID_ANY, "120", wxDefaultPosition, wxSize(50, -1));
	m_txtMaxAPM->Disable();
	bSizer65->Add(m_txtMaxAPM, 0, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer6->Add(bSizer65, 0, wxEXPAND, 0);

	wxBoxSizer *bSizer64 = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText *staticInitialBuildOrder = new wxStaticText(m_panelSettings, wxID_ANY, "Initial Build Order:", wxDefaultPosition, wxDefaultSize);
	bSizer64->Add(staticInitialBuildOrder, 0, wxALIGN_CENTER_VERTICAL|wxALL, CONTROL_BORDER);
	wxArrayString arrInitialBuildOrderChoices;
	arrInitialBuildOrderChoices.Add(wxT("<None>"));
	arrInitialBuildOrderChoices.Add(wxT("<Custom>"));
	engine->GetInitialBuildOrders(arrInitialBuildOrderChoices);
	m_choiceInitialBuildOrder = new wxChoice(m_panelSettings, wxID_INITIALBUILDORDER_CHOICE, wxDefaultPosition, wxDefaultSize, arrInitialBuildOrderChoices, 0);
	m_choiceInitialBuildOrder->SetSelection(0);
	bSizer64->Add(m_choiceInitialBuildOrder, 1, wxALL|wxEXPAND, CONTROL_BORDER);
	bSizer6->Add(bSizer64, 0, wxEXPAND, 0);
	m_txtInitialBuildOrder = new wxTextCtrl(m_panelSettings, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxHSCROLL|wxTE_RICH2);
	m_txtInitialBuildOrder->Disable();
	bSizer6->Add(m_txtInitialBuildOrder, 1, wxALL|wxEXPAND, CONTROL_BORDER);

	m_panelSettings->SetSizer(bSizer6);
	m_panelSettings->Layout();
	bSizer6->Fit(m_panelSettings);
	m_notebookTargets->AddPage(m_panelSettings, wxT("Settings"), false);

	bSizer9->Add(m_notebookTargets, 1, wxEXPAND | wxALL, CONTROL_BORDER);

	bSizer3->Add(bSizer9, 0, wxEXPAND, 0);

	wxBoxSizer *bSizer4 = new wxBoxSizer(wxVERTICAL);

	m_outputControlsSizer = new wxBoxSizer(wxHORIZONTAL);

	m_btnStart = new wxButton(this, wxID_APPLY, wxT("Start"), wxDefaultPosition, wxDefaultSize, 0);
	m_outputControlsSizer->Add(m_btnStart, 0, wxALL, CONTROL_BORDER);

	m_staticCompletionLikelihood = new wxStaticText(this, wxID_ANY, wxT("Completion Likelihood:"), wxDefaultPosition, wxDefaultSize, 0);
	m_outputControlsSizer->Add(m_staticCompletionLikelihood, 0, wxALIGN_CENTER_VERTICAL | wxALL, CONTROL_BORDER);

	m_txtCompletionLikelihood = new wxTextCtrl(this, wxID_COMPLETIONLIKELIHOOD, wxT("0.00 %"), wxDefaultPosition, wxSize(60, -1), wxTE_READONLY | wxTE_RIGHT);
	m_outputControlsSizer->Add(m_txtCompletionLikelihood, 0, wxALIGN_CENTER_VERTICAL | wxALL, CONTROL_BORDER);

	m_outputControlsSizer->AddSpacer(12);
	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Output Format:"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	m_outputControlsSizer->Add(m_staticText1, 0, wxALIGN_CENTER_VERTICAL | wxALL, CONTROL_BORDER);

	wxArrayString arrOutputChoices;
	arrOutputChoices.Add(wxT("Minimal"));
	arrOutputChoices.Add(wxT("Grid"));
	arrOutputChoices.Add(wxT("Chart"));
	m_choiceOutput = new wxChoice(this, wxID_OUTPUTFORMAT, wxDefaultPosition, wxDefaultSize, arrOutputChoices, 0);
	m_choiceOutput->SetSelection(1);
	m_outputControlsSizer->Add(m_choiceOutput, 0, wxALL, CONTROL_BORDER);
	m_outputControlsSizer->AddSpacer(12);

	m_gridOptionsSizer = new wxBoxSizer(wxHORIZONTAL);

	wxStaticText* levelText = new wxStaticText(this, wxID_ANY, wxT("Grid Level:"), wxDefaultPosition, wxDefaultSize, 0);
	levelText->Wrap(-1);
	m_gridOptionsSizer->Add(levelText, 0, wxALIGN_CENTER_VERTICAL | wxALL, CONTROL_BORDER);

	wxArrayString arrLevelChoices;
	arrLevelChoices.Add(wxT("Simple"));
	arrLevelChoices.Add(wxT("Detailed"));
	arrLevelChoices.Add(wxT("Full"));
	m_choiceLevel = new wxChoice(this, wxID_LEVEL, wxDefaultPosition, wxDefaultSize, arrLevelChoices, 0);
	m_choiceLevel->SetSelection(0);
	m_gridOptionsSizer->Add(m_choiceLevel, 0, wxALL, CONTROL_BORDER);

	m_outputControlsSizer->Add(m_gridOptionsSizer, 0, wxEXPAND, 0);

	m_btnExportSVG = new wxButton(this, wxID_EXPORT_SVG, wxT("Export SVG"), wxDefaultPosition, wxDefaultSize, 0);
	m_btnExportSVG->Hide();
	m_outputControlsSizer->Add(m_btnExportSVG, 0, wxALL, CONTROL_BORDER);

	bSizer4->AddSpacer(6);
	bSizer4->Add(m_outputControlsSizer, 0, wxEXPAND, 0);

	bSizer4->AddSpacer(6);
	m_btnPrint = new wxButton(this, wxID_PRINT, wxT("Print"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(m_btnPrint, 0, wxALL, CONTROL_BORDER);

	wxBoxSizer *bSizer10 = new wxBoxSizer(wxHORIZONTAL);

	m_txtOutput = new wxTextCtrl(this, wxID_OUTPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_READONLY|wxHSCROLL|wxTE_RICH2|wxTE_AUTO_URL);
	m_txtOutput->SetFont(wxFont(wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
	//m_txtOutput->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DLIGHT));
	bSizer10->Add(m_txtOutput, 1, wxEXPAND|wxALL, CONTROL_BORDER);

	m_visualOutput = new ChartPanel(this, wxID_ANY);
	bSizer10->Add(m_visualOutput, 1, wxEXPAND | wxALL, CONTROL_BORDER);
	m_visualOutput->Hide();

	m_gridOutput = new GridOutput(this, wxID_ANY, m_engine->GetRaceInfo()->GetData().m_hasLarvae);
	bSizer10->Add(m_gridOutput, 1, wxEXPAND | wxALL, CONTROL_BORDER);
	m_gridOutput->Hide();

	m_pgResult = new wxPropertyGrid(this, -1, wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED | wxPG_LIMITED_EDITING);
	m_pgResult->SetMinSize(wxSize(230,-1));
	bSizer10->Add(m_pgResult, 0, wxEXPAND|wxALL, CONTROL_BORDER);

	bSizer4->Add(bSizer10, 1, wxALL|wxEXPAND, 0);

	m_listVillages = new wxListCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(-1, 170), wxLC_REPORT | wxLC_SINGLE_SEL);
	bSizer4->Add(m_listVillages, 0, wxALL | wxEXPAND, CONTROL_BORDER);

#if wxUSE_TOOLBAR
	wxToolBar *toolBar = ((MyFrame *)GetParent())->GetToolBar();
	InitToolBar(toolBar);
#endif // wxUSE_TOOLBAR

#if wxUSE_ACCEL
	// Accelerators
	wxAcceleratorEntry entries[2];
	entries[0].Set(wxACCEL_CTRL, (int) 'S', wxID_SAVE);
	entries[1].Set(wxACCEL_CTRL, (int) 'A', wxID_SAVEAS);
	wxAcceleratorTable accel(2, entries);
	m_txtOutput->SetAcceleratorTable(accel);
#endif // wxUSE_ACCEL

	bSizer3->Add(bSizer4, 1, wxEXPAND, 0);

	this->SetSizer(bSizer3);
	this->Layout();

	this->Centre(wxBOTH);

	m_engine->AddTargetProperties(m_pgTarget, m_setDoubleProperties, m_setTimeProperties, m_setSizeTMinMaxProperties, m_setBoolMinMaxProperties, m_setBoolProperties);

	// Village list
	wxListItem itemCol;
	itemCol.SetImage(-1);
	itemCol.SetAlign(wxLIST_FORMAT_CENTRE);
	itemCol.SetWidth(100);

	itemCol.SetText(wxT("Name"));
	m_listVillages->InsertColumn(0, itemCol);
	itemCol.SetText(wxT("Population"));
	m_listVillages->InsertColumn(1, itemCol);
	itemCol.SetText(wxT("Evolution"));
	m_listVillages->InsertColumn(2, itemCol);
	itemCol.SetText(wxT("Stagnation"));
	m_listVillages->InsertColumn(3, itemCol);
	itemCol.SetText(wxT("Game Count"));
	m_listVillages->InsertColumn(4, itemCol);
	itemCol.SetText(wxT("Game Rate"));
	m_listVillages->InsertColumn(5, itemCol);
	itemCol.SetText(wxT("Best Fitness"));
	m_listVillages->InsertColumn(6, itemCol);

	m_listVillages->InsertItem(0, wxT("Overall"));
	m_listVillages->SetItem(0, 1, wxT("2000"));
	m_listVillages->InsertItem(1, wxT("City"));
	m_listVillages->SetItem(1, 1, wxT("1000"));
	m_listVillages->InsertItem(2, wxT("Village 1"));
	m_listVillages->SetItem(2, 1, wxT("200"));
	m_listVillages->InsertItem(3, wxT("Village 2"));
	m_listVillages->SetItem(3, 1, wxT("200"));
	m_listVillages->InsertItem(4, wxT("Village 3"));
	m_listVillages->SetItem(4, 1, wxT("200"));
	m_listVillages->InsertItem(5, wxT("Village 4"));
	m_listVillages->SetItem(5, 1, wxT("200"));
	m_listVillages->InsertItem(6, wxT("Village 5"));
	m_listVillages->SetItem(6, 1, wxT("200"));

	m_pgTarget->SetSplitterPosition(WAYPOINT_GRID_SPLIT_POSITION);

	m_pgResult->SetSplitterPosition(148);

	Connect(wxID_ADD, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(MyChild::AddWaypoint));

	Connect(wxID_REMOVE, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(MyChild::RemoveWaypoint));

	Connect(wxID_APPLY, wxEVT_COMMAND_BUTTON_CLICKED, 
		wxCommandEventHandler(MyChild::Start));

	Connect(wxID_OUTPUTFORMAT, wxEVT_COMMAND_CHOICE_SELECTED, 
		wxCommandEventHandler(MyChild::UpdateOutputFormat));

	Connect(wxID_LEVEL, wxEVT_COMMAND_CHOICE_SELECTED,
		wxCommandEventHandler(MyChild::UpdateOutputLevel));

	Connect(wxID_INITIALBUILDORDER_CHOICE, wxEVT_COMMAND_CHOICE_SELECTED, 
		wxCommandEventHandler(MyChild::UpdateInitialBuildOrder));

	Connect(wxID_EXPORT_SVG, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(MyChild::OnExportSVG));

	Connect(wxID_PRINT, wxEVT_COMMAND_BUTTON_CLICKED,
		wxCommandEventHandler(MyChild::OnPrintButtonClicked));

	m_txtMaxTime->Connect(wxEVT_KILL_FOCUS, wxCommandEventHandler(MyChild::OnMaxTime), 0, this);
	m_txtScoutingWorkerTime->Connect(wxEVT_KILL_FOCUS, wxCommandEventHandler(MyChild::OnScoutingWorkerTime), 0, this);
	m_txtScoutingWorkerEndTime->Connect(wxEVT_KILL_FOCUS, wxCommandEventHandler(MyChild::OnScoutingWorkerEndTime), 0, this);

	UpdateScoutingCheckboxes();
	UpdateOutputFormat();
	UpdateOutputLevel();
	UpdateTitle();
	Maximize();
	m_btnStart->SetFocus();
}

MyChild::~MyChild()
{
	PopEventHandler(true);

	ms_numChildren--;

	if(m_timer)
		StopEngine();

	delete m_engine;
}

void MyChild::OnScoutingWorker(wxCommandEvent &event)
{
	UpdateScoutingCheckboxes();

	SetModified();
}

void MyChild::OnScoutingWorkerDies(wxCommandEvent &event)
{
	if(m_chkScoutingWorkerDies->IsChecked())
		m_chkScoutingWorkerReturns->SetValue(false);
	UpdateScoutingCheckboxes();

	SetModified();
}

void MyChild::OnScoutingWorkerReturns(wxCommandEvent &event)
{
	if(m_chkScoutingWorkerReturns->IsChecked())
		m_chkScoutingWorkerDies->SetValue(false);
	UpdateScoutingCheckboxes();

	SetModified();
}

void MyChild::OnMaxAPM(wxCommandEvent& event)
{
	if(!m_chkMaxAPM->IsChecked())
	{
		m_txtMaxAPM->Disable();
	}
	else
	{
		m_txtMaxAPM->Enable();
	}

	SetModified();
}

void MyChild::UpdateScoutingCheckboxes()
{
	if(!m_chkScoutingWorker->IsChecked())
	{
		m_txtScoutingWorkerTime->Disable();
		m_chkScoutingWorkerDies->Disable();
		m_chkScoutingWorkerReturns->Disable();
		m_txtScoutingWorkerEndTime->Disable();
	}
	else
	{
		m_txtScoutingWorkerTime->Enable();
		m_chkScoutingWorkerDies->Enable();
		m_chkScoutingWorkerReturns->Enable();
		if(m_chkScoutingWorkerDies->IsChecked() || m_chkScoutingWorkerReturns->IsChecked())
			m_txtScoutingWorkerEndTime->Enable();
		else
			m_txtScoutingWorkerEndTime->Disable();
	}
}

void MyChild::UpdateOutputFormat(wxCommandEvent& event)
{
	UpdateOutputFormat();
}

void MyChild::UpdateOutputFormat()
{
	if (m_engine)
	{
		m_txtOutput->Hide();
		m_gridOutput->Hide();
		m_visualOutput->Hide();
		m_btnExportSVG->Hide();
		m_outputControlsSizer->Hide(m_gridOptionsSizer);
		switch (m_choiceOutput->GetCurrentSelection())
		{
		case 0:
			m_engine->SetOutput(new CSC2OutputMinimal());
			m_txtOutput->Show();
			break;
		case 1:
			m_engine->SetOutput(new CSC2OutputGrid());
			m_gridOutput->Show();
			m_gridOutput->SetLevel(m_choiceLevel->GetCurrentSelection());
			m_outputControlsSizer->Show(m_gridOptionsSizer);
			break;
		case 2:
			m_engine->SetOutput(new CSC2OutputChart());
			m_visualOutput->SetColorfulOutput();
			m_visualOutput->Show();
			m_btnExportSVG->Show();
			break;
		}
		this->Layout();
	}

	RefreshOutput();
}

void MyChild::UpdateOutputLevel(wxCommandEvent& event)
{
	UpdateOutputLevel();
}

void MyChild::UpdateOutputLevel()
{
	if (m_engine)
	{
		m_gridOutput->SetLevel(m_choiceLevel->GetCurrentSelection());
	}
	RefreshOutput();
}

void MyChild::UpdateInitialBuildOrder(wxCommandEvent &event)
{
	const wxString &description = m_choiceInitialBuildOrder->GetStringSelection();
	if(description == "<None>")
		m_txtInitialBuildOrder->Disable();
	else
	{
		m_txtInitialBuildOrder->Enable();
		if(description != "<Custom>")
		{
			wxString buildOrder;
			m_engine->GetInitialBuildOrder(description, buildOrder);
			m_txtInitialBuildOrder->SetValue(buildOrder);
		}
	}

	SetModified();
}

void MyChild::UpdateTitle()
{
	if(m_fileName.IsEmpty())
		SetTitle(wxString::Format("%s%s Build %u", m_modified ? "* " : "", m_engine->GetRaceString(), m_buildOrderNumber));
	else
		SetTitle(wxString::Format("%s%s Build - %s", m_modified ? "* " : "", m_engine->GetRaceString(), m_fileName));
}

void MyChild::OnPropertyGridChange(wxPropertyGridEvent &event)
{
	wxPGProperty *prop = event.GetProperty();

	wxPGProperty *parent = prop->GetParent();

	if(!parent->IsCategory())
	{
		if(!UpdatePropertySummary(parent))
		{
			if(event.CanVeto())
				event.Veto();
		}
	}
	else
	{
		if(!InterpretProperty(prop))
		{

		}
	}

	SetModified();
}

void MyChild::SetModified(bool modified /* = true */)
{
	m_modified = modified;
	UpdateTitle();
}

bool MyChild::UpdatePropertySummary(wxPGProperty *prop)
{
	if(m_setSizeTMinMaxProperties.find(prop) != m_setSizeTMinMaxProperties.end())
	{
		wxPGProperty *propMin = prop->GetPropertyByName(wxT("Minimum"));
		wxPGProperty *propMax = prop->GetPropertyByName(wxT("Maximum"));

		if(propMin->GetValue().GetString().IsEmpty())
		{
			if(propMax->GetValue().GetString().IsEmpty())
			{
				prop->SetValue(wxT(""));
				prop->SetModifiedStatus(false);
			}
			else
			{
				propMin->SetModifiedStatus(false);
				prop->SetValue(wxString::Format("<= %d", propMax->GetValue().GetLong()));
			}
		}
		else
		{
			if(propMax->GetValue().GetString().IsEmpty())
			{
				propMax->SetModifiedStatus(false);
				prop->SetValue(wxString::Format(">= %d", propMin->GetValue().GetLong()));
			}
			else
			{
				CSizeTOptionalMinMax minMax;
				minMax.haveMin = true;
				minMax.min = (size_t)propMin->GetValue().GetLong();
				minMax.haveMax = true;
				minMax.max = (size_t)propMax->GetValue().GetLong();

				// Set formatted
				wxString str;
				CSC2Engine::BuildMinMaxString(str, &minMax);
				prop->SetValueFromString(str);
			}
		}
	}

	return true;
}

bool MyChild::InterpretProperty(wxPGProperty *prop)
{
	if(m_setTimeProperties.find(prop) != m_setTimeProperties.end())
	{
		double time;
		if(!CSC2Engine::InterpretTimeValue(prop->GetValue(), time))
			return false;

		prop->SetValueFromString(CSC2Engine::TimeToString(time, 0));
	}

	if(m_setSizeTMinMaxProperties.find(prop) != m_setSizeTMinMaxProperties.end())
	{
		CSizeTOptionalMinMax minMax;
		if(!CSC2Engine::InterpretMinMaxValue(minMax, prop->GetValue().GetString()))
			return false;

		// Set formatted
		wxString str;
		CSC2Engine::BuildMinMaxString(str, &minMax);
		prop->SetValueFromString(str);

		prop->GetPropertyByName("Minimum")->SetValueFromString(minMax.haveMin ? wxString::Format("%d", minMax.min) : wxT(""));
		prop->GetPropertyByName("Maximum")->SetValueFromString(minMax.haveMax ? wxString::Format("%d", minMax.max) : wxT(""));

		prop->SetModifiedStatus(minMax.haveMin || minMax.haveMax);
		prop->GetPropertyByName("Minimum")->SetModifiedStatus(minMax.haveMin);
		prop->GetPropertyByName("Maximum")->SetModifiedStatus(minMax.haveMax);
	}

	if(m_setDoubleProperties.find(prop) != m_setDoubleProperties.end())
	{
		CDoubleMinMax minMax;
		if(!CSC2Engine::InterpretMinMaxValue(minMax, prop->GetValue().GetString()))
			return false;

		// Set formatted
		wxString str;
		CSC2Engine::BuildMinMaxString(str, &minMax);
		prop->SetValueFromString(str);
	}

	if(m_setBoolMinMaxProperties.find(prop) != m_setBoolMinMaxProperties.end())
	{
		CBoolMinMax minMax;
		if(!CSC2Engine::InterpretMinMaxValue(minMax, prop->GetValue().GetInteger()))
			return false;
	}

	return true;
}

void MyChild::OnPropertyGridChanging(wxPropertyGridEvent &event)
{
	wxPGProperty *p = event.GetProperty();

	wxPGProperty *parent = p->GetParent();

	if(parent->IsCategory())
	{
		if(m_setSizeTMinMaxProperties.find(p) != m_setSizeTMinMaxProperties.end())
		{
			CSizeTOptionalMinMax minMax;
			if(!CSC2Engine::InterpretMinMaxValue(minMax, event.GetPropertyValue().GetString()))
			{
				if(event.CanVeto())
					event.Veto();
				return;
			}
		}
	}

	wxLogVerbose("OnPropertyGridChanging(%s)", p->GetName().c_str());
}

void MyChild::OnNotebookPageChanged(wxNotebookEvent& event)
{
	UpdateRemoveButton();
}

void MyChild::UpdateRemoveButton()
{
	int index = m_notebookTargets->GetSelection();
	if(0 <= index && m_btnRemoveWaypoint)
	{
		if((size_t)index < m_panelWaypoints.size())
			m_btnRemoveWaypoint->Enable();
		else
			m_btnRemoveWaypoint->Disable();
	}
}

void MyChild::AddWaypoint(wxCommandEvent & WXUNUSED(event))
{
	AddWaypoint();
}

void MyChild::AddWaypoint()
{
	wxPanel *panel = new wxPanel(m_notebookTargets, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	wxBoxSizer *bSizer = new wxBoxSizer(wxVERTICAL);

	wxPropertyGrid *prop = new wxPropertyGrid(panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxPG_BOLD_MODIFIED);
	bSizer->Add(prop, 1, wxALL|wxEXPAND, 0);

	panel->SetSizer(bSizer);
	panel->Layout();
	bSizer->Fit(panel);
	m_notebookTargets->InsertPage(m_panelWaypoints.size(), panel, wxString::Format("Milestone %d", m_panelWaypoints.size() + 1), true);

	m_engine->AddWaypoint(prop, m_panelWaypoints.size() + 1, m_setDoubleProperties, m_setTimeProperties, m_setSizeTMinMaxProperties, m_setBoolMinMaxProperties, m_setBoolProperties);

	m_panelWaypoints.push_back(panel);
	m_pgWaypoints.push_back(prop);

	prop->SetSplitterPosition(WAYPOINT_GRID_SPLIT_POSITION);

	UpdateRemoveButton();
}

void MyChild::RemoveWaypoint(wxCommandEvent & WXUNUSED(event))
{
	int index = m_notebookTargets->GetSelection();
	if(0 <= index && (size_t)index < m_panelWaypoints.size())
	{
		m_panelWaypoints.erase(index);
		m_pgWaypoints.erase(index);
		m_notebookTargets->DeletePage(index);
		UpdateRemoveButton();
	}

	for (size_t i = 0; i < m_panelWaypoints.size(); i++)
	{
		m_notebookTargets->SetPageText(i, wxString::Format("Milestone %d", i + 1));
	}
}

void MyChild::Start(wxCommandEvent & WXUNUSED(event))
{
	if(!m_timer)
	{
		if(!Validate() || !m_panelSettings->Validate())
			return;
		if(!TransferDataFromWindow() || !m_panelSettings->TransferDataFromWindow())
			return;

		CSC2Engine::EScoutingWorker scout = CSC2Engine::eNoScout;
		if(m_chkScoutingWorker->IsChecked())
		{
			if(m_chkScoutingWorkerReturns->IsChecked())
				scout = CSC2Engine::eScoutReturns;
			else if(m_chkScoutingWorkerDies->IsChecked())
				scout = CSC2Engine::eScoutDies;
			else
				scout = CSC2Engine::eScoutStays;
		}

		double minimumCommandDuration = 0.0;
		if(m_chkMaxAPM->IsChecked())
		{
			double maxAPM = 0.0;
			if(!m_txtMaxAPM->GetValue().ToCDouble(&maxAPM) || 0.0 == maxAPM)
			{
				wxMessageDialog msg(this, "Please enter a valid APM.", "Invalid max APM");
				msg.ShowModal();

				m_notebookTargets->SetSelection(m_panelWaypoints.size());

				m_txtMaxAPM->SetFocus();
				return;
			}

			minimumCommandDuration = 60.0 / maxAPM;
		}

		if(scout != CSC2Engine::eNoScout)
		{
			if(0.0 == m_scoutingWorkerTime)
			{
				wxMessageDialog msg(this, "Please enter a valid scouting worker time.", "Invalid scouting worker time");
				msg.ShowModal();

				m_notebookTargets->SetSelection(m_panelWaypoints.size());

				m_txtScoutingWorkerTime->SetFocus();
				return;
			}

			if(scout == CSC2Engine::eScoutReturns || scout == CSC2Engine::eScoutDies)
			{
				if(0.0 == m_scoutingWorkerEndTime)
				{
					wxMessageDialog msg(this, "Please enter a valid scouting worker end time.", "Invalid scouting worker end time");
					msg.ShowModal();

					m_notebookTargets->SetSelection(m_panelWaypoints.size());

					m_txtScoutingWorkerEndTime->SetFocus();
					return;
				}
			}
			else
				m_scoutingWorkerEndTime = 0;
		}
		else
		{
			m_scoutingWorkerTime = 0;
			m_scoutingWorkerEndTime = 0;
		}

		if(m_choiceInitialBuildOrder->GetStringSelection() != "<None>"
			&& !m_engine->SetInitialBuildOrder(m_txtInitialBuildOrder->GetValue()))
		{
			wxMessageDialog msg(this, "Could not interpret initial build order.\nPlease use one command per line using the same description as the output.", "Invalid initial build order");
			msg.ShowModal();

			m_notebookTargets->SetSelection(m_panelWaypoints.size());

			m_txtInitialBuildOrder->SetFocus();
			return;
		}

		if(!StartEngine(scout, m_scoutingWorkerTime, m_scoutingWorkerEndTime, minimumCommandDuration))
			return;
		m_btnStart->SetLabel("Stop");
	}
	else
	{
		StopEngine();
		m_btnStart->SetLabel("Start");
	}
}

bool MyChild::StartEngine(CSC2Engine::EScoutingWorker scout, int scoutTime, int scoutEndTime, double minimumCommandDuration)
{

	SetModified();

	m_engine->SetGasMicro((EGasMicro)(1 + m_choiceGasMicro->GetSelection()));
	m_engine->SetAllowWaitCommands(m_chkAllowTimeWaiting->IsChecked());
	m_engine->SetMinimumCommandDuration(minimumCommandDuration);

	if(!m_engine->Start(m_pgWaypoints, m_pgTarget, scout, scoutTime, scoutEndTime))
		return false;

	m_timer = new wxTimer(this, wxID_TIMER);
	m_timer->Start(1000);

	m_startTickCount = GetTickCount();

	return true;
}

void MyChild::StopEngine()
{
	m_timer->Stop();
	delete m_timer;
	m_timer = NULL;

	m_engine->Stop();
}

void MyChild::UpdateListBoxEntry(int nItem, size_t population, size_t evolution, size_t stagnationCount, unsigned long long gameCount, double bestFitness, DWORD timeDiff)
{
	wxString strText;

	strText = wxString::Format(L"%u", population);
	//m_numberFormatInt = wxString::FormatNumberByLocale(strText, strText2);
	m_listVillages->SetItem(nItem, 1, strText);

	strText = wxString::Format(L"%u", evolution);
	//m_numberFormatInt = wxString::FormatNumberByLocale(strText, strText2);
	m_listVillages->SetItem(nItem, 2, strText);

	strText = wxString::Format(L"%u", stagnationCount);
	//m_numberFormatInt = wxString::FormatNumberByLocale(strText, strText2);
	m_listVillages->SetItem(nItem, 3, strText);

	strText = wxString::Format(L"%llu", gameCount);
	//m_numberFormatInt = wxString::FormatNumberByLocale(strText, strText2);
	m_listVillages->SetItem(nItem, 4, strText);

	if(0 != timeDiff)
	{
		strText = wxString::Format(L"%llu", (1000 * gameCount) / timeDiff);
		//m_numberFormatInt = wxString::FormatNumberByLocale(strText, strText2);
		m_listVillages->SetItem(nItem, 5, strText);
	}
	else
		m_listVillages->SetItem(nItem, 5, wxT("0"));

	strText = wxString::Format(L"%08.3lf", bestFitness);
	//m_numberFormatFloat = wxString::FormatNumberByLocale(strText, strText2);
	m_listVillages->SetItem(nItem, 6, strText);
}

void MyChild::OnTimer(wxTimerEvent& event)
{
	DWORD timeDiff = GetTickCount() - m_startTickCount;

	size_t totalPopulation, totalEvolution, totalStagnationCount;
	unsigned long long totalGameCount;
	double overallBestFitness;

	m_engine->GetCityStats(totalPopulation, totalEvolution, totalStagnationCount, totalGameCount, overallBestFitness);
	UpdateListBoxEntry(1, totalPopulation, totalEvolution, totalStagnationCount, totalGameCount, 1000.0 * overallBestFitness, timeDiff);

	double completionPercentage = 100.0 * (double)totalStagnationCount / mymax((double)totalEvolution, 50000.0);
	if(completionPercentage > 90.0)
	{
		m_txtCompletionLikelihood->SetValue(wxT("> 90.0 %"));
		m_txtCompletionLikelihood->SetBackgroundColour(wxColour(0, 255, 0));
	}
	else
	{
		unsigned char red, green;
		if(completionPercentage < 45.0)
		{
			red = 255;
			green = (unsigned char)(255.0 * completionPercentage / 45.0);
		}
		else
		{
			red = 255 - (unsigned char)(((completionPercentage - 45.0) / 45.0) * 255.0);
			green = 255;
		}

		m_txtCompletionLikelihood->SetValue(wxString::Format("%6.2f %%", completionPercentage));
		m_txtCompletionLikelihood->SetBackgroundColour(wxColour(red, green, 0));
	}

	for(size_t i=0; i < 5; i++)
	{
		size_t villagePopulation, villageEvolution, villageStagnationCount;
		unsigned long long villageGameCount;
		double villageBestFitness;

		m_engine->GetVillageStats(i, villagePopulation, villageEvolution, villageStagnationCount, villageGameCount, villageBestFitness);
		UpdateListBoxEntry(i+2, villagePopulation, villageEvolution, villageStagnationCount, villageGameCount, 1000.0 * villageBestFitness, timeDiff);

		totalPopulation += villagePopulation;
		totalGameCount += villageGameCount;
		if(overallBestFitness < villageBestFitness)
			overallBestFitness = villageBestFitness;
	}
	totalStagnationCount = m_engine->GetStagnationLimit();
	UpdateListBoxEntry(0, totalPopulation, totalEvolution, totalStagnationCount, totalGameCount, 1000.0 * overallBestFitness, timeDiff);

	m_engine->UpdateBestGame();

	RefreshOutput();
}

void MyChild::RefreshOutput()
{
	if (m_choiceOutput->GetCurrentSelection() == 1)
	{
		GetBestGameGridData();
	}
	else if (m_choiceOutput->GetCurrentSelection() == 2) {
		DrawBestGame();
	}
	else
	{
		PrintBestGame();
	}
}

void MyChild::PrintBestGame()
{
	wxString output;
	m_engine->PrintBestGame(output, m_pgResult);
	output.Trim();

	wxString existing = m_txtOutput->GetValue();
	if(existing != output)
	{
		int clientHeight = m_txtOutput->GetClientSize().GetHeight();

		wxTextCoord col, topRow, bottomRow;
		m_txtOutput->HitTest(wxPoint(0, 0), &col, &topRow);
		m_txtOutput->HitTest(wxPoint(0, clientHeight), &col, &bottomRow);

		int lastScrollPos = m_txtOutput->GetScrollPos(wxVERTICAL);
		int lastScrollRange = m_txtOutput->GetScrollRange(wxVERTICAL);

		m_txtOutput->Freeze();
		m_txtOutput->Clear();
		m_txtOutput->AppendText(output);

		m_txtOutput->ShowPosition(0);

		if(lastScrollRange - clientHeight - 1 > 0)
		{
			// Approximation
			int scrollableLines = m_txtOutput->GetNumberOfLines() - (bottomRow - topRow);
			m_txtOutput->ScrollLines(mymin((scrollableLines * lastScrollPos) / (lastScrollRange - clientHeight - 1), scrollableLines));
		}
		m_txtOutput->Thaw();
	}
}

bool compareRowStartTime(vector<ChartItem> a, vector<ChartItem> b)
{
	if (a.size() == 0) return false;
	if (b.size() == 0) return true;
	return a.front().startTime < b.front().startTime;
}

void MyChild::DrawBestGame()
{
	vector<vector<ChartItem>> chartItems;
	m_engine->DrawBestGame(chartItems, m_pgResult);
	if (chartItems.size() > 3)	sort(chartItems.begin() + 2, chartItems.end(), compareRowStartTime);
	m_visualOutput->SetChartItems(chartItems);
}

void MyChild::GetBestGameGridData()
{
	vector<GridItem> data;
	m_engine->GetBestGameGridData(data, m_pgResult);
	m_gridOutput->SetData(data);
}

void MyChild::OnSave(wxCommandEvent& WXUNUSED(event))
{
	DoSave();
}

bool MyChild::DoSave()
{
	if(m_fileName.IsEmpty())
		return DoSaveAs();
	else
	{
		if(!WriteToFile(m_fileName))
			return false;

		SetModified(false);
		UpdateTitle();
		return true;
	}
}

void MyChild::OnSaveAs(wxCommandEvent& WXUNUSED(event))
{
	DoSaveAs();
}

bool MyChild::DoSaveAs()
{
	wxFileDialog *saveAsDialog = new wxFileDialog(this, wxT("Choose a file to open"), wxEmptyString, wxEmptyString, wxT("XML files (*.xml)|*.xml|All Files (*.*)|*.*"), wxFD_SAVE);

	if (saveAsDialog->ShowModal() != wxID_OK)
	{
		saveAsDialog->Destroy();
		return false;
	}

	m_fileName = saveAsDialog->GetPath();

	saveAsDialog->Destroy();

	if(!WriteToFile(m_fileName))
		return false;

	SetModified(false);
	UpdateTitle();

	return true;
}

void MyChild::OnPrintButtonClicked(wxCommandEvent& WXUNUSED(event))
{
	wxPrintData* g_printData = NULL;
	wxPageSetupDialogData* g_pageSetupData = NULL;

	g_printData = new wxPrintData;
	g_pageSetupData = new wxPageSetupDialogData;

	// copy over initial paper size from print record
	(*g_pageSetupData) = *g_printData;

	// Set some initial page margins in mm.
	g_pageSetupData->SetMarginTopLeft(wxPoint(300, 300));
	g_pageSetupData->SetMarginBottomRight(wxPoint(300, 300));


	// Pass two printout objects: for preview, and possible printing.
	wxPrintDialogData printDialogData(*g_printData);
	wxPrintPreview* preview =
		new wxPrintPreview(new OutputPrintout(m_gridOutput), new OutputPrintout(m_gridOutput), &printDialogData);
	if (!preview->IsOk()) {
		delete preview;
		wxLogError("There was a problem previewing.\nPerhaps your current printer is not set correctly?");
		return;
	}

	wxPreviewFrame* preview_frame =
		new wxPreviewFrame(preview, m_gridOutput, "Print Preview", wxPoint(0, 0), wxSize(1200, 650));
	preview_frame->Centre(wxBOTH);
	preview_frame->InitializeWithModality(wxPreviewFrame_AppModal);
	preview_frame->Show();
}


void MyChild::OnExportSVG(wxCommandEvent& WXUNUSED(event))
{
	DoExportSVG();
}

bool MyChild::DoExportSVG()
{
	wxFileDialog* exportSvgDialog = new wxFileDialog(this, wxT("Save SVG file"), wxEmptyString, wxEmptyString, wxT("SVG files (*.svg)|*.svg|All Files (*.*)|*.*"), wxFD_SAVE);

	if (exportSvgDialog->ShowModal() != wxID_OK)
	{
		exportSvgDialog->Destroy();
		return false;
	}

	wxString path = exportSvgDialog->GetPath();

	exportSvgDialog->Destroy();

	return m_visualOutput->ExportSVG(path);
}

bool MyChild::WriteToFile(wxString fileName)
{
	wxXmlNode *rootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("SCFusionBuild"));

	wxXmlNode *xmlFileVersionNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("XMLFileVersion"));
	xmlFileVersionNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxT("1.0")));
	rootNode->AddChild(xmlFileVersionNode);

	wxXmlNode *gameLongDescriptionNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("GameLongDescription"));
	gameLongDescriptionNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, m_engine->GetGameLongDescription()));
	rootNode->AddChild(gameLongDescriptionNode);

	wxXmlNode *gameMinVersionNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("GameMinVersion"));
	gameMinVersionNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, m_engine->GetMinGameVersion()));
	rootNode->AddChild(gameMinVersionNode);

	wxXmlNode *raceNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Race"));
	raceNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, m_engine->GetRaceString()));
	rootNode->AddChild(raceNode);

	wxXmlNode *settingsNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Settings"));
	{
		double timeValue;
		if(CSC2Engine::InterpretTimeValue(m_txtMaxTime->GetValue(), timeValue))
		{
			wxXmlNode *maximumTimeNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("MaximumTime"));
			maximumTimeNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%g", timeValue)));
			settingsNode->AddChild(maximumTimeNode);
		}
		if(m_chkScoutingWorker->GetValue() && CSC2Engine::InterpretTimeValue(m_txtScoutingWorkerTime->GetValue(), timeValue))
		{
			wxXmlNode *scoutingWorkerNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("ScoutingWorker"));
			scoutingWorkerNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%g", timeValue)));
			if(CSC2Engine::InterpretTimeValue(m_txtScoutingWorkerEndTime->GetValue(), timeValue))
			{
				if(m_chkScoutingWorkerReturns->GetValue())
					scoutingWorkerNode->AddAttribute(wxT("returnTime"), wxString::Format("%g", timeValue));
				else if(m_chkScoutingWorkerDies->GetValue())
					scoutingWorkerNode->AddAttribute(wxT("deathTime"), wxString::Format("%g", timeValue));
			}
			settingsNode->AddChild(scoutingWorkerNode);
		}

		wxXmlNode *gasMicroNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("GasMicro"));
		gasMicroNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, m_choiceGasMicro->GetString(m_choiceGasMicro->GetCurrentSelection())));
		settingsNode->AddChild(gasMicroNode);

		wxXmlNode *allowWaitCommandsNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("AllowWaitCommands"));
		allowWaitCommandsNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, m_chkAllowTimeWaiting->GetValue() ? wxT("True") : wxT("False")));
		settingsNode->AddChild(allowWaitCommandsNode);

		if(m_chkMaxAPM->GetValue())
		{
			wxXmlNode *maxAPMNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("MaxAPM"));
			maxAPMNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, m_txtMaxAPM->GetValue()));
			settingsNode->AddChild(maxAPMNode);
		}

		if(m_choiceInitialBuildOrder->GetString(m_choiceInitialBuildOrder->GetCurrentSelection()) == "<Custom>")
		{
			wxXmlNode *initialBuildOrderNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("InitialBuildOrder"));

			wxStringTokenizer initialBuildOrderTokenizer(m_txtInitialBuildOrder->GetValue(), wxT("\r\n"), wxTOKEN_STRTOK);

			while(initialBuildOrderTokenizer.HasMoreTokens())
			{
				wxString token = initialBuildOrderTokenizer.GetNextToken();

				wxXmlNode *initialBuildOrderCommandNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Command"));
				initialBuildOrderCommandNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, token));
				initialBuildOrderNode->AddChild(initialBuildOrderCommandNode);
			}

			settingsNode->AddChild(initialBuildOrderNode);
		}
	}
	rootNode->AddChild(settingsNode);

	wxXmlNode *waypointsNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Waypoints"));
	{
		for(size_t i=0; i < m_pgWaypoints.size() + 1; i++)
		{
			const wxPropertyGrid *propertyGrid = (i < m_pgWaypoints.size() ? m_pgWaypoints[i] : m_pgTarget);

			wxXmlNode *waypointNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Waypoint"));
			if(i >= m_pgWaypoints.size())
				waypointNode->AddAttribute("target", "true");

			for(wxPropertyGridConstIterator iter = propertyGrid->GetIterator(); !iter.AtEnd(); iter.Next(false))
			{
				const wxPGProperty *prop = iter.GetProperty();

				if(m_setTimeProperties.end() != m_setTimeProperties.find(prop))
				{
					double timeValue;
					if(CSC2Engine::InterpretTimeValue(prop->GetValue(), timeValue) && 0.0 != timeValue)
					{
						if(prop->GetName() == wxT("Min Time") || prop->GetName() == wxT("Max Time"))
						{
							wxString nodeName = prop->GetName();
							nodeName.Replace(" ", "");
							wxXmlNode *targetNode = new wxXmlNode(wxXML_ELEMENT_NODE, nodeName);
							targetNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%g", timeValue)));
							waypointNode->AddChild(targetNode);
						}
						else
						{
							wxXmlNode *targetNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Target"));
							targetNode->AddAttribute("name", prop->GetName());
							wxXmlNode *valueNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
							valueNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%g", (double)prop->GetValue())));
							targetNode->AddChild(valueNode);
							waypointNode->AddChild(targetNode);
						}
					}
				}
				else if(m_setDoubleProperties.end() != m_setDoubleProperties.find(prop))
				{
					if((double)prop->GetValue() != 0.0)
					{
						wxXmlNode *targetNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Target"));
						targetNode->AddAttribute("name", prop->GetName());
						wxXmlNode *valueNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
						valueNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, wxString::Format("%g", (double)prop->GetValue())));
						targetNode->AddChild(valueNode);
						waypointNode->AddChild(targetNode);
					}
				}
				else if(m_setSizeTMinMaxProperties.end() != m_setSizeTMinMaxProperties.find(prop))
				{
					wxXmlNode *targetNode = NULL;

					for(size_t j=0; j < prop->GetChildCount(); j++)
					{
						wxPGProperty *propChild = prop->Item(j);
						if(!((wxString)propChild->GetValue()).IsEmpty())
						{
							wxString nodeName = propChild->GetName().Mid(propChild->GetName().Find('.') + 1);
							nodeName.Replace(" ", "");
							if(NULL == targetNode)
							{
								targetNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Target"));
								targetNode->AddAttribute("name", prop->GetName());
							}

							wxXmlNode *valueNode = new wxXmlNode(wxXML_ELEMENT_NODE, nodeName);
							valueNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, (wxString)propChild->GetValue()));
							targetNode->AddChild(valueNode);
						}
					}

					if(NULL != targetNode)
						waypointNode->AddChild(targetNode);
				}
				else if(m_setBoolMinMaxProperties.end() != m_setBoolMinMaxProperties.find(prop)
					|| m_setBoolProperties.end() != m_setBoolProperties.find(prop))
				{
					wxEnumProperty *propEnum = (wxEnumProperty *)prop;
					if(propEnum->GetValueAsString() != wxT("Optional"))
					{
						wxXmlNode *targetNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Target"));
						targetNode->AddAttribute("name", prop->GetName());
						wxXmlNode *valueNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Value"));
						valueNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, propEnum->GetValueAsString()));
						targetNode->AddChild(valueNode);
						waypointNode->AddChild(targetNode);
					}
				}
			}

			waypointsNode->AddChild(waypointNode);
		}
	}
	rootNode->AddChild(waypointsNode);

	wxXmlNode *buildOrderNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("BuildOrder"));
	{
		const CVector<const CSC2Command *> &bestGame = m_engine->GetBestGame().GetValue();

		for(size_t i=0; i < bestGame.size(); i++)
		{
			wxXmlNode *commandNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("Command"));
			commandNode->AddChild(new wxXmlNode(wxXML_TEXT_NODE, wxEmptyString, bestGame[i]->GetName()));
			buildOrderNode->AddChild(commandNode);
		}
	}
	rootNode->AddChild(buildOrderNode);

	wxXmlDocument doc;
	doc.SetRoot(rootNode);
	doc.Save(fileName);

	return true;
}

void MyChild::OnClose(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void MyChild::OnActivate(wxActivateEvent& event)
{
}

void MyChild::OnMove(wxMoveEvent& event)
{
	event.Skip();
}

void MyChild::OnSize(wxSizeEvent& event)
{
	Refresh();
	event.Skip();
}

void MyChild::OnCloseWindow(wxCloseEvent& event)
{
	if(event.CanVeto() && m_modified)
	{
		wxMessageDialog msg(this, "Do you want to save the build order before closing?", "Close Build Order", wxYES | wxNO | wxCANCEL);
		int dlgResult = msg.ShowModal();
		if(wxID_CANCEL == dlgResult)
		{
			event.Veto();
			return;
		}
		else if(wxID_YES == dlgResult)
		{
			if(!DoSave())
			{
				event.Veto();
				return;
			}
		}
	}

	event.Skip();
}

#if wxUSE_CLIPBOARD

#include "wx/clipbrd.h"

void MyChild::OnPaste(wxCommandEvent& WXUNUSED(event))
{
	wxClipboardLocker lock;
	wxTextDataObject data;
}

void MyChild::OnUpdatePaste(wxUpdateUIEvent& event)
{
	wxClipboardLocker lock;
	event.Enable(wxTheClipboard->IsSupported(wxDF_TEXT));
}

#endif // wxUSE_CLIPBOARD

void MyChild::OnOutputURL(wxTextUrlEvent &event)
{
	if(event.GetMouseEvent().LeftDown())
	{
		wxString output = m_txtOutput->GetValue();
		wxLaunchDefaultBrowser(output.SubString(event.GetURLStart(), event.GetURLEnd()));
	}
}

void MyChild::OnMaxTime(wxCommandEvent& event)
{
	if(!CSC2Engine::InterpretTimeValue(m_txtMaxTime->GetValue(), m_engine->GetMaxTargetTime()) || m_engine->GetMaxTargetTime() == 0.0)
		m_engine->GetMaxTargetTime() = 20.0;

	m_txtMaxTime->SetValue(CSC2Engine::TimeToString(m_engine->GetMaxTargetTime(), 0));

	event.Skip();
}

void MyChild::OnScoutingWorkerTime(wxCommandEvent& event)
{
	if(!CSC2Engine::InterpretTimeValue(m_txtScoutingWorkerTime->GetValue(), m_scoutingWorkerTime))
		m_scoutingWorkerTime = 0.0;

	m_txtScoutingWorkerTime->SetValue(CSC2Engine::TimeToString(m_scoutingWorkerTime, 0));

	event.Skip();
}

void MyChild::OnScoutingWorkerEndTime(wxCommandEvent& event)
{
	if(!CSC2Engine::InterpretTimeValue(m_txtScoutingWorkerEndTime->GetValue(), m_scoutingWorkerEndTime))
		m_scoutingWorkerEndTime = 0.0;

	m_txtScoutingWorkerEndTime->SetValue(CSC2Engine::TimeToString(m_scoutingWorkerEndTime, 0));

	event.Skip();
}

#if wxUSE_TOOLBAR
void MyChild::InitToolBar(wxToolBar* toolBar)
{
}
#endif // wxUSE_TOOLBAR

bool MyChild::LoadSettingsFromXML(wxXmlNode *node)
{
	while(node)
	{
		if(node->GetName() == wxT("MaximumTime"))
		{
			double maxTime;
			if(CSC2Engine::InterpretTimeValue(node->GetNodeContent(), maxTime))
				m_txtMaxTime->SetValue(CSC2Engine::TimeToString(maxTime, 0));
		}
		else if(node->GetName() == wxT("ScoutingWorker"))
		{
			if(CSC2Engine::InterpretTimeValue(node->GetNodeContent(), m_scoutingWorkerTime))
			{
				m_chkScoutingWorker->SetValue(true);
				m_txtScoutingWorkerTime->Enable(true);
				m_chkScoutingWorkerReturns->Enable(true);
				m_chkScoutingWorkerDies->Enable(true);
				m_txtScoutingWorkerTime->SetValue(CSC2Engine::TimeToString(m_scoutingWorkerTime, 0));

				wxString endTimeString;
				if(node->GetAttribute(wxT("returnTime"), &endTimeString) && CSC2Engine::InterpretTimeValue(endTimeString, m_scoutingWorkerEndTime))
				{
					m_chkScoutingWorkerReturns->SetValue(true);
					m_txtScoutingWorkerEndTime->Enable(true);
					m_txtScoutingWorkerEndTime->SetValue(CSC2Engine::TimeToString(m_scoutingWorkerEndTime, 0));
				}
				else if(node->GetAttribute(wxT("deathTime"), &endTimeString) && CSC2Engine::InterpretTimeValue(endTimeString, m_scoutingWorkerEndTime))
				{
					m_chkScoutingWorkerDies->SetValue(true);
					m_txtScoutingWorkerEndTime->Enable(true);
					m_txtScoutingWorkerEndTime->SetValue(CSC2Engine::TimeToString(m_scoutingWorkerEndTime, 0));
				}
			}
		}
		else if(node->GetName() == wxT("GasMicro"))
		{
			int nSelection = m_choiceGasMicro->FindString(node->GetNodeContent());
			if(-1 == nSelection)
			{
				wxMessageBox(wxString::Format("Unknown gas micro: '%s'", node->GetNodeContent()));
				return false;
			}
			m_choiceGasMicro->SetSelection(nSelection);
		}
		else if(node->GetName() == wxT("AllowWaitCommands"))
		{
			if(0 == node->GetNodeContent().CompareTo(wxT("True"), wxString::ignoreCase))
				m_chkAllowTimeWaiting->SetValue(true);
		}
		else if(node->GetName() == wxT("MaxAPM"))
		{
			double maxAPM;
			if(node->GetNodeContent().ToCDouble(&maxAPM))
			{
				m_chkMaxAPM->SetValue(true);
				m_txtMaxAPM->Enable(true);
				m_txtMaxAPM->SetValue(wxString::Format("%g", maxAPM));
			}
		}
		else if(node->GetName() == wxT("InitialBuildOrder"))
		{
			m_choiceInitialBuildOrder->SetSelection(m_choiceInitialBuildOrder->FindString(wxT("<Custom>")));
			m_txtInitialBuildOrder->Enable(true);

			wxString initialBuildOrder;

			wxXmlNode *nodeInitialBuildOrder = node->GetChildren();
			while(nodeInitialBuildOrder)
			{
				if(nodeInitialBuildOrder->GetName() == wxT("Command"))
				{
					initialBuildOrder.Append(nodeInitialBuildOrder->GetNodeContent());
					initialBuildOrder.Append("\r\n");
				}
				else
				{
					wxMessageBox(wxString::Format("Unexpected node in InitialBuildOrder: '%s'", nodeInitialBuildOrder->GetName()));
					return false;
				}

				nodeInitialBuildOrder = nodeInitialBuildOrder->GetNext();
			}

			m_engine->SetInitialBuildOrder(initialBuildOrder);
			m_txtInitialBuildOrder->SetValue(initialBuildOrder);
		}

		node = node->GetNext();
	}

	CSC2Engine::EScoutingWorker scout = CSC2Engine::eNoScout;
	if(m_chkScoutingWorker->IsChecked())
	{
		if(m_chkScoutingWorkerReturns->IsChecked())
			scout = CSC2Engine::eScoutReturns;
		else if(m_chkScoutingWorkerDies->IsChecked())
			scout = CSC2Engine::eScoutDies;
		else
			scout = CSC2Engine::eScoutStays;
	}

	m_engine->InitialiseFitnessCalc(m_pgWaypoints, m_pgTarget, scout, m_scoutingWorkerTime, m_scoutingWorkerEndTime);

	RefreshOutput();

	return true;
}

bool MyChild::LoadWaypointsFromXML(wxXmlNode *node)
{
	const CVector<CSC2Target *> &targets = m_engine->GetRaceInfo()->GetTargets();

	while(node)
	{
		if(node->GetName() == wxT("Waypoint"))
		{
			wxPropertyGrid *pgWaypoint;
			if(node->GetAttribute(wxT("target"), wxT("false")) == wxT("true"))
			{
				pgWaypoint = m_pgTarget;
			}
			else
			{
				AddWaypoint();
				pgWaypoint = m_pgWaypoints[m_pgWaypoints.size() - 1];
			}

			wxXmlNode *nodeWaypoint = node->GetChildren();
			while(nodeWaypoint)
			{
				if(nodeWaypoint->GetName() == wxT("MinTime"))
				{
					wxPGProperty *prop = pgWaypoint->GetProperty("Min Time");

					prop->SetValue(nodeWaypoint->GetNodeContent());
					if(!InterpretProperty(prop))
					{
						wxMessageBox(wxString::Format("Could not interpret min time: '%s'", nodeWaypoint->GetNodeContent()));
						return false;
					}
					prop->SetModifiedStatus(true);
				}
				else if(nodeWaypoint->GetName() == wxT("MaxTime"))
				{
					wxPGProperty *prop = pgWaypoint->GetProperty("Max Time");

					prop->SetValue(nodeWaypoint->GetNodeContent());
					if(!InterpretProperty(prop))
					{
						wxMessageBox(wxString::Format("Could not interpret max time: '%s'", nodeWaypoint->GetNodeContent()));
						return false;
					}
					prop->SetModifiedStatus(true);
				}
				else if(nodeWaypoint->GetName() == wxT("Target"))
				{
					wxPGProperty *prop = NULL;

					wxString targetName = nodeWaypoint->GetAttribute("name");
					if(targetName != "Bases"
						&& targetName != "Supply"
						&& targetName != "Minerals"
						&& targetName != "Gas"
						&& targetName != "Minerals Mined"
						&& targetName != "Gas Mined")
					{
						for(size_t i=0; i < targets.size(); i++)
						{
							if(targets[i]->GetName() == targetName)
								break;

							if(i == targets.size() - 1)
							{
								wxFAIL_MSG(wxString::Format("Unknown target '%s'", targetName));
								return false;
							}
						}
					}

					prop = pgWaypoint->GetProperty(targetName);

					wxXmlNode *nodeTarget = nodeWaypoint->GetChildren();

					if(m_setSizeTMinMaxProperties.end() != m_setSizeTMinMaxProperties.find(prop))
					{
						while(nodeTarget)
						{
							const wxString &content = nodeTarget->GetNodeContent();

							if(nodeTarget->GetName() == wxT("Minimum"))
							{
								long minimum;
								if(content.ToLong(&minimum))
									prop->GetPropertyByName("Minimum")->SetValue(minimum);
							}
							else if(nodeTarget->GetName() == wxT("Maximum"))
							{
								long maximum;
								if(content.ToLong(&maximum))
									prop->GetPropertyByName("Maximum")->SetValue(maximum);
							}
							else if(nodeTarget->GetName() == wxT("MinimumUnderConstruction"))
							{
								long minimumUnderConstruction;
								if(content.ToLong(&minimumUnderConstruction))
									prop->GetPropertyByName("Minimum Under Construction")->SetValue(minimumUnderConstruction);
							}
							else if(nodeTarget->GetName() == wxT("MaximumUnderConstruction"))
							{
								long maximumUnderConstruction;
								if(content.ToLong(&maximumUnderConstruction))
									prop->GetPropertyByName("Maximum Under Construction")->SetValue(maximumUnderConstruction);
							}
							else
							{
								wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Target>: '%s'", nodeTarget->GetName()));
								return false;
							}

							nodeTarget = nodeTarget->GetNext();
						}

						UpdatePropertySummary(prop);
					}
					else if(m_setDoubleProperties.end() != m_setDoubleProperties.find(prop))
					{
						while(nodeTarget)
						{
							const wxString &content = nodeTarget->GetNodeContent();

							if(nodeTarget->GetName() == wxT("Value"))
							{
								double minimum;
								if(content.ToDouble(&minimum))
									prop->SetValue(minimum);
							}
							else
							{
								wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Target>: '%s'", nodeTarget->GetName()));
								return false;
							}

							nodeTarget = nodeTarget->GetNext();
						}
					}
					else if(m_setBoolMinMaxProperties.end() != m_setBoolMinMaxProperties.find(prop))
					{
						while(nodeTarget)
						{
							const wxString &content = nodeTarget->GetNodeContent();

							if(nodeTarget->GetName() == wxT("Value"))
							{
								prop->SetValue(content);
							}
							else
							{
								wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Target>: '%s'", nodeTarget->GetName()));
								return false;
							}

							nodeTarget = nodeTarget->GetNext();
						}
					}
					else if(m_setBoolProperties.end() != m_setBoolProperties.find(prop))
					{
						while(nodeTarget)
						{
							const wxString &content = nodeTarget->GetNodeContent();

							if(nodeTarget->GetName() == wxT("Value"))
							{
								prop->SetValue(content);
							}
							else
							{
								wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Target>: '%s'", nodeTarget->GetName()));
								return false;
							}

							nodeTarget = nodeTarget->GetNext();
						}
					}

					prop->SetModifiedStatus(true);
				}
				else
				{
					wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Waypoint>: '%s'", nodeWaypoint->GetName()));
					return false;
				}

				nodeWaypoint = nodeWaypoint->GetNext();
			}
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Waypoints>: '%s'", node->GetName()));
			return false;
		}

		node = node->GetNext();
	}

	CSC2Engine::EScoutingWorker scout = CSC2Engine::eNoScout;
	if(m_chkScoutingWorker->IsChecked())
	{
		if(m_chkScoutingWorkerReturns->IsChecked())
			scout = CSC2Engine::eScoutReturns;
		else if(m_chkScoutingWorkerDies->IsChecked())
			scout = CSC2Engine::eScoutDies;
		else
			scout = CSC2Engine::eScoutStays;
	}

	m_engine->InitialiseFitnessCalc(m_pgWaypoints, m_pgTarget, scout, m_scoutingWorkerTime, m_scoutingWorkerEndTime);

	RefreshOutput();

	return true;
}

bool MyChild::LoadBuildOrderFromXML(wxXmlNode *node)
{
	std::vector<wxString> seed;

	while(node)
	{
		if(node->GetName() == wxT("Command"))
			seed.push_back(node->GetNodeContent());
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <BuildOrder>: '%s'", node->GetName()));
			return false;
		}

		node = node->GetNext();
	}

	m_engine->SetSeed(seed);

	RefreshOutput();

	return true;
}
