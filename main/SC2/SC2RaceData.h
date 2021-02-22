#pragma once

#include "../Core/Vector.h"

#include "SC2Defines.h"
#include "SC2Race.h"
#include "SC2Building.h"
#include "SC2BuildingStatus.h"
#include "SC2Unit.h"
#include "SC2Research.h"

#include <wx/xml/xml.h>

class CSC2RaceData
{
public:
	CSC2RaceData();
	~CSC2RaceData();

	bool LoadBuildingStatusesXML(const wxXmlNode *xmlBuildingStatuses);
	bool LoadBuildingXML(const wxXmlNode *xmlBuildings);
	bool LoadUnitXML(const wxXmlNode *xmlUnits);
	bool LoadResearchXML(const wxXmlNode *xmlResearch);

	bool ResolveIDs();

	CVector<CSC2Building *> m_buildings;
	CVector<CSC2BuildingStatus *> m_buildingStatuses;
	CVector<CSC2Unit *> m_units;
	CVector<CSC2Research *> m_research;
	bool m_hasLarvae;

protected:
	std::vector<wxString> m_buildingNames;
	std::vector<wxString> m_buildingStatusNames;
	std::vector<wxString> m_unitNames;
	std::vector<wxString> m_researchNames;
};
