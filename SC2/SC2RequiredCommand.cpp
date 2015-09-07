#include "stdafx.h"
#include "SC2RequiredCommand.h"

CSC2RequiredCommand::CSC2RequiredCommand()
	: m_buildingRequirementNames()
	, m_buildingRequirements(0)
	, m_unitRequirementNames()
	, m_unitRequirements(0)
	, m_researchRequirementNames()
	, m_researchRequirements(0)
	, m_buildingNames()
	, m_buildingIDs()
	, m_buildingRange()
	, m_unitNames()
	, m_unitIDs()
	, m_unitRange()
	, m_commandName()
	, m_command(NULL)
{
}

CSC2RequiredCommand::~CSC2RequiredCommand()
{
}

bool CSC2RequiredCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("BuildingRequirement"))
		{
			m_buildingRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("UnitRequirement"))
		{
			m_unitRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("ResearchRequirement"))
		{
			m_researchRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("BuildingCount"))
		{
			m_buildingNames.push_back(content);

			CTargetOptionalMinMax<size_t> minMax;
			wxString strValue;
			unsigned long value = 0;
			if(child->GetAttribute("minimum", &strValue) && strValue.ToULong(&value))
			{
				minMax.haveMin = true;
				minMax.min = (size_t)value;
			}
			if(child->GetAttribute("maximum", &strValue) && strValue.ToULong(&value))
			{
				minMax.haveMax = true;
				minMax.max = (size_t)value;
			}

			m_buildingRange.push_back(minMax);
		}
		else if (child->GetName() == wxT("UnitCount"))
		{
			m_unitNames.push_back(content);

			CTargetOptionalMinMax<size_t> minMax;
			wxString strValue;
			unsigned long value = 0;
			if(child->GetAttribute("minimum", &strValue) && strValue.ToULong(&value))
			{
				minMax.haveMin = true;
				minMax.min = (size_t)value;
			}
			if(child->GetAttribute("maximum", &strValue) && strValue.ToULong(&value))
			{
				minMax.haveMax = true;
				minMax.max = (size_t)value;
			}

			m_unitRange.push_back(minMax);
		}
		else if (child->GetName() == wxT("Command"))
		{
			m_commandName = content;
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected required command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RequiredCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_buildingRequirementNames.size(); i++)
	{
		const wxString &buildingName = m_buildingRequirementNames[i];
		for(size_t j=0; j < raceData.m_buildings.size(); j++)
		{
			if(buildingName == raceData.m_buildings[j]->GetName())
			{
				m_buildingRequirements |= ((SC2BuildingFlags)1 << j);
				break;
			}

			if(j >= raceData.m_buildings.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined building '%s'", buildingName));
				return false;
			}
		}
	}

	for(size_t i=0; i < m_unitRequirementNames.size(); i++)
	{
		const wxString &unitName = m_unitRequirementNames[i];
		for(size_t j=0; j < raceData.m_units.size(); j++)
		{
			if(unitName == raceData.m_units[j]->GetName())
			{
				m_unitRequirements |= ((SC2UnitFlags)1 << j);
				break;
			}

			if(j >= raceData.m_units.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined unit '%s'", unitName));
				return false;
			}
		}
	}

	for(size_t i=0; i < m_researchRequirementNames.size(); i++)
	{
		const wxString &researchName = m_researchRequirementNames[i];
		for(size_t j=0; j < raceData.m_research.size(); j++)
		{
			if(researchName == raceData.m_research[j]->GetName())
			{
				m_researchRequirements |= ((SC2ResearchFlags)1 << j);
				break;
			}

			if(j >= raceData.m_research.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined research '%s'", researchName));
				return false;
			}
		}
	}

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

			if(j >= raceData.m_buildings.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined building '%s'", buildingName));
				return false;
			}
		}
	}

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

			if(j >= raceData.m_units.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined unit '%s'", unitName));
				return false;
			}
		}
	}

	if(!m_commandName.IsEmpty())
	{
		for(size_t i=0; i < commands.size(); i++)
		{
			if(m_commandName == commands[i]->GetName())
			{
				m_command = commands[i];
				break;
			}
		}

		if(NULL == m_command)
		{
			wxMessageBox(wxString::Format("Undefined command '%s'", m_commandName));
			return false;
		}
	}

	return true;
}

bool CSC2RequiredCommand::MeetsRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_unitRequirements != (m_unitRequirements & state.m_unitCompletedFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchCompletedFlags))
		return false;

	for(size_t i=0; i < m_buildingIDs.size(); i++)
	{
		if(!m_buildingRange[i].IsWithinRange(state.m_buildings[m_buildingIDs[i]]->size()))
			return false;
	}
	for(size_t i=0; i < m_unitIDs.size(); i++)
	{
		if(!m_unitRange[i].IsWithinRange(state.m_units[m_unitIDs[i]]->size()))
			return false;
	}

	return true;
}
