#pragma once

#include <wx/xml/xml.h>

#include "../Core/Vector.h"

class CSC2Research
{
public:
	CSC2Research();
	~CSC2Research() {}

	bool LoadXML(const wxXmlNode *xmlRace);

	const wxString &GetName() const { return m_name; }

	bool ResolveIDs(const std::vector<wxString> &researchNames);

protected:
	wxString m_name;
	std::vector<wxString> m_deprecateResearch;
	CVector<size_t> m_deprecateResearchIDs;
};
