#pragma once

class CRaceChoiceDlg : public wxDialog
{
public:
	CRaceChoiceDlg(wxWindow *parent = NULL);
	~CRaceChoiceDlg();

    void OnOK(wxCommandEvent& event);

    void SetValue(const wxString& val);
    wxString GetValue() const { return m_value; }

protected:
	wxComboBox *m_combo;
    wxString    m_value;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxTextEntryDialog)
};
