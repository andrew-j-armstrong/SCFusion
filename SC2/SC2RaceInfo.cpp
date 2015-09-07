#include "stdafx.h"
#include "SC2RaceInfo.h"

CSC2RaceInfo::CSC2RaceInfo()
	: m_name()
	, m_data()
	, m_allCommands()
	, m_xmlCommands()
	, m_buildingPrerequisitCommands()
	, m_unitPrerequisitCommands()
	, m_researchPrerequisitCommands()
	, m_buildGeyserBuildingCommands()
	, m_moveWorkersToMineralsCommands()
	, m_moveWorkersToGasCommands()
	, m_requiredCommands()
{
}

CSC2RaceInfo::~CSC2RaceInfo()
{
	RemoveAllPointer(m_xmlCommands);
	RemoveAllPointer(m_buildingPrerequisitCommands);
	for(size_t i=0; i < m_buildingStatusPrerequisitCommands.size(); i++)
	{
		if(m_buildingStatusPrerequisitCommands[i])
		{
			RemoveAllPointer(*m_buildingStatusPrerequisitCommands[i]);
		}
	}
	RemoveAllPointer(m_buildingStatusPrerequisitCommands);
	RemoveAllPointer(m_unitPrerequisitCommands);
	RemoveAllPointer(m_researchPrerequisitCommands);
}

bool CSC2RaceInfo::LoadXML(const wxXmlNode *xmlRace)
{
	for(wxXmlNode *child = xmlRace->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Name"))
		{
			m_name = child->GetChildren()->GetContent();
		}
		else if (child->GetName() == wxT("BuildingStatuses"))
		{
			if(!m_data.LoadBuildingStatusesXML(child))
				return false;
		}
		else if (child->GetName() == wxT("Buildings"))
		{
			if(!m_data.LoadBuildingXML(child))
				return false;
		}
		else if (child->GetName() == wxT("Units"))
		{
			if(!m_data.LoadUnitXML(child))
				return false;
		}
		else if (child->GetName() == wxT("Research"))
		{
			if(!m_data.LoadResearchXML(child))
				return false;
		}
		else if (child->GetName() == wxT("Commands"))
		{
			if(!LoadCommandXML(child))
				return false;
		}
		else if (child->GetName() == wxT("RequiredCommands"))
		{
			if(!LoadRequiredCommandXML(child))
				return false;
		}
		else if (child->GetName() == wxT("Targets"))
		{
			if(!LoadTargets(child))
				return false;
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Race>: '%s'", child->GetName()));
			return false;
		}
	}

	if(!m_data.ResolveIDs())
		return false;

	for(size_t i=0; i < m_data.m_buildings.size(); i++)
	{
		m_buildingPrerequisitCommands.push_back(new CVector<const CSC2Command *>());

		CVector<CVector<const CSC2Command *> *> *buildingStatusList = new CVector<CVector<const CSC2Command *> *>();
		for(size_t j=0; j < m_data.m_buildingStatuses.size(); j++)
			buildingStatusList->push_back(new CVector<const CSC2Command *>());

		m_buildingStatusPrerequisitCommands.push_back(buildingStatusList);
	}
	for(size_t i=0; i < m_data.m_units.size(); i++)
		m_unitPrerequisitCommands.push_back(new CVector<const CSC2Command *>());
	for(size_t i=0; i < m_data.m_research.size(); i++)
		m_researchPrerequisitCommands.push_back(new CVector<const CSC2Command *>());

	for(size_t i=0; i < m_xmlCommands.size(); i++)
	{
		if(!m_xmlCommands[i]->ResolveIDs(m_data, m_allCommands))
			return false;

		if(m_xmlCommands[i]->AddToPrerequisitCommands(m_xmlCommands[i], m_buildingPrerequisitCommands, m_buildingStatusPrerequisitCommands, m_unitPrerequisitCommands, m_researchPrerequisitCommands))
		{
			if(m_xmlCommands[i]->WillBuildGeyserBuilding())
				m_buildGeyserBuildingCommands.push_back(m_xmlCommands[i]);
		}

		if(m_xmlCommands[i]->IsMacroAbility((SC2BuildingFlags)-1, (SC2UnitFlags)-1, (SC2ResearchFlags)-1))
			m_macroAbilityCommands.push_back(m_xmlCommands[i]);
		if(m_xmlCommands[i]->GetProvidedSupply() > 0)
			m_provideSupplyCommands.push_back(m_xmlCommands[i]);
	}

	for(size_t i=0; i < m_requiredCommands.size(); i++)
	{
		if(!m_requiredCommands[i]->ResolveIDs(m_data, m_allCommands))
			return false;
	}

	for(size_t i=0; i < m_targets.size(); i++)
	{
		if(!m_targets[i]->ResolveIDs(m_data, m_allCommands))
			return false;
	}

	return true;
}

bool CSC2RaceInfo::LoadCommandXML(const wxXmlNode *xmlCommands)
{
	for(wxXmlNode *child = xmlCommands->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Command"))
		{
			CSC2XMLCommand *command;
			CSC2MoveWorkersCommand *moveWorkersCommand = NULL;
			wxString commandCategory = child->GetAttribute(wxT("category"));
			if(commandCategory == "MoveWorkers")
			{
				moveWorkersCommand = new CSC2MoveWorkersCommand(m_data);
				command = moveWorkersCommand;
			}
			else if(commandCategory == "BuildBuilding")
				command = new CSC2BuildBuildingCommand(m_data);
			else if(commandCategory == "BuildingAbility")
				command = new CSC2BuildingAbilityCommand(m_data);
			else if(commandCategory == "CancelBuilding")
				command = new CSC2CancelBuildingCommand(m_data);
			else if(commandCategory == "BuildUnit")
				command = new CSC2BuildUnitCommand(m_data);
			else if(commandCategory == "UnitAbility")
				command = new CSC2UnitAbilityCommand(m_data);
			else if(commandCategory == "Research")
				command = new CSC2ResearchCommand(m_data);
			else if(commandCategory == "Sequence")
				command = new CSC2SequenceCommand(m_data);
			else if(commandCategory == "Multi")
				command = new CSC2MultiCommand(m_data);
			else
			{
				wxFAIL_MSG(wxString::Format("Unexpected command category: '%s'", commandCategory));
				return false;
			}

			if(!command->LoadXML(child))
			{
				delete command;
				return false;
			}
			
			m_allCommands.push_back(command);
			m_xmlCommands.push_back(command);

			if(commandCategory == "MoveWorkers")
			{
				CVector<const CSC2Command *> *commandList = NULL;
				if(moveWorkersCommand->GetMoveToGas())
					commandList = &m_moveWorkersToGasCommands;
				else
					commandList = &m_moveWorkersToMineralsCommands;

				while(commandList->size() < moveWorkersCommand->GetMoveWorkerCount() + 1)
					commandList->push_back(NULL);

				(*commandList)[moveWorkersCommand->GetMoveWorkerCount()] = command;
			}
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Commands>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RaceInfo::LoadRequiredCommandXML(const wxXmlNode *xmlCommands)
{
	for(wxXmlNode *child = xmlCommands->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("RequiredCommand"))
		{
			CSC2RequiredCommand *requiredCommand = new CSC2RequiredCommand();

			if(!requiredCommand->LoadXML(child))
			{
				delete requiredCommand;
				return false;
			}

			m_requiredCommands.push_back(requiredCommand);
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <RequiredCommands>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2RaceInfo::LoadTargets(const wxXmlNode *xmlTargets)
{
	for(wxXmlNode *child = xmlTargets->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Target"))
		{
			CSC2Target *target;
			wxString targetCategory = child->GetAttribute(wxT("category"));
			if(targetCategory == "BuildingCount")
				target = new CSC2TargetBuildingCount();
			else if(targetCategory == "BuildingAbility")
				target = new CSC2TargetBuildingAbility();
			else if(targetCategory == "BuildingStatus")
				target = new CSC2TargetBuildingStatus();
			else if(targetCategory == "BuildingStatusDuration")
				target = new CSC2TargetBuildingStatusDuration();
			else if(targetCategory == "UnitCount")
				target = new CSC2TargetUnitCount();
			else if(targetCategory == "UnitAbility")
				target = new CSC2TargetUnitAbility();
			else if(targetCategory == "Research")
				target = new CSC2TargetResearch();
			else
			{
				wxFAIL_MSG(wxString::Format("Unexpected target category: '%s'", targetCategory));
				return false;
			}

			if(!target->LoadXML(child))
			{
				delete target;
				return false;
			}

			m_targets.push_back(target);
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Targets>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

const CSC2Command *CSC2RaceInfo::GetRequiredCommand(EGasMicro gasMicro, const CSC2State &state) const
{
	if(eGasMicroThreeUponCompletion == gasMicro && 3 * state.m_geyserCount > state.m_workersOnGas + state.m_workersMovingToGas
		&& 3 < m_moveWorkersToGasCommands.size())
		return m_moveWorkersToGasCommands[3];

	for(size_t i=0; i < m_requiredCommands.size(); i++)
	{
		const CSC2RequiredCommand *requiredCommand = m_requiredCommands[i];
		if(requiredCommand->MeetsRequirements(state))
			return requiredCommand->GetCommand();
	}

	return NULL;
}
