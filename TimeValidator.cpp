#include "stdafx.h"
#include "TimeValidator.h"
#include "SC2Engine.h"

bool CTimeValidator::TransferFromWindow()
{
	wxString s;

	try
	{
		wxTextCtrl *text = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
		s = text->GetValue();
	}
	catch(...)
	{
		wxFAIL_MSG( _T("CTimeValidator works only with wxTextCtrl"));
		return false;
	}

	if(!CSC2Engine::InterpretTimeValue(s, m_value))
		return false;

	return true;
}

bool CTimeValidator::TransferToWindow()
{
	try
	{
		wxTextCtrl *text = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
		text->ChangeValue(CSC2Engine::TimeToString(m_value, m_decimalPlaces));
	}
	catch(...)
	{
		wxFAIL_MSG( _T("CTimeValidator works only with wxTextCtrl"));
		return false;
	}

	return true;
}

bool CTimeValidator::Validate(wxWindow *parent)
{
	wxString s;

	try
	{
		wxTextCtrl *text = dynamic_cast<wxTextCtrl*>(m_validatorWindow);
		s = text->GetValue();
	}
	catch(...)
	{
		wxFAIL_MSG( _T("CTimeValidator works only with wxTextCtrl"));
		return false;
	}

	double value;
	return CSC2Engine::InterpretTimeValue(s, value);
}
