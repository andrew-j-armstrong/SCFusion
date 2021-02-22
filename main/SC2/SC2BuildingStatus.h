#pragma once

#include <wx/xml/xml.h>

#include "../Core/Vector.h"

class CSC2BuildingStatus
{
public:
	CSC2BuildingStatus();
	~CSC2BuildingStatus() {}

	bool LoadXML(const wxXmlNode *xmlRace);

	const wxString &GetName() const { return m_name; }
	double GetProductionBoostFactor() const { return m_productionBoostFactor; }

protected:
	wxString m_name;
	double m_productionBoostFactor;
};
