#include "stdafx.h"
#include "SC2Target.h"


CSC2TargetBuildingCount::CSC2TargetBuildingCount()
{
}

bool CSC2TargetBuildingCount::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AddBuildingCount"))
		{
			m_buildingNames.push_back(content);
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'BuildingCount' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetBuildingCount::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_buildingNames.size(); i++)
	{
		const wxString &buildingName = m_buildingNames[i];
		for(size_t j=0; j < raceData.m_buildings.size(); j++)
		{
			if(buildingName == raceData.m_buildings[j]->GetName())
			{
				m_buildingIDs.push_back(j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_buildings.size() - 1, false, wxString::Format("Undefined building '%s'", buildingName));
		}
	}

	return true;
}

size_t CSC2TargetBuildingCount::GetStateCount(const CSC2State &state) const
{
	size_t count = 0;
	for(size_t i=0; i < m_buildingIDs.size(); i++)
	{
		count += state.m_buildings[m_buildingIDs[i]]->size();
	}

	return count;
}

size_t CSC2TargetBuildingCount::GetStateCountUnderConstruction(const CSC2State &state) const
{
	size_t count = 0;
	for(size_t i=0; i < m_buildingIDs.size(); i++)
	{
		count += state.m_buildingUnderConstruction[m_buildingIDs[i]];
	}

	return count;
}

CSC2TargetBuildingAbility::CSC2TargetBuildingAbility()
	: m_sourceBuildingID(0)
	, m_requiredEnergy(0.0)
{
}

bool CSC2TargetBuildingAbility::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("SourceBuilding"))
		{
			m_sourceBuildingName = content;
		}
		else if (child->GetName() == wxT("EnergyRequired"))
		{
			content.ToCDouble(&m_requiredEnergy);
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'BuildingAbility' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetBuildingAbility::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t j=0; j < raceData.m_buildings.size(); j++)
	{
		if(m_sourceBuildingName == raceData.m_buildings[j]->GetName())
		{
			m_sourceBuildingID = j;
			break;
		}

		wxCHECK_MSG(j != raceData.m_buildings.size() - 1, false, wxString::Format("Undefined building '%s'", m_sourceBuildingName));
	}

	return true;
}

size_t CSC2TargetBuildingAbility::GetStateCount(const CSC2State &state) const
{
	size_t count = 0;
	const CSC2State::CBuildingStateList *buildingStates = state.m_buildings[m_sourceBuildingID];
	for(size_t i=0; i < buildingStates->size(); i++)
	{
		count += (size_t)(buildingStates->CalculateEnergy(i, state.m_time) / m_requiredEnergy);
	}

	return count;
}

size_t CSC2TargetBuildingAbility::GetStateCountUnderConstruction(const CSC2State &state) const
{
	const CSC2State::CBuildingStateList *buildingStates = state.m_buildings[m_sourceBuildingID];
	return buildingStates->size() * (size_t)(buildingStates->m_building.GetMaxEnergy() / m_requiredEnergy);
}

CSC2TargetBuildingStatus::CSC2TargetBuildingStatus()
{
}

bool CSC2TargetBuildingStatus::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("RequireBuildingStatus"))
		{
			m_buildingStatusNames.push_back(child->GetAttribute("status"));
			m_buildingNames.push_back(content);
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'BuildingStatus' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetBuildingStatus::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_buildingNames.size(); i++)
	{
		for(size_t j=0; j < raceData.m_buildings.size(); j++)
		{
			if(m_buildingNames[i] == raceData.m_buildings[j]->GetName())
			{
				m_buildingIDs.push_back(j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_buildings.size() - 1, false, wxString::Format("Undefined building '%s'", m_buildingNames[i]));
		}
	}

	for(size_t i=0; i < m_buildingStatusNames.size(); i++)
	{
		for(size_t j=0; j < raceData.m_buildingStatuses.size(); j++)
		{
			if (m_buildingStatusNames[i] == raceData.m_buildingStatuses[j]->GetName())
			{
				m_buildingStatusFlags.push_back((SC2BuildingStatusFlags)1 << j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_buildingStatuses.size() - 1, false, wxString::Format("Undefined building status '%s'", m_buildingStatusNames[i]));
		}
	}

	return true;
}

size_t CSC2TargetBuildingStatus::GetStateCount(const CSC2State &state) const
{
	size_t count = 0;
	for(size_t i=0; i < m_buildingIDs.size(); i++)
	{
		CSC2State::CBuildingStateList &buildingStateList = *state.m_buildings[m_buildingIDs[i]];
		for(size_t j=0; j < buildingStateList.size(); j++)
		{
			if(m_buildingStatusFlags[i] == (m_buildingStatusFlags[i] & buildingStateList[j]->status))
				count++;
		}
	}

	return count;
}

SC2BuildingStatusFlags CSC2TargetBuildingStatus::GetBuildingStatusRequirementFlags(size_t buildingID) const
{
	for(size_t i=0; i < m_buildingIDs.size(); i++)
	{
		if(m_buildingIDs[i] == buildingID)
			return m_buildingStatusFlags[i];
	}

	return 0;
}

CSC2TargetBuildingStatusDuration::CSC2TargetBuildingStatusDuration()
{
}

bool CSC2TargetBuildingStatusDuration::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("RequireBuildingStatus"))
		{
			m_buildingStatusNames.push_back(child->GetAttribute("status"));
			m_buildingNames.push_back(content);
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'BuildingStatus' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetBuildingStatusDuration::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_buildingNames.size(); i++)
	{
		for(size_t j=0; j < raceData.m_buildings.size(); j++)
		{
			if(m_buildingNames[i] == raceData.m_buildings[j]->GetName())
			{
				m_buildingIDs.push_back(j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_buildings.size() - 1, false, wxString::Format("Undefined building '%s'", m_buildingNames[i]));
		}
	}

	for(size_t i=0; i < m_buildingStatusNames.size(); i++)
	{
		for (size_t j = 0; j < raceData.m_buildingStatuses.size(); j++)
		{
			if (m_buildingStatusNames[i] == raceData.m_buildingStatuses[j]->GetName())
			{
				m_buildingStatusFlags.push_back((SC2BuildingStatusFlags)1 << j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_buildingStatuses.size() - 1, false, wxString::Format("Undefined building status '%s'", m_buildingStatusNames[i]));
		}
	}

	return true;
}

double CSC2TargetBuildingStatusDuration::GetStateDurationPenalty(const CSC2State &state, double time) const
{
	double penalty = 0.0;
	for(size_t i=0; i < m_buildingIDs.size(); i++)
	{
		CSC2State::CBuildingStateList &buildingStateList = *state.m_buildings[m_buildingIDs[i]];
		for(size_t j=0; j < buildingStateList.size(); j++)
		{
			if(0 == (m_buildingStatusFlags[i] & buildingStateList[j]->status))
				penalty += time;
		}
	}

	return penalty / 1000.0;
}

CSC2TargetUnitCount::CSC2TargetUnitCount()
	: m_addTotalLarvae(false)
{
}

bool CSC2TargetUnitCount::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AddUnitCount"))
		{
			m_unitNames.push_back(content);
		}
		else if(child->GetName() == wxT("AddTotalLarvae"))
		{
			m_addTotalLarvae = true;
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'UnitCount' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetUnitCount::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_unitNames.size(); i++)
	{
		const wxString &unitName = m_unitNames[i];
		for(size_t j=0; j < raceData.m_units.size(); j++)
		{
			if(unitName == raceData.m_units[j]->GetName())
			{
				m_unitIDs.push_back(j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_units.size() - 1, false, wxString::Format("Undefined unit '%s'", unitName));
		}
	}

	return true;
}

size_t CSC2TargetUnitCount::GetStateCount(const CSC2State &state) const
{
	size_t count = 0;
	if(m_addTotalLarvae)
		count += state.m_totalLarvaeCount;
	for(size_t i=0; i < m_unitIDs.size(); i++)
	{
		count += state.m_units[m_unitIDs[i]]->size();
	}

	return count;
}

size_t CSC2TargetUnitCount::GetStateCountUnderConstruction(const CSC2State &state) const
{
	size_t count = 0;
	for(size_t i=0; i < m_unitIDs.size(); i++)
	{
		count += state.m_unitUnderConstruction[m_unitIDs[i]];
	}

	return count;
}

CSC2TargetUnitAbility::CSC2TargetUnitAbility()
	: m_sourceUnitIDs()
	, m_sourceUnitNames()
	, m_requiredEnergy()
	, m_requiredUnitFlags(0)
{
}

bool CSC2TargetUnitAbility::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("SourceUnit"))
		{
			double energyRequirement = 0.0;
			child->GetAttribute("energy").ToCDouble(&energyRequirement);
			m_sourceUnitNames.push_back(content);
			m_requiredEnergy.push_back(energyRequirement);
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'UnitAbility' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetUnitAbility::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for (size_t i = 0; i < m_sourceUnitNames.size(); i++)
	{
		size_t sourceUnitID = 0;
		for (size_t j = 0; j < raceData.m_units.size(); j++)
		{
			if (m_sourceUnitNames[i] == raceData.m_units[j]->GetName())
			{
				sourceUnitID = j;
				break;
			}

			wxCHECK_MSG(j != raceData.m_units.size() - 1, false, wxString::Format("Undefined unit '%s'", m_sourceUnitNames[i]));
		}

		m_sourceUnitIDs.push_back(sourceUnitID);
	}

	if (m_sourceUnitIDs.size() == 1)
		m_requiredUnitFlags |= (SC2UnitFlags)(1 << m_sourceUnitIDs[0]);

	return true;
}

size_t CSC2TargetUnitAbility::GetStateCount(const CSC2State &state) const
{
	size_t count = 0;
	for (size_t i = 0; i < m_sourceUnitIDs.size(); i++)
	{
		const CSC2State::CUnitStateList *unitStates = state.m_units[m_sourceUnitIDs[i]];
		for (size_t j = 0; j < unitStates->size(); j++)
		{
			count += (size_t)(unitStates->CalculateEnergy(j, state.m_time) / m_requiredEnergy[i]);
		}
	}

	return count;
}

size_t CSC2TargetUnitAbility::GetStateCountUnderConstruction(const CSC2State &state) const
{
	size_t count = 0;
	for (size_t i = 0; i < m_sourceUnitIDs.size(); i++)
	{
		const CSC2State::CUnitStateList *unitStates = state.m_units[m_sourceUnitIDs[i]];
		for (size_t j = 0; j < unitStates->size(); j++)
		{
			count += unitStates->size() * (size_t)(unitStates->m_unit.GetMaxEnergy() / m_requiredEnergy[i]);
		}
	}
	return count;
}

CSC2TargetResearch::CSC2TargetResearch()
	: m_flags(0)
{
}

bool CSC2TargetResearch::LoadXML(const wxXmlNode *xmlTarget)
{
	for(wxXmlNode *child = xmlTarget->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("Research"))
		{
			m_researchNames.push_back(content);
		}
		else if (child->GetName() == wxT("DisplayInResults"))
		{
			m_displayInResults = (content == wxT("True"));
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected 'Research' target tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2TargetResearch::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_researchNames.size(); i++)
	{
		const wxString &researchName = m_researchNames[i];
		for(size_t j=0; j < raceData.m_research.size(); j++)
		{
			if(researchName == raceData.m_research[j]->GetName())
			{
				m_researchIDs.push_back(j);
				m_flags |= ((SC2ResearchFlags)1 << j);
				break;
			}

			wxCHECK_MSG(j != raceData.m_research.size() - 1, false, wxString::Format("Undefined research '%s'", researchName));
		}
	}

	return true;
}

size_t CSC2TargetResearch::GetStateCount(const CSC2State &state) const
{
	return (state.m_researchCompletedFlags & m_flags) == m_flags ? 1 : 0;
}

size_t CSC2TargetResearch::GetStateCountUnderConstruction(const CSC2State &state) const
{
	for(size_t i=0; i < m_researchIDs.size(); i++)
	{
		if(!state.m_researchUnderConstruction[m_researchIDs[i]])
			return 0;
	}

	return 1;
}
