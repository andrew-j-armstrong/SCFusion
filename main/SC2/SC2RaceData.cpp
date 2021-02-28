#include "stdafx.h"
#include "SC2RaceData.h"
#include "SC2Building.h"
#include "SC2Unit.h"
#include "SC2Research.h"

CSC2RaceData::CSC2RaceData()
	: m_buildings()
	, m_buildingStatuses()
	, m_units()
	, m_research()
	, m_buildingNames()
	, m_buildingStatusNames()
	, m_unitNames()
	, m_researchNames()
	, m_hasLarvae(false)
{
}

CSC2RaceData::~CSC2RaceData()
{
	RemoveAllPointer(m_buildings);
	RemoveAllPointer(m_buildingStatuses);
	RemoveAllPointer(m_units);
	RemoveAllPointer(m_research);
}

bool CSC2RaceData::LoadBuildingStatusesXML(const wxXmlNode *xmlBuildingStatuses)
{
	for(wxXmlNode *child = xmlBuildingStatuses->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("BuildingStatus"))
		{
			CSC2BuildingStatus *buildingStatus = new CSC2BuildingStatus();
			if (!buildingStatus->LoadXML(child))
			{
				delete buildingStatus;
				return false;
			}

			m_buildingStatuses.push_back(buildingStatus);
			m_buildingStatusNames.push_back(buildingStatus->GetName());
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <BuildingStatuses>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RaceData::LoadBuildingXML(const wxXmlNode *xmlBuildings)
{
	for(wxXmlNode *child = xmlBuildings->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Building"))
		{
			CSC2Building *building = new CSC2Building();
			if(!building->LoadXML(child))
			{
				delete building;
				return false;
			}

			m_buildings.push_back(building);
			m_buildingNames.push_back(building->GetName());

			if(building->GetMaxLarvaeCount() > 0)
				m_hasLarvae = true;
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Buildings>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RaceData::LoadUnitXML(const wxXmlNode *xmlUnits)
{
	for(wxXmlNode *child = xmlUnits->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Unit"))
		{
			CSC2Unit *unit = new CSC2Unit();
			if(!unit->LoadXML(child))
			{
				delete unit;
				return false;
			}

			m_units.push_back(unit);
			m_unitNames.push_back(unit->GetName());
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Units>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RaceData::LoadResearchXML(const wxXmlNode *xmlResearch)
{
	for(wxXmlNode *child = xmlResearch->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Research"))
		{
			CSC2Research *research = new CSC2Research();
			if(!research->LoadXML(child))
			{
				delete research;
				return false;
			}

			m_research.push_back(research);
			m_researchNames.push_back(research->GetName());
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Research>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RaceData::ResolveIDs()
{
	for (size_t i = 0; i < m_buildings.size(); i++)
	{
		if (!m_buildings[i]->ResolveIDs(m_buildingStatuses))
			return false;
	}
	for (size_t i = 0; i < m_research.size(); i++)
	{
		if (!m_research[i]->ResolveIDs(m_researchNames))
			return false;
	}

	return true;
}
