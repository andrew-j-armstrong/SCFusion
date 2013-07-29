#include "stdafx.h"
#include "TimeTextCtrl.h"

wxTimeTextCtrl::wxTimeTextCtrl()
	: wxTextCtrl()
{
}

wxTimeTextCtrl::wxTimeTextCtrl(wxWindow *parent,
	wxWindowID id,
	const wxString& value /* = wxEmptyString */,
	const wxPoint& pos /* = wxDefaultPosition */,
	const wxSize& size /* = wxDefaultSize */,
	long style /* = 0 */,
	const wxValidator& validator /* = wxDefaultValidator */,
	const wxString& name /* = wxTextCtrlNameStr */)
	: wxTextCtrl(parent, id, value, pos, size, style, validator, name)
	, m_time(0.0)
{
}

void wxTimeTextCtrl::OnKillFocus(wxFocusEvent& event)
{
	wxString value = GetValue().Trim(false).Trim(true);
	int minutesSep = value.Find(":");
	if(-1 != minutesSep)
	{
		double minutes;
		value.SubString(0, minutesSep).Trim(false).Trim(true).ToDouble(&minutes);
		value.SubString(minutesSep+1, value.Length()).Trim(false).Trim(true).ToDouble(&m_time);
		m_time += minutes * 60.0;
	}
	else
		value.Trim(false).Trim(true).ToDouble(&m_time);

	int nDecimalPlaces = 0;
	ChangeValue(wxString::Format(wxString::Format("%d:%%0%d.%df", (int)(m_time/60), nDecimalPlaces == 0 ? 2 : (nDecimalPlaces + 3), nDecimalPlaces), m_time - 60*(int)(m_time/60)));
}
