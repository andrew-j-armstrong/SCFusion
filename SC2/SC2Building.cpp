#include "stdafx.h"
#include "SC2Building.h"

CSC2Building::CSC2Building()
	: m_name()
	, m_gameStartCount(0)
	, m_isBase(false)
	, m_isGeyserBuilding(false)
	, m_providedSupply(0)
	, m_startingEnergy(0.0)
	, m_maxEnergy(0.0)
	, m_energyRechargeRate(0.0)
	, m_gameStartLarvaeCount(0)
	, m_initialLarvaeCount(0) // Upon building completion
	, m_maxLarvaeSpawningCount(0)
	, m_maxLarvaeCount(0)
	, m_larvaeSpawnTime(0.0)
	, m_initialStatusNames()
	, m_initialStatus(0)
	, m_initialStatusList()
	, m_initialStatusDurations()
	, m_initialProductionBoost(1.0)
	, m_gameStartStatusNames()
	, m_gameStartStatus(0)
	, m_gameStartStatusList()
	, m_gameStartStatusDurations()
	, m_gameStartProductionBoost(1.0)
{
}

bool CSC2Building::LoadXML(const wxXmlNode *xmlBuilding)
{
	for(wxXmlNode *child = xmlBuilding->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("ProvidedSupply"))
		{
			unsigned long providedSupply;
			content.ToULong(&providedSupply);
			m_providedSupply = (size_t)providedSupply;
		}
		else if (child->GetName() == wxT("IsBase"))
		{
			if(content == "True")
				m_isBase = true;
			else
				m_isBase = false;
		}
		else if (child->GetName() == wxT("IsGeyserBuilding"))
		{
			if(content == "True")
				m_isGeyserBuilding = true;
			else
				m_isGeyserBuilding = false;
		}
		else if (child->GetName() == wxT("StartingEnergy"))
		{
			content.ToCDouble(&m_startingEnergy);
		}
		else if (child->GetName() == wxT("MaxEnergy"))
		{
			content.ToCDouble(&m_maxEnergy);
		}
		else if (child->GetName() == wxT("EnergyRechargeRate"))
		{
			content.ToCDouble(&m_energyRechargeRate);
		}
		else if (child->GetName() == wxT("GameStartCount"))
		{
			unsigned long gameStartCount;
			content.ToULong(&gameStartCount);
			m_gameStartCount = (size_t)gameStartCount;
		}
		else if (child->GetName() == wxT("GameStartLarvaeCount"))
		{
			unsigned long gameStartLarvaeCount;
			content.ToULong(&gameStartLarvaeCount);
			m_gameStartLarvaeCount = (size_t)gameStartLarvaeCount;
		}
		else if (child->GetName() == wxT("InitialLarvaeCount"))
		{
			unsigned long initialLarvaeCount;
			content.ToULong(&initialLarvaeCount);
			m_initialLarvaeCount = (size_t)initialLarvaeCount;
		}
		else if (child->GetName() == wxT("MaxLarvaeSpawningCount"))
		{
			unsigned long maxLarvaeSpawningCount;
			content.ToULong(&maxLarvaeSpawningCount);
			m_maxLarvaeSpawningCount = (size_t)maxLarvaeSpawningCount;
		}
		else if (child->GetName() == wxT("MaxLarvaeCount"))
		{
			unsigned long maxLarvaeCount;
			content.ToULong(&maxLarvaeCount);
			m_maxLarvaeCount = (size_t)maxLarvaeCount;
		}
		else if (child->GetName() == wxT("LarvaeSpawnTime"))
		{
			content.ToCDouble(&m_larvaeSpawnTime);
		}
		else if (child->GetName() == wxT("InitialStatus"))
		{
			double initialStatusDuration = 0.0;
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&initialStatusDuration);
			m_initialStatusDurations.push_back(initialStatusDuration);
			m_initialStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("GameStartStatus"))
		{
			double gameStartStatusDuration = 0.0;
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&gameStartStatusDuration);
			m_gameStartStatusDurations.push_back(gameStartStatusDuration);
			m_gameStartStatusNames.push_back(content);
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Buidling>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2Building::ResolveIDs(const CVector<CSC2BuildingStatus *> &buildingStatuses)
{
	for (size_t i = 0; i < m_initialStatusNames.size(); i++)
	{
		size_t statusID = UINT_MAX;
		for (size_t j = 0; j < buildingStatuses.size(); j++)
		{
			if (m_initialStatusNames[i] == buildingStatuses[j]->GetName())
			{
				statusID = j;
				break;
			}
		}

		wxCHECK_MSG(statusID < buildingStatuses.size(), false, wxString::Format("Undefined building state '%s'", m_initialStatusNames[i]));
		m_initialStatus |= ((SC2BuildingStatusFlags)1 << statusID);
		m_initialStatusList.push_back(statusID);
		m_initialProductionBoost *= buildingStatuses[statusID]->GetProductionBoostFactor();
	}

	for (size_t i = 0; i < m_gameStartStatusNames.size(); i++)
	{
		size_t statusID = UINT_MAX;
		for (size_t j = 0; j < buildingStatuses.size(); j++)
		{
			if (m_gameStartStatusNames[i] == buildingStatuses[j]->GetName())
			{
				statusID = j;
				break;
			}
		}

		wxCHECK_MSG(statusID < buildingStatuses.size(), false, wxString::Format("Undefined building state '%s'", m_gameStartStatusNames[i]));
		m_gameStartStatus |= ((SC2BuildingStatusFlags)1 << statusID);
		m_gameStartStatusList.push_back(statusID);
		m_gameStartProductionBoost *= buildingStatuses[statusID]->GetProductionBoostFactor();
	}

	return true;
}
