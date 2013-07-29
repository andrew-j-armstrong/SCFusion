#ifndef TIMEVALIDATOR_H
#define TIMEVALIDATOR_H

#include <wx/validate.h>

class CTimeValidator : public wxValidator
{
public:
	CTimeValidator(double &value, int decimalPlaces = 0) : wxValidator(), m_value(value), m_decimalPlaces(decimalPlaces) {}
	CTimeValidator(const CTimeValidator &validator) : wxValidator(), m_value(validator.GetValue()), m_decimalPlaces(validator.GetDecimalPlaces()) { Copy(validator); }
	~CTimeValidator() {}

	wxObject *Clone() const { return new CTimeValidator(*this); }
	bool TransferFromWindow();
	bool TransferToWindow();
	bool Validate(wxWindow *parent);

	double &GetValue() const { return m_value; }
	int GetDecimalPlaces() const { return m_decimalPlaces; }

protected:
	double &m_value;
	int m_decimalPlaces;
};

#endif
