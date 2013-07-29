#ifndef TIMETEXTCTRL_H
#define TIMETEXTCTRL_H

class wxTimeTextCtrl : public wxTextCtrl
{
public:
	wxTimeTextCtrl();

	wxTimeTextCtrl(wxWindow *parent,
		wxWindowID id,
		const wxString& value = wxEmptyString,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = 0,
		const wxValidator& validator = wxDefaultValidator,
		const wxString& name = wxTextCtrlNameStr);

	virtual ~wxTimeTextCtrl() {}

protected:
	virtual void OnKillFocus(wxFocusEvent& event);

	double m_time;
};

#endif
