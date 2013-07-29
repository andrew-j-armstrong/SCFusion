#include "stdafx.h"
#include "RaceChoiceDlg.h"

BEGIN_EVENT_TABLE(CRaceChoiceDlg, wxDialog)
    EVT_BUTTON(wxID_OK, CRaceChoiceDlg::OnOK)
END_EVENT_TABLE()

IMPLEMENT_CLASS(CRaceChoiceDlg, wxDialog)

CRaceChoiceDlg::CRaceChoiceDlg(wxWindow *parent /* = NULL */)
	: wxDialog(parent, -1, "Choose Race")
{
    wxBeginBusyCursor();

    wxBoxSizer *topsizer = new wxBoxSizer( wxVERTICAL );

    wxSizerFlags flagsBorder2;
    flagsBorder2.DoubleBorder();

    topsizer->Add(CreateTextSizer("Select race:"), flagsBorder2);

	wxString races[] = {"Protoss", "Terran", "Zerg"};

	m_combo = new wxComboBox(this, wxID_ANY, "Protoss", wxDefaultPosition, wxDefaultSize, 3, races, wxCB_READONLY|wxCB_SORT);

    topsizer->Add(m_combo,
                  wxSizerFlags(0).
                    Expand().
                    TripleBorder(wxLEFT | wxRIGHT));

    wxSizer *buttonSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	topsizer->Add(buttonSizer, wxSizerFlags(flagsBorder2).Expand());

    SetAutoLayout( true );
    SetSizer( topsizer );

    topsizer->SetSizeHints( this );
    topsizer->Fit( this );

    Centre( wxBOTH );

    m_combo->SetFocus();

    wxEndBusyCursor();
}

CRaceChoiceDlg::~CRaceChoiceDlg()
{
}

void CRaceChoiceDlg::SetValue(const wxString &val)
{
    m_value = val;

    m_combo->SetValue(val);
}

void CRaceChoiceDlg::OnOK(wxCommandEvent &WXUNUSED(event) )
{
    m_value = m_combo->GetValue();

    EndModal(wxID_OK);
}
