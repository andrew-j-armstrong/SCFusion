#pragma once

#include "../Core/Vector.h"

#include "SC2Defines.h"
#include "SC2Race.h"
#include "SC2Building.h"
#include "SC2Unit.h"
#include "SC2Research.h"

#include <wx/xml/xml.h>

class CSC2RaceData
{
public:
	CSC2RaceData() : m_chronoBoostStatusFlags(0), m_hasLarvae(false) {}
	~CSC2RaceData();

	bool LoadBuildingStatusesXML(const wxXmlNode *xmlBuildingStatuses);
	bool LoadBuildingXML(const wxXmlNode *xmlBuildings);
	bool LoadUnitXML(const wxXmlNode *xmlUnits);
	bool LoadResearchXML(const wxXmlNode *xmlResearch);

	bool ResolveIDs();

	std::vector<wxString> m_buildingStatusList;
	SC2BuildingStatusFlags m_chronoBoostStatusFlags;
	CVector<CSC2Building *> m_buildings;
	CVector<CSC2Unit *> m_units;
	CVector<CSC2Research *> m_research;
	bool m_hasLarvae;

protected:
	bool LoadBuildingStatusXML(const wxXmlNode *xmlBuildingStatus);

	std::vector<wxString> m_buildingNames;
	std::vector<wxString> m_unitNames;
	std::vector<wxString> m_researchNames;
};
