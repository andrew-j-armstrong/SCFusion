#include "stdafx.h"
#include "SC2Command.h"
#include "GameCalcs.h"

CSC2Command::CSC2Command(const CSC2RaceData &raceData)
	: m_raceData(raceData)
	, m_name()
	, m_isAvailableToGeneticAlgorithm(true)
	, m_isAutoCastAbility(false)
{
}

const CSC2Command *CSC2Command::GetPrerequisitCommand(
	const CSC2State &state
	, EGasMicro gasMicro
	, const CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands
	, const CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands
	, const CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands
	, const CVector<const CSC2Command *> &buildGeyserBuildingCommands
	, const CVector<const CSC2Command *> &moveWorkersToMineralsCommands
	, const CVector<const CSC2Command *> &moveWorkersToGasCommands
	, const CVector<const CSC2Command *> &provideSupplyCommands) const
{
	if(GetRequiredSupply() > 0 && state.m_supply + GetRequiredSupply() > state.m_supplyCap)
	{
		if(provideSupplyCommands.size() > 0)
			return provideSupplyCommands[rand_sse() % provideSupplyCommands.size()];
	}

	if(GetGasCost() > 0.0 && 0.0 == state.m_gasIncomeRate && 0 == state.m_workersMovingToGas)
	{
		switch(gasMicro)
		{
		case eGasMicroOneBackAndForth:
		case eGasMicroOneToGasOnly:
			if(moveWorkersToGasCommands.size() > 1 && 1 < state.m_workersOnMinerals + state.m_workersMovingToMinerals)
				return moveWorkersToGasCommands[1];
			break;
		case eGasMicroThreeBackAndForth:
		case eGasMicroThreeToGasOnly:
		case eGasMicroThreeUponCompletion:
			if(moveWorkersToGasCommands.size() > 3 && 3 < state.m_workersOnMinerals + state.m_workersMovingToMinerals)
				return moveWorkersToGasCommands[3];
			break;
		}
	}

	if(GetMineralCost() > 0.0 && 0.0 == state.m_mineralIncomeRate && 0 == state.m_workersMovingToMinerals)
	{
		switch(gasMicro)
		{
		case eGasMicroOneBackAndForth:
			if(moveWorkersToMineralsCommands.size() > 1)
				return moveWorkersToMineralsCommands[1];
			break;
		case eGasMicroThreeBackAndForth:
			if(moveWorkersToMineralsCommands.size() > 3)
				return moveWorkersToMineralsCommands[3];
			break;
		default:
			break;
		}
	}

	if(RequiresGeyser() && 0 == state.m_geyserCount && 0 == state.m_geysersUnderConstruction)
	{
		if(buildGeyserBuildingCommands.size() > 0)
			return buildGeyserBuildingCommands[rand_sse() % buildGeyserBuildingCommands.size()];
	}

	SC2BuildingFlags requiredBuildings = GetBuildingRequirementFlags() - (state.m_buildingUnderConstructionFlags & GetBuildingRequirementFlags());
	if(requiredBuildings != 0)
	{
		int buildingID = 0;
		while((requiredBuildings & 1) == 0)
		{
			requiredBuildings >>= 1;
			buildingID++;
		}

		if(buildingPrerequisitCommands[buildingID] && buildingPrerequisitCommands[buildingID]->size() > 0)
			return (*buildingPrerequisitCommands[buildingID])[rand_sse() % buildingPrerequisitCommands[buildingID]->size()];
	}

	SC2UnitFlags requiredUnits = GetUnitRequirementFlags() - (state.m_unitUnderConstructionFlags & GetUnitRequirementFlags());
	if(requiredUnits != 0)
	{
		int unitID = 0;
		while((requiredUnits & 1) == 0)
		{
			requiredUnits >>= 1;
			unitID++;
		}

		if(unitPrerequisitCommands[unitID] && unitPrerequisitCommands[unitID]->size() > 0)
			return (*unitPrerequisitCommands[unitID])[rand_sse() % unitPrerequisitCommands[unitID]->size()];
	}

	SC2ResearchFlags requiredResearchs = GetResearchRequirementFlags() - (state.m_researchUnderConstructionFlags & GetResearchRequirementFlags());
	if(requiredResearchs != 0)
	{
		int researchID = 0;
		while((requiredResearchs & 1) == 0)
		{
			requiredResearchs >>= 1;
			researchID++;
		}

		if(researchPrerequisitCommands[researchID] && researchPrerequisitCommands[researchID]->size() > 0)
			return (*researchPrerequisitCommands[researchID])[rand_sse() % researchPrerequisitCommands[researchID]->size()];
	}

	return 0;
}

CSC2WaitCommand::CSC2WaitCommand(const CSC2RaceData &raceData, double duration)
	: CSC2Command(raceData)
	, m_duration(duration)
{
	if(duration == 1)
		m_name = wxT("Wait 1 second");
	else
		m_name = wxString::Format("Wait %g seconds", m_duration);
}

CSC2XMLCommand::CSC2XMLCommand(const CSC2RaceData &raceData)
	: CSC2Command(raceData)
{
}

CSC2MoveWorkersCommand::CSC2MoveWorkersCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirements(0)
	, m_moveToGas(false)
	, m_moveWorkerCount(0)
{
}

CSC2MoveWorkersCommand::CSC2MoveWorkersCommand(const CSC2RaceData &raceData, bool moveToGas, size_t moveWorkerCount)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirements(0)
	, m_moveToGas(moveToGas)
	, m_moveWorkerCount(moveWorkerCount)
{
}

bool CSC2MoveWorkersCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	return false;
}

bool CSC2MoveWorkersCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("BuildingRequirement"))
		{
			m_buildingRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("MoveToGas"))
		{
			if(content == "True")
				m_moveToGas = true;
			else
				m_moveToGas = false;
		}
		else if (child->GetName() == wxT("WorkerCount"))
		{
			unsigned long moveWorkerCount;
			content.ToULong(&moveWorkerCount);
			m_moveWorkerCount = (size_t)moveWorkerCount;
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Move Worker' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2MoveWorkersCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
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

	return true;
}

bool CSC2MoveWorkersCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;

	if(m_moveToGas)
	{
		if(state.m_workersOnMinerals + state.m_workersMovingToMinerals < m_moveWorkerCount
			|| state.m_workersOnGas + state.m_workersMovingToGas + m_moveWorkerCount > CGameCalcs::GasWorkerLimit2Near(state.m_baseCount + state.m_basesUnderConstruction, state.m_geyserCount + state.m_geysersUnderConstruction))
			return false;
	}
	else
	{
		if(state.m_workersOnGas + state.m_workersMovingToGas < m_moveWorkerCount)
			return false;
	}

	return true;
}

bool CSC2MoveWorkersCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;

	if(m_moveToGas)
	{
		if(state.m_workersOnMinerals < m_moveWorkerCount
			|| state.m_workersOnGas + m_moveWorkerCount > CGameCalcs::GasWorkerLimit2Near(state.m_baseCount, state.m_geyserCount))
			return false;
	}
	else
	{
		if(state.m_workersOnGas < m_moveWorkerCount)
			return false;
	}

	return true;
}

double CSC2MoveWorkersCommand::GetResourceWaitTime(const CSC2State &state) const
{
	return 0.0;
}

bool CSC2MoveWorkersCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	if(m_moveToGas)
	{
		state.m_workersOnMinerals -= m_moveWorkerCount;
		state.m_workersMovingToGas += m_moveWorkerCount;
		for(size_t i=0; i < m_moveWorkerCount; i++)
			events.add(CSC2Event(state.m_time + 2.0 * (i + 1), CSC2Event::eWorkerStartMiningGas));
		state.RecalculateMineralIncomeRate();
	}
	else
	{
		state.m_workersOnGas -= m_moveWorkerCount;
		state.m_workersMovingToMinerals += m_moveWorkerCount;
		for(size_t i=0; i < m_moveWorkerCount; i++)
			events.add(CSC2Event(state.m_time + 2.0, CSC2Event::eWorkerStartMiningMinerals));
		state.RecalculateGasIncomeRate();
	}

	return true;
}

CSC2BuildBuildingCommand::CSC2BuildBuildingCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirementNames()
	, m_buildingRequirements(0)
	, m_targetBuildingName()
	, m_targetBuildingTypeID(0)
	, m_targetBuilding(NULL)
	, m_mineralCost(0.0)
	, m_gasCost(0.0)
	, m_workerTravelTime(0.0)
	, m_buildBuildingCompletionTime(0.0)
	, m_buildBuildingName()
	, m_buildBuildingTypeID(0)
	, m_buildBuilding(NULL)
	, m_buildBuildingInitialStatus(0)
	, m_occupyWorker(false)
	, m_occupyWorkerTime(false)
	, m_consumeWorker(false)
	, m_consumeWorkerUnitID(0)
	, m_consumeTargetBuilding(false)
	, m_spawnBase(false)
	, m_spawnBaseTime(0.0)
{
}

bool CSC2BuildBuildingCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	buildingPrerequisitCommands[m_buildBuildingTypeID]->push_back(command);
	return true;
}

bool CSC2BuildBuildingCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("BuildingRequirement"))
		{
			m_buildingRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("InitialBuildingStatus"))
		{
			m_buildBuildingInitialStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("BuildingComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_buildBuildingCompletionTime);
			m_buildBuildingName = content;
		}
		else if (child->GetName() == wxT("TargetBuilding"))
		{
			m_consumeTargetBuilding = (child->GetAttribute(wxT("consume"), wxT("False")) == wxT("True"));
			m_targetBuildingName = content;
		}
		else if (child->GetName() == wxT("WorkerTravelTime"))
		{
			content.ToCDouble(&m_workerTravelTime);
		}
		else if (child->GetName() == wxT("OccupyWorker"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_occupyWorkerTime);
			m_occupyWorker = (content == wxT("True"));
		}
		else if (child->GetName() == wxT("ConsumeWorker"))
		{
			m_consumeWorker = (content == wxT("True"));
		}
		else if (child->GetName() == wxT("SpawnBase"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_spawnBaseTime);
			m_spawnBase = (content == wxT("True"));
		}
		else if (child->GetName() == wxT("MineralCost"))
		{
			content.ToCDouble(&m_mineralCost);
		}
		else if (child->GetName() == wxT("GasCost"))
		{
			content.ToCDouble(&m_gasCost);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Build Building' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2BuildBuildingCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
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

	for(size_t i=0; i < m_buildBuildingInitialStatusNames.size(); i++)
	{
		const wxString &buildingStatusName = m_buildBuildingInitialStatusNames[i];
		for (size_t j = 0; j < raceData.m_buildingStatuses.size(); j++)
		{
			if (buildingStatusName == raceData.m_buildingStatuses[j]->GetName())
			{
				m_buildBuildingInitialStatus |= ((SC2BuildingStatusFlags)1 << j);
				break;
			}

			if (j >= raceData.m_buildingStatuses.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined building status '%s'", buildingStatusName));
				return false;
			}
		}
	}

	if(!m_targetBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_targetBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_targetBuildingTypeID = i;
				m_targetBuilding = raceData.m_buildings[i];
				break;
			}
		}
	}

	if(!m_buildBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_buildBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_buildBuildingTypeID = i;
				m_buildBuilding = raceData.m_buildings[i];
				break;
			}
		}
	}

	while(!raceData.m_units[m_consumeWorkerUnitID]->IsWorker())
		m_consumeWorkerUnitID++;

	return true;
}

bool CSC2BuildBuildingCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;
	if(m_targetBuilding && 0 == state.m_buildings[m_targetBuildingTypeID]->size() && 0 == state.m_buildingUnderConstruction[m_targetBuildingTypeID])
		return false;
	if(state.m_minerals < m_mineralCost && state.m_mineralIncomeRate == 0.0 && 0 == state.m_workersMovingToMinerals)
		return false;
	if(state.m_gas < m_gasCost && state.m_gasIncomeRate == 0.0 && 0 == state.m_workersMovingToGas)
		return false;
	if(m_consumeWorker && state.m_units[m_consumeWorkerUnitID]->size() + state.m_unitUnderConstruction[m_consumeWorkerUnitID] < 2)
		return false;

	return true;
}

bool CSC2BuildBuildingCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_targetBuilding && 0 == state.m_buildings[m_targetBuildingTypeID]->size())
		return false;
	if(m_consumeWorker && state.m_workersOnMinerals + state.m_workersOnGas < 2)
		return false;

	return true;
}

double CSC2BuildBuildingCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = 0.0;

	if(m_mineralCost > state.m_minerals)
	{
		double mineralWaitTime = (m_mineralCost - state.m_minerals) / state.m_mineralIncomeRate;
		if(mineralWaitTime > resourceWaitTime)
			resourceWaitTime = mineralWaitTime;
	}

	if(m_gasCost > state.m_gas)
	{
		double gasWaitTime = (m_gasCost - state.m_gas) / state.m_gasIncomeRate;
		if(gasWaitTime > resourceWaitTime)
			resourceWaitTime = gasWaitTime;
	}

	return resourceWaitTime;
}

bool CSC2BuildBuildingCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	state.m_minerals -= m_mineralCost;
	state.m_gas -= m_gasCost;

	if(state.m_workersOnMinerals > 0)
	{
		events.add(CSC2Event(state.m_time + m_workerTravelTime, CSC2Event::eWorkerStartMiningMinerals));
		state.m_workersMovingToMinerals++;
		state.m_workersOnMinerals--;
		state.RecalculateMineralIncomeRate();
	}
	else if(state.m_workersOnGas > 0)
	{
		events.add(CSC2Event(state.m_time + m_workerTravelTime, CSC2Event::eWorkerStartMiningGas));
		state.m_workersMovingToGas++;
		state.m_workersOnGas--;
		state.RecalculateGasIncomeRate();
	}

	if(m_consumeTargetBuilding)
	{
		CSC2State::CBuildingStateList &targetBuildingList = *state.m_buildings[m_targetBuildingTypeID];

		// Use the one with the least energy
		size_t targetBuildingIndex = 0;
		if(0.0 < m_targetBuilding->GetMaxEnergy())
		{
			double bestMaxEnergyTime = targetBuildingList[0]->maxEnergyTime;
			for(size_t i=1; i < targetBuildingList.size(); i++)
			{
				if(targetBuildingList[i]->maxEnergyTime > bestMaxEnergyTime)
				{
					bestMaxEnergyTime = targetBuildingList[i]->maxEnergyTime;
					targetBuildingIndex = i;
				}
			}
		}
		else
			targetBuildingIndex = targetBuildingList.size() - 1;

		state.m_allBuildings[targetBuildingList[targetBuildingIndex]->buildingID] = NULL;
		delete targetBuildingList[targetBuildingIndex];
		targetBuildingList.erase(targetBuildingIndex);
	}

	if(m_consumeWorker)
	{
		state.m_workerCount--;
		CSC2State::SUnitState *unit = state.m_units[m_consumeWorkerUnitID]->m_unitList.pop();
		state.m_allUnits[unit->unitID] = NULL;
		state.m_supply -= m_raceData.m_units[m_consumeWorkerUnitID]->GetSupplyCost();
		delete unit;

		if(state.m_workersOnMinerals > 0)
		{
			state.m_workersOnMinerals--;
			state.RecalculateMineralIncomeRate();
		}
		else if(state.m_workersOnGas > 0)
		{
			state.m_workersOnGas--;
			state.RecalculateGasIncomeRate();
		}
	}
	else if(m_occupyWorker)
	{
		if(state.m_workersOnMinerals > 0)
		{
			events.add(CSC2Event(state.m_time + m_occupyWorkerTime, CSC2Event::eWorkerStartMiningMinerals));
			state.m_workersOnMinerals--;
			state.m_workersMovingToMinerals++;
			state.RecalculateMineralIncomeRate();
		}
		else if(state.m_workersOnGas > 0)
		{
			events.add(CSC2Event(state.m_time + m_occupyWorkerTime, CSC2Event::eWorkerStartMiningGas));
			state.m_workersOnGas--;
			state.m_workersMovingToGas++;
			state.RecalculateGasIncomeRate();
		}
	}

	events.add(CSC2Event(state.m_time + m_buildBuildingCompletionTime, CSC2Event::eBuildingComplete, false, 0, m_buildBuildingTypeID, m_buildBuildingInitialStatus, state.m_time));
	state.m_buildingUnderConstructionFlags |= ((SC2BuildingFlags)1 << m_buildBuildingTypeID);
	state.m_buildingUnderConstruction[m_buildBuildingTypeID]++;
	state.m_supplyCapUnderConstruction += m_buildBuilding->GetProvidedSupply();
	if(m_spawnBase)
	{
		events.add(CSC2Event(state.m_time + m_spawnBaseTime, CSC2Event::eSpawnBase));
		state.m_basesUnderConstruction++;
	}
	if(m_buildBuilding->IsGeyserBuilding())
		state.m_geysersUnderConstruction++;

	return true;
}

CSC2BuildingAbilityCommand::CSC2BuildingAbilityCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirementNames()
	, m_buildingRequirements(0)
	, m_researchRequirementNames()
	, m_researchRequirements(0)
	, m_sourceBuildingName()
	, m_sourceBuildingTypeID(0)
	, m_sourceBuilding(NULL)
	, m_requireSourceBuildingStatusAbsentNames()
	, m_requireSourceBuildingStatusAbsent(0)
	, m_requireSourceBuildingStatusNames()
	, m_requireSourceBuildingStatus(0)
	, m_targetBuildingName()
	, m_targetBuildingTypeID(0)
	, m_targetBuilding(NULL)
	, m_canTargetSelf(false)
	, m_requireTargetBuildingStatusAbsentNames()
	, m_requireTargetBuildingStatusAbsent(0)
	, m_mineralCost(0.0)
	, m_gasCost(0.0)
	, m_energyCost(0.0)
	, m_removeSourceBuildingStatusNames()
	, m_removeSourceBuildingStatusIDs()
	, m_removeSourceBuildingStatus(0)
	, m_applySourceBuildingStatusNames()
	, m_applySourceBuildingStatusIDs()
	, m_applySourceBuildingStatus()
	, m_applySourceBuildingStatusDuration()
	, m_applySourceBuildingStatusDelay()
	, m_sourceProductionBoost(1.0)
	, m_applyTargetBuildingStatusNames()
	, m_applyTargetBuildingStatusID(0)
	, m_applyTargetBuildingStatus(0)
	, m_applyTargetBuildingStatusDuration(0.0)
	, m_targetProductionBoost(1.0)
	, m_morphSourceBuildingName()
	, m_morphSourceBuildingTypeID(0)
	, m_morphSourceBuilding(NULL)
	, m_morphSourceBuildingTime(0.0)
	, m_consumeTargetBuilding(false)
	, m_morphTargetBuildingName()
	, m_morphTargetBuildingTypeID(0)
	, m_morphTargetBuilding(NULL)
	, m_morphTargetBuildingTime(0.0)
	, m_buildBuildingName()
	, m_buildBuildingTypeID(0)
	, m_buildBuilding(NULL)
	, m_buildBuildingTime(0.0)
	, m_buildUnitName()
	, m_buildUnitTypeID(0)
	, m_buildUnit(NULL)
	, m_buildUnitTime(0.0)
	, m_spawnBase(false)
	, m_spawnBaseTime(0.0)
{
}

bool CSC2BuildingAbilityCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	bool added = false;
	if(m_morphSourceBuilding)
	{
		buildingPrerequisitCommands[m_morphSourceBuildingTypeID]->push_back(command);
		added = true;
	}
	if(m_morphTargetBuilding)
	{
		buildingPrerequisitCommands[m_morphTargetBuildingTypeID]->push_back(command);
		added = true;
	}
	if(m_buildBuilding)
	{
		buildingPrerequisitCommands[m_buildBuildingTypeID]->push_back(command);
		added = true;
	}
	if(m_buildUnit)
	{
		unitPrerequisitCommands[m_buildUnitTypeID]->push_back(command);
		added = true;
	}
	for(size_t i=0; i < m_applySourceBuildingStatus.size(); i++)
	{
		(*buildingStatusPrerequisitCommands[m_sourceBuildingTypeID])[m_applySourceBuildingStatusIDs[i]]->push_back(command);
	}
	if(m_applyTargetBuildingStatus)
	{
		(*buildingStatusPrerequisitCommands[m_targetBuildingTypeID])[m_applyTargetBuildingStatusID]->push_back(command);
	}

	return added;
}

bool CSC2BuildingAbilityCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("IsAutoCastAbility"))
		{
			m_isAutoCastAbility = (content == "True");
		}
		else if (child->GetName() == wxT("BuildingRequirement"))
		{
			m_buildingRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("ResearchRequirement"))
		{
			m_researchRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("SourceBuilding"))
		{
			child->GetAttribute(wxT("energy"), wxT("0.0")).ToCDouble(&m_energyCost);
			m_sourceBuildingName = content;
		}
		else if (child->GetName() == wxT("RequiresSourceBuildingStatus"))
		{
			m_requireSourceBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("RequiresSourceBuildingStatusAbsent"))
		{
			m_requireSourceBuildingStatusAbsentNames.push_back(content);
		}
		else if (child->GetName() == wxT("TargetBuilding"))
		{
			m_consumeTargetBuilding = (child->GetAttribute(wxT("consume"), wxT("False")) == wxT("True"));
			m_targetBuildingName = content;
		}
		else if (child->GetName() == wxT("CanTargetSelf"))
		{
			m_canTargetSelf = (content == "True");
		}
		else if (child->GetName() == wxT("RequiresTargetBuildingStatusAbsent"))
		{
			m_requireTargetBuildingStatusAbsentNames.push_back(content);
		}
		else if (child->GetName() == wxT("MorphSourceBuilding"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_morphSourceBuildingTime);
			m_morphSourceBuildingName = content;
		}
		else if (child->GetName() == wxT("MorphTargetBuilding"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_morphTargetBuildingTime);
			m_morphTargetBuildingName = content;
		}
		else if (child->GetName() == wxT("BuildingComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_buildBuildingTime);
			m_buildBuildingName = content;
		}
		else if (child->GetName() == wxT("UnitComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_buildUnitTime);
			m_buildUnitName = content;
		}
		else if (child->GetName() == wxT("RemovesSourceBuildingStatus"))
		{
			m_removeSourceBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("AppliesSourceBuildingStatus"))
		{
			double duration = 0.0, delay = 0.0;
			child->GetAttribute(wxT("duration"), wxT("0.0")).ToCDouble(&duration);
			child->GetAttribute(wxT("delay"), wxT("0.0")).ToCDouble(&delay);
			m_applySourceBuildingStatusDuration.push_back(duration);
			m_applySourceBuildingStatusDelay.push_back(delay);
			m_applySourceBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("AppliesTargetBuildingStatus"))
		{
			child->GetAttribute(wxT("duration"), wxT("0.0")).ToCDouble(&m_applyTargetBuildingStatusDuration);
			m_applyTargetBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("SpawnBase"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_spawnBaseTime);
			m_spawnBase = (content == wxT("True"));
		}
		else if (child->GetName() == wxT("MineralCost"))
		{
			content.ToCDouble(&m_mineralCost);
		}
		else if (child->GetName() == wxT("GasCost"))
		{
			content.ToCDouble(&m_gasCost);
		}
		else if (child->GetName() == wxT("EnergyCost"))
		{
			content.ToCDouble(&m_energyCost);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Building Ability' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2BuildingAbilityCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
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

	if(!m_sourceBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_sourceBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_sourceBuildingTypeID = i;
				m_sourceBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_sourceBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_sourceBuildingName));
			return false;
		}

		for (size_t i = 0; i < m_requireSourceBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_requireSourceBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_requireSourceBuildingStatusNames[i]));
				return false;
			}
			m_requireSourceBuildingStatus |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}

		for (size_t i = 0; i < m_requireSourceBuildingStatusAbsentNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_requireSourceBuildingStatusAbsentNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_requireSourceBuildingStatusAbsentNames[i]));
				return false;
			}
			m_requireSourceBuildingStatusAbsent |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}

		for (size_t i = 0; i < m_removeSourceBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_removeSourceBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_removeSourceBuildingStatusNames[i]));
				return false;
			}
			m_removeSourceBuildingStatusIDs.push_back(statusIndex);
			m_removeSourceBuildingStatus |= (SC2BuildingStatusFlags)1 << statusIndex;
			m_sourceProductionBoost /= m_raceData.m_buildingStatuses[statusIndex]->GetProductionBoostFactor();
		}

		for (size_t i = 0; i < m_applySourceBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_applySourceBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_applySourceBuildingStatusNames[i]));
				return false;
			}
			m_applySourceBuildingStatusIDs.push_back(statusIndex);
			m_applySourceBuildingStatus.push_back((SC2BuildingStatusFlags)1 << statusIndex);
			m_sourceProductionBoost *= m_raceData.m_buildingStatuses[statusIndex]->GetProductionBoostFactor();
		}
	}

	if(!m_targetBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_targetBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_targetBuildingTypeID = i;
				m_targetBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_targetBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_targetBuildingName));
			return false;
		}

		for(size_t i=0; i < m_requireTargetBuildingStatusAbsentNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for(size_t j=0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_requireTargetBuildingStatusAbsentNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_requireTargetBuildingStatusAbsentNames[i]));
				return false;
			}
			m_requireTargetBuildingStatusAbsent |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}

		for(size_t i=0; i < m_applyTargetBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_applyTargetBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_applyTargetBuildingStatusNames[i]));
				return false;
			}
			m_applyTargetBuildingStatusID = statusIndex;
			m_applyTargetBuildingStatus |= ((SC2BuildingStatusFlags)1 << statusIndex);
			m_targetProductionBoost *= m_raceData.m_buildingStatuses[statusIndex]->GetProductionBoostFactor();
		}
	}

	if(!m_morphSourceBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_morphSourceBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_morphSourceBuildingTypeID = i;
				m_morphSourceBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_morphSourceBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_morphSourceBuildingName));
			return false;
		}
	}

	if(!m_morphTargetBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_morphTargetBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_morphTargetBuildingTypeID = i;
				m_morphTargetBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_morphTargetBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_morphTargetBuildingName));
			return false;
		}
	}

	if(!m_buildBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_buildBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_buildBuildingTypeID = i;
				m_buildBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_buildBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_buildBuildingName));
			return false;
		}
	}

	if(!m_buildUnitName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_buildUnitName == raceData.m_units[i]->GetName())
			{
				m_buildUnitTypeID = i;
				m_buildUnit = raceData.m_units[i];
				break;
			}
		}
		if(NULL == m_buildUnit)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_buildUnitName));
			return false;
		}
	}

	return true;
}

bool CSC2BuildingAbilityCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchUnderConstructionFlags))
		return false;
	if(!m_canTargetSelf && NULL != m_sourceBuilding && m_sourceBuilding == m_targetBuilding)
	{
		if(2 > state.m_buildings[m_sourceBuildingTypeID]->size() + state.m_buildingUnderConstruction[m_sourceBuildingTypeID])
			return false;
	}
	else
	{
		if(NULL != m_sourceBuilding && 0 == state.m_buildings[m_sourceBuildingTypeID]->size() && 0 == state.m_buildingUnderConstruction[m_sourceBuildingTypeID])
			return false;
		if(NULL != m_targetBuilding && 0 == state.m_buildings[m_targetBuildingTypeID]->size() && 0 == state.m_buildingUnderConstruction[m_targetBuildingTypeID])
			return false;
	}
	if(state.m_minerals < m_mineralCost && state.m_mineralIncomeRate == 0.0 && 0 == state.m_workersMovingToMinerals)
		return false;
	if(state.m_gas < m_gasCost && state.m_gasIncomeRate == 0.0 && 0 == state.m_workersMovingToGas)
		return false;
	if(m_buildUnit && m_buildUnit->IsUnique() && (0 != state.m_units[m_buildUnitTypeID]->size() || 0 != state.m_unitUnderConstruction[m_buildUnitTypeID]))
		return false;

	return true;
}

bool CSC2BuildingAbilityCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchCompletedFlags))
		return false;
	if(!m_canTargetSelf && NULL != m_sourceBuilding && m_sourceBuilding == m_targetBuilding)
	{
		if(2 > state.m_buildings[m_sourceBuildingTypeID]->size())
			return false;
	}
	else
	{
		if(NULL != m_sourceBuilding && 0 == state.m_buildings[m_sourceBuildingTypeID]->size())
			return false;
		if(NULL != m_targetBuilding && 0 == state.m_buildings[m_targetBuildingTypeID]->size())
			return false;
	}
	if (0 != m_requireSourceBuildingStatus || 0 != m_requireSourceBuildingStatusAbsent)
	{
		bool availableBuilding = false;
		const CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
		for (size_t i = 0; i < sourceBuildingList.size(); i++)
		{
			if (m_requireSourceBuildingStatus != (sourceBuildingList[i]->status & m_requireSourceBuildingStatus))
				continue;
			if (0 != (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
				continue;

			availableBuilding = true;
			break;
		}
		if (!availableBuilding)
			return false;
	}
	if (0 != m_requireTargetBuildingStatusAbsent)
	{
		bool availableBuilding = false;
		const CSC2State::CBuildingStateList &targetBuildingList = *state.m_buildings[m_targetBuildingTypeID];
		for(size_t i=0; i < targetBuildingList.size(); i++)
		{
			if(0 != (targetBuildingList[i]->status & m_requireTargetBuildingStatusAbsent))
				continue;

			availableBuilding = true;
			break;
		}
		if(!availableBuilding)
			return false;
	}

	return true;
}

double CSC2BuildingAbilityCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = 0.0;

	if(m_mineralCost > state.m_minerals)
	{
		double mineralWaitTime = (m_mineralCost - state.m_minerals) / state.m_mineralIncomeRate;
		if(mineralWaitTime > resourceWaitTime)
			resourceWaitTime = mineralWaitTime;
	}

	if(m_gasCost > state.m_gas)
	{
		double gasWaitTime = (m_gasCost - state.m_gas) / state.m_gasIncomeRate;
		if(gasWaitTime > resourceWaitTime)
			resourceWaitTime = gasWaitTime;
	}

	if(m_energyCost > 0)
	{
		double minimumEnergyWaitTime = DBL_MAX;
		const CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			if(0 != (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
				continue;

			double energy = sourceBuildingList.CalculateEnergy(i, state.m_time);
			if(m_energyCost <= energy)
			{
				minimumEnergyWaitTime = 0.0;
				break;
			}

			double energyWaitTime = (m_energyCost - energy) / m_sourceBuilding->GetEnergyRechargeRate();
			if(energyWaitTime < minimumEnergyWaitTime)
				minimumEnergyWaitTime = energyWaitTime;
		}

		if(minimumEnergyWaitTime > resourceWaitTime)
			resourceWaitTime = minimumEnergyWaitTime;
	}

	return resourceWaitTime;
}

size_t CSC2BuildingAbilityCommand::GetSourceID(const CSC2State &state) const
{
	CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
	CSC2State::SBuildingState *sourceBuilding;
	if(0.0 < m_energyCost)
	{
		size_t sourceIndex = 0;
		double earliestMaxEnergyTime = DBL_MAX;
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			if(0 != (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
				continue;

			if(earliestMaxEnergyTime > sourceBuildingList[i]->maxEnergyTime)
			{
				sourceIndex = i;
				sourceBuilding = sourceBuildingList[i];
				earliestMaxEnergyTime = sourceBuilding->maxEnergyTime;
			}
		}

		return sourceIndex;
	}
	else if (0 != m_requireSourceBuildingStatus || 0 != m_requireSourceBuildingStatusAbsent)
	{
		for (size_t i = 0; i < sourceBuildingList.size(); i++)
		{
			if (m_requireSourceBuildingStatus != (sourceBuildingList[i]->status & m_requireSourceBuildingStatus))
				continue;
			if (0 != (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
				continue;

			return sourceBuildingList[i]->buildingID;
			break;
		}
	}
	else
		return state.m_buildings[m_sourceBuildingTypeID]->m_buildingList[state.m_buildings[m_sourceBuildingTypeID]->size() - 1]->buildingID;

	wxASSERT(false);
	return 0;
}

bool CSC2BuildingAbilityCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	state.m_minerals -= m_mineralCost;
	state.m_gas -= m_gasCost;

	CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
	CSC2State::SBuildingState *sourceBuilding = NULL;
	if(0.0 < m_energyCost)
	{
		size_t sourceIndex = 0;
		double earliestMaxEnergyTime = DBL_MAX;
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			if (m_requireSourceBuildingStatus != (sourceBuildingList[i]->status & m_requireSourceBuildingStatus))
				continue;
			if (0 != (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
				continue;

			if(earliestMaxEnergyTime > sourceBuildingList[i]->maxEnergyTime)
			{
				sourceIndex = i;
				sourceBuilding = sourceBuildingList[i];
				earliestMaxEnergyTime = sourceBuilding->maxEnergyTime;
			}
		}

		sourceBuildingList.UseEnergy(sourceIndex, m_energyCost, state.m_time);
	}
	else if (0 != m_requireSourceBuildingStatus || 0 != m_requireSourceBuildingStatusAbsent)
	{
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			if (m_requireSourceBuildingStatus != (sourceBuildingList[i]->status & m_requireSourceBuildingStatus))
				continue;
			if (0 != (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
				continue;
			
			sourceBuilding = sourceBuildingList[i];
			break;
		}
	}
	else
		sourceBuilding = state.m_buildings[m_sourceBuildingTypeID]->m_buildingList[state.m_buildings[m_sourceBuildingTypeID]->size() - 1];

	CSC2State::SBuildingState *targetBuilding = NULL;
	if(m_targetBuilding)
	{
		if (1.0 < m_targetProductionBoost)
		{
			double maxEventTime = 0.0;
			for(size_t i=0; i < events.size(); i++)
			{
				const CSC2Event &event = events.getQueue()[i];
				const CSC2Event::SEvent::SData &eventData = event.m_event.m_data;
				if(!eventData.m_sourceIsBuilding)
					continue;

				CSC2State::SBuildingState *buildingState = state.m_allBuildings[eventData.m_sourceID];
				if(buildingState
					&& (m_canTargetSelf || buildingState != sourceBuilding)
					&& buildingState->buildingTypeID == m_targetBuildingTypeID
					&& 0 == (buildingState->status & m_requireTargetBuildingStatusAbsent)
					&& event.m_time > maxEventTime)
				{
					maxEventTime = event.m_time;
					targetBuilding = buildingState;
				}
			}
		}
		if(NULL == targetBuilding)
		{
			CSC2State::CBuildingStateList &targetBuildingList = *state.m_buildings[m_targetBuildingTypeID];
			if(m_requireTargetBuildingStatusAbsent)
			{
				for(size_t i=0; i < targetBuildingList.size(); i++)
				{
					if((m_canTargetSelf || targetBuildingList[i] != sourceBuilding)
						&& 0 == (targetBuildingList[i]->status & m_requireTargetBuildingStatusAbsent))
					{
						targetBuilding = targetBuildingList[i];
						break;
					}
				}
			}
			else if(!m_canTargetSelf && m_sourceBuilding == m_targetBuilding)
			{
				for(size_t i=0; i < targetBuildingList.size(); i++)
				{
					if(targetBuildingList[i] != sourceBuilding)
					{
						targetBuilding = targetBuildingList[i];
						break;
					}
				}
			}
			else
				targetBuilding = state.m_buildings[m_targetBuildingTypeID]->m_buildingList[state.m_buildings[m_targetBuildingTypeID]->size() - 1];
		}
	}
	if (1.0 != m_sourceProductionBoost)
	{
		sourceBuilding->productionBoost *= m_sourceProductionBoost;
		CVector<CSC2Event> eventsModified;
		bool eventModified = true;
		while(eventModified)
		{
			eventModified = false;
			for(size_t eventIndex=0; eventIndex < events.size(); eventIndex++)
			{
				const CSC2Event::SEvent::SData &eventData = events.getQueue()[eventIndex].m_event.m_data;
				if(eventData.m_sourceIsBuilding && eventData.m_sourceID == sourceBuilding->buildingID)
				{
					eventModified = true;
					CSC2Event event = events.pop(eventIndex);
					event.m_time = state.m_time + (event.m_time - state.m_time) / m_sourceProductionBoost;
					eventsModified.push_back(event);
					break;
				}
			}
		}
		for(size_t i=0; i < eventsModified.size(); i++)
			events.add(eventsModified[i]);
	}
	if (1.0 != m_targetProductionBoost)
	{
		targetBuilding->productionBoost *= m_targetProductionBoost;
		CVector<CSC2Event> eventsModified;
		bool eventModified = true;
		while(eventModified)
		{
			eventModified = false;
			for(size_t eventIndex=0; eventIndex < events.size(); eventIndex++)
			{
				const CSC2Event::SEvent::SData &eventData = events.getQueue()[eventIndex].m_event.m_data;
				if(eventData.m_sourceIsBuilding && eventData.m_sourceID == targetBuilding->buildingID)
				{
					eventModified = true;
					CSC2Event event = events.pop(eventIndex);
					event.m_time = state.m_time + (event.m_time - state.m_time) / m_targetProductionBoost;
					eventsModified.push_back(event);
					break;
				}
			}
		}
		for(size_t i=0; i < eventsModified.size(); i++)
			events.add(eventsModified[i]);
	}
	if(m_morphSourceBuilding)
	{
		state.m_buildingUnderConstructionFlags |= ((SC2BuildingFlags)1 << m_morphSourceBuildingTypeID);
		state.m_buildingUnderConstruction[m_morphSourceBuildingTypeID]++;
		state.m_buildingMorphing[m_sourceBuildingTypeID]++;
		state.m_supplyCapUnderConstruction += m_morphSourceBuilding->GetProvidedSupply() - m_sourceBuilding->GetProvidedSupply();
		if(m_morphSourceBuilding->IsGeyserBuilding() && !m_sourceBuilding->IsGeyserBuilding())
			state.m_geysersUnderConstruction++;
		events.add(CSC2Event(state.m_time + m_morphSourceBuildingTime, CSC2Event::eBuildingMorph, true, sourceBuilding->buildingID, m_morphSourceBuildingTypeID, 0, state.m_time));
	}
	if(m_morphTargetBuilding)
	{
		state.m_buildingUnderConstructionFlags |= ((SC2BuildingFlags)1 << m_morphTargetBuildingTypeID);
		state.m_buildingUnderConstruction[m_morphTargetBuildingTypeID]++;
		state.m_buildingMorphing[m_targetBuildingTypeID]++;
		state.m_supplyCapUnderConstruction += m_morphTargetBuilding->GetProvidedSupply() - m_targetBuilding->GetProvidedSupply();
		if(m_morphTargetBuilding->IsBase() && !m_targetBuilding->IsBase())
			state.m_basesUnderConstruction++;
		if(m_morphTargetBuilding->IsGeyserBuilding() && !m_targetBuilding->IsGeyserBuilding())
			state.m_geysersUnderConstruction++;
		events.add(CSC2Event(state.m_time + m_morphTargetBuildingTime, CSC2Event::eBuildingMorph, true, targetBuilding->buildingID, m_morphTargetBuildingTypeID, 0, state.m_time));
	}
	else if(m_consumeTargetBuilding)
	{
		CSC2State::CBuildingStateList &targetBuildingList = *state.m_buildings[m_targetBuildingTypeID];
		for(size_t i=0; i < targetBuildingList.size(); i++)
		{
			if(targetBuildingList[i] == targetBuilding)
				targetBuildingList.m_buildingList.erase(i);
		}
		events.add(CSC2Event(state.m_time + m_morphTargetBuildingTime, CSC2Event::eBuildingConsume, true, targetBuilding->buildingID));
	}
	if(m_buildBuilding)
	{
		events.add(CSC2Event(state.m_time + m_buildBuildingTime, CSC2Event::eBuildingComplete, false, 0, m_buildBuildingTypeID, 0, state.m_time));
		state.m_buildingUnderConstructionFlags |= ((SC2BuildingFlags)1 << m_buildBuildingTypeID);
		state.m_buildingUnderConstruction[m_buildBuildingTypeID]++;
		state.m_supplyCapUnderConstruction += m_buildBuilding->GetProvidedSupply();
		if(m_buildBuilding->IsGeyserBuilding())
			state.m_geysersUnderConstruction++;
	}
	if(m_buildUnit)
	{
		events.add(CSC2Event(state.m_time + m_buildUnitTime, CSC2Event::eUnitComplete, false, sourceBuilding->buildingID, m_buildUnitTypeID, 0, state.m_time));
		state.m_unitUnderConstructionFlags |= ((SC2UnitFlags)1 << m_buildUnitTypeID);
		state.m_unitUnderConstruction[m_buildUnitTypeID]++;
		state.m_supplyCapUnderConstruction += m_buildUnit->GetProvidedSupply();
	}
	if (m_removeSourceBuildingStatus)
	{
		CVector<CSC2Event> eventsModified;

		bool eventModified = true;
		while (eventModified)
		{
			eventModified = false;
			for (size_t eventIndex = 0; eventIndex < events.size(); eventIndex++)
			{
				const CSC2Event::SEvent::SData &eventData = events.getQueue()[eventIndex].m_event.m_data;
				if (eventData.m_sourceIsBuilding
					&& eventData.m_sourceID == targetBuilding->buildingID
					&& eventData.m_eventCategory == CSC2Event::eBuildingStatusLapse
					&& 0 != (eventData.m_data & m_removeSourceBuildingStatus))
				{
					eventModified = true;

					CSC2Event event = events.pop(eventIndex); // Remove the event to lapse it later
					event.m_event.m_data.m_data &= (~m_removeSourceBuildingStatus);
					if (0 != event.m_event.m_data.m_data)
						eventsModified.push_back(event);
				}
			}
		}

		for (size_t i = 0; i < eventsModified.size(); i++)
			events.add(eventsModified[i]);
		sourceBuilding->status &= (~m_removeSourceBuildingStatus);
	}
	for (size_t i = 0; i < m_applySourceBuildingStatus.size(); i++)
	{
		if(0.0 == m_applySourceBuildingStatusDelay[i])
			sourceBuilding->status |= m_applySourceBuildingStatus[i];
		else
			events.add(CSC2Event(state.m_time + m_applySourceBuildingStatusDelay[i], CSC2Event::eBuildingStatusApply, true, sourceBuilding->buildingID, 0, m_applySourceBuildingStatus[i]));
		
		if(0.0 < m_applySourceBuildingStatusDuration[i])
			events.add(CSC2Event(state.m_time + m_applySourceBuildingStatusDuration[i], CSC2Event::eBuildingStatusLapse, true, sourceBuilding->buildingID, 0, m_applySourceBuildingStatus[i], state.m_time));
	}
	if (m_applyTargetBuildingStatus)
	{
		targetBuilding->status |= m_applyTargetBuildingStatus;
		if (0.0 < m_applyTargetBuildingStatusDuration)
			events.add(CSC2Event(state.m_time + m_applyTargetBuildingStatusDuration, CSC2Event::eBuildingStatusLapse, true, targetBuilding->buildingID, 0, m_applyTargetBuildingStatus, state.m_time));
	}
	if (m_spawnBase)
	{
		state.m_basesUnderConstruction++;
		events.add(CSC2Event(state.m_time + m_spawnBaseTime, CSC2Event::eSpawnBase));
	}

	return true;
}

bool CSC2BuildingAbilityCommand::IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const
{
	return (1.0 != m_targetProductionBoost && 0 != (buildings & ((SC2BuildingFlags)1 << m_targetBuildingTypeID)))
		|| m_spawnBase
		|| (m_buildUnit && (m_buildUnit->IsWorker() || 0.0 < m_buildUnit->GetMineralIncomeRate() || 0 < m_buildUnit->GetProvidedSupply()))
		|| (m_buildBuilding && (m_buildBuilding->IsBase() || 0 < m_buildBuilding->GetProvidedSupply()))
		|| (m_morphSourceBuilding && ((!m_sourceBuilding->IsBase() && m_morphSourceBuilding->IsBase()) || (m_sourceBuilding->GetProvidedSupply() < m_morphSourceBuilding->GetProvidedSupply())))
		|| (m_morphTargetBuilding && ((!m_targetBuilding->IsBase() && m_morphTargetBuilding->IsBase()) || (m_targetBuilding->GetProvidedSupply() < m_morphTargetBuilding->GetProvidedSupply())))
		;
}

size_t CSC2BuildingAbilityCommand::GetProvidedSupply() const
{
	size_t providedSupply = 0, requiredSupply = 0;
	if(m_morphSourceBuilding)
		providedSupply += m_morphSourceBuilding->GetProvidedSupply() - m_sourceBuilding->GetProvidedSupply();
	if(m_morphTargetBuilding)
		providedSupply += m_morphTargetBuilding->GetProvidedSupply() - m_targetBuilding->GetProvidedSupply();
	if(m_buildUnit)
	{
		providedSupply += m_buildUnit->GetProvidedSupply();
		requiredSupply += m_buildUnit->GetSupplyCost();
	}
	if(m_buildBuilding)
		providedSupply += m_buildBuilding->GetProvidedSupply();

	if(m_consumeTargetBuilding)
	{
		if(providedSupply <= m_targetBuilding->GetProvidedSupply())
			providedSupply = 0;
		else
			providedSupply -= m_targetBuilding->GetProvidedSupply();
	}

	if(requiredSupply >= providedSupply)
		return 0;
	else
		return providedSupply - requiredSupply;
}

size_t CSC2BuildingAbilityCommand::GetRequiredSupply() const
{
	size_t providedSupply = 0, requiredSupply = 0;
	if(m_morphSourceBuilding)
		providedSupply += m_morphSourceBuilding->GetProvidedSupply() - m_sourceBuilding->GetProvidedSupply();
	if(m_morphTargetBuilding)
		providedSupply += m_morphTargetBuilding->GetProvidedSupply() - m_targetBuilding->GetProvidedSupply();
	if(m_buildUnit)
	{
		providedSupply += m_buildUnit->GetProvidedSupply();
		requiredSupply += m_buildUnit->GetSupplyCost();
	}
	if(m_buildBuilding)
		providedSupply += m_buildBuilding->GetProvidedSupply();

	if(m_consumeTargetBuilding)
	{
		if(providedSupply <= m_targetBuilding->GetProvidedSupply())
			providedSupply = 0;
		else
			providedSupply -= m_targetBuilding->GetProvidedSupply();
	}

	if(providedSupply >= requiredSupply)
		return 0;
	else
		return requiredSupply - providedSupply;
}

CSC2CancelBuildingCommand::CSC2CancelBuildingCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_cancelBuildingName()
	, m_cancelBuildingTypeID(0)
	, m_cancelBuilding(NULL)
	, m_refundMinerals(0.0)
	, m_refundGas(0.0)
	, m_producedWorkerName()
	, m_producedWorkerTypeID(0)
	, m_producedWorker(NULL)
	, m_producedWorkerTravelTime(0.0)
{
}

bool CSC2CancelBuildingCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	bool added = false;
	if(m_producedWorker)
	{
		unitPrerequisitCommands[m_producedWorkerTypeID]->push_back(command);
		added = true;
	}

	return added;
}

bool CSC2CancelBuildingCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("CancelBuildingUnderConstruction"))
		{
			m_cancelBuildingName = content;
		}
		else if (child->GetName() == wxT("ProducesWorker"))
		{
			child->GetAttribute(wxT("travelTime"), wxT("0.0")).ToCDouble(&m_producedWorkerTravelTime);
			m_producedWorkerName = content;
		}
		else if (child->GetName() == wxT("RefundMinerals"))
		{
			content.ToCDouble(&m_refundMinerals);
		}
		else if (child->GetName() == wxT("RefundGas"))
		{
			content.ToCDouble(&m_refundGas);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Cancel Building' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2CancelBuildingCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	if(!m_cancelBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_cancelBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_cancelBuildingTypeID = i;
				m_cancelBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_cancelBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_cancelBuildingName));
			return false;
		}
	}

	if(!m_producedWorkerName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_producedWorkerName == raceData.m_units[i]->GetName())
			{
				m_producedWorkerTypeID = i;
				m_producedWorker = raceData.m_units[i];
				break;
			}
		}
		if(NULL == m_producedWorker)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_producedWorkerName));
			return false;
		}
	}

	return true;
}

bool CSC2CancelBuildingCommand::HasPrerequisits(const CSC2State &state) const
{
	if(0 == state.m_buildingUnderConstruction[m_cancelBuildingTypeID])
		return false;

	return true;
}

bool CSC2CancelBuildingCommand::HasRequirements(const CSC2State &state) const
{
	if(0 == state.m_buildingUnderConstruction[m_cancelBuildingTypeID])
		return false;

	return true;
}

bool CSC2CancelBuildingCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	state.m_minerals += m_refundMinerals;
	state.m_gas += m_refundGas;

	state.m_buildingUnderConstruction[m_cancelBuildingTypeID]--;
	if(0 == state.m_buildingUnderConstruction[m_cancelBuildingTypeID] && 0 == state.m_buildings[m_cancelBuildingTypeID]->size())
		state.m_buildingUnderConstructionFlags &= (~((SC2BuildingFlags)1 << m_cancelBuildingTypeID));

	double lastBuildTime = DBL_MAX;
	size_t eventPopIndex = 0;
	for(size_t i=0; i < events.size(); i++)
	{
		CSC2Event &event = events[i];
		CSC2Event::SEvent::SData &eventData = event.m_event.m_data;
		if(eventData.m_eventCategory == CSC2Event::eBuildingComplete && eventData.m_targetID == m_cancelBuildingTypeID && lastBuildTime > event.m_time)
		{
			lastBuildTime = event.m_time;
			eventPopIndex = i;
		}
	}

	events.pop(eventPopIndex);

	if(m_producedWorker)
	{
		CSC2State::CUnitStateList &unitStateList = *state.m_units[m_producedWorkerTypeID];
		const CSC2Unit &unit = unitStateList.m_unit;
		CSC2State::SUnitState *unitState = new CSC2State::SUnitState();
		unitState->unitTypeID = m_producedWorkerTypeID;
		unitState->unitID = state.m_allUnits.size();
		state.m_allUnits.push_back(unitState);
		if(unitStateList.m_unit.GetEnergyRechargeRate() > 0)
			unitState->maxEnergyTime = state.m_time + (unit.GetMaxEnergy() - unit.GetStartingEnergy()) / unit.GetEnergyRechargeRate();
		unitStateList.m_unitList.push_back(unitState);

		state.m_unitCompletedFlags |= ((SC2UnitFlags)1 << m_producedWorkerTypeID);
		if(unit.IsWorker())
		{
			state.m_workerCount++;
			state.m_workersMovingToMinerals++;
			events.add(CSC2Event(state.m_time + m_producedWorkerTravelTime, CSC2Event::eWorkerStartMiningMinerals));
		}
		state.m_supply += unit.GetSupplyCost();
		state.m_supplyCap += unit.GetProvidedSupply();
	}

	return true;
}

CSC2BuildUnitCommand::CSC2BuildUnitCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirementNames()
	, m_buildingRequirements(0)
	, m_researchRequirementNames()
	, m_researchRequirements(0)
	, m_sourceBuildingName()
	, m_sourceBuildingTypeID(0)
	, m_sourceBuilding(NULL)
	, m_requireSourceBuildingStatusAbsentNames()
	, m_requireSourceBuildingStatusAbsent(0)
	, m_applySourceBuildingStatusNames()
	, m_applySourceBuildingStatusID(0)
	, m_applySourceBuildingStatus(0)
	, m_applySourceBuildingStatusDuration(0)
	, m_mineralCost(0.0)
	, m_gasCost(0.0)
	, m_consumesLarva(false)
	, m_buildUnitCompletionTime(0.0)
	, m_buildUnitName()
	, m_buildUnitTypeID(0)
	, m_buildUnit(NULL)
	, m_buildUnitCount(1)
	, m_unitOccupiesBuilding(false)
	, m_queueType(VisualItem::qSingle)
{
}

bool CSC2BuildUnitCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	unitPrerequisitCommands[m_buildUnitTypeID]->push_back(command);

	if(m_applySourceBuildingStatus)
	{
		(*buildingStatusPrerequisitCommands[m_sourceBuildingTypeID])[m_applySourceBuildingStatusID]->push_back(command);
	}

	return true;
}

bool CSC2BuildUnitCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("BuildingRequirement"))
		{
			m_buildingRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("ResearchRequirement"))
		{
			m_researchRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("SourceBuilding"))
		{
			m_sourceBuildingName = content;
		}
		else if (child->GetName() == wxT("RequiresSourceBuildingStatusAbsent"))
		{
			m_requireSourceBuildingStatusAbsentNames.push_back(content);
		}
		else if (child->GetName() == wxT("ConsumesLarva"))
		{
			m_consumesLarva = (content == "True");
		}
		else if (child->GetName() == wxT("UnitComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_buildUnitCompletionTime);
			if(child->GetAttribute(wxT("occupiesSourceBuilding"), wxT("")) == wxT("True"))
				m_unitOccupiesBuilding = true;
			else
				m_unitOccupiesBuilding = false;

			unsigned long buildUnitCount = 0;
			child->GetAttribute(wxT("count"), wxT("1")).ToULong(&buildUnitCount);
			m_buildUnitCount = (size_t)buildUnitCount;

			m_buildUnitName = content;
		}
		else if (child->GetName() == wxT("AppliesSourceBuildingStatus"))
		{
			child->GetAttribute(wxT("duration"), wxT("0.0")).ToCDouble(&m_applySourceBuildingStatusDuration);
			m_applySourceBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("Queue"))
		{
			if (content == "Primary")
				m_queueType = VisualItem::qDoublePrimary;
			else if (content == "Secondary")
				m_queueType = VisualItem::qDoubleSecondary;
		}
		else if (child->GetName() == wxT("MineralCost"))
		{
			content.ToCDouble(&m_mineralCost);
		}
		else if (child->GetName() == wxT("GasCost"))
		{
			content.ToCDouble(&m_gasCost);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Build Unit' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2BuildUnitCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
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

	if(!m_sourceBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_sourceBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_sourceBuildingTypeID = i;
				m_sourceBuilding = raceData.m_buildings[i];
				break;
			}
		}

		if(NULL == m_sourceBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_sourceBuildingName));
			return false;
		}

		for(size_t i=0; i < m_requireSourceBuildingStatusAbsentNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_requireSourceBuildingStatusAbsentNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_requireSourceBuildingStatusAbsentNames[i]));
				return false;
			}
			m_requireSourceBuildingStatusAbsent |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}

		for(size_t i=0; i < m_applySourceBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_applySourceBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_applySourceBuildingStatusNames[i]));
				return false;
			}
			m_applySourceBuildingStatusID = statusIndex;
			m_applySourceBuildingStatus |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}
	}

	if(!m_buildUnitName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_buildUnitName == raceData.m_units[i]->GetName())
			{
				m_buildUnitTypeID = i;
				m_buildUnit = raceData.m_units[i];
				break;
			}
		}

		if(NULL == m_buildUnit)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_buildUnitName));
			return false;
		}
	}

	return true;
}

bool CSC2BuildUnitCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchUnderConstructionFlags))
		return false;
	if(state.m_minerals < m_mineralCost && state.m_mineralIncomeRate == 0.0 && 0 == state.m_workersMovingToMinerals)
		return false;
	if(state.m_gas < m_gasCost && state.m_gasIncomeRate == 0.0 && 0 == state.m_workersMovingToGas)
		return false;
	if(m_buildUnit->GetSupplyCost() > 0 && state.m_supply + m_buildUnit->GetSupplyCost() * m_buildUnitCount > state.m_supplyCapUnderConstruction)
		return false;
	if(m_buildUnit->IsUnique() && (0 != state.m_units[m_buildUnitTypeID]->size() || 0 != state.m_unitUnderConstruction[m_buildUnitTypeID]))
		return false;

	return true;
}

bool CSC2BuildUnitCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchCompletedFlags))
		return false;
	if(m_buildUnit->GetSupplyCost() > 0 && state.m_supply + m_buildUnit->GetSupplyCost() * m_buildUnitCount > state.m_supplyCap)
		return false;
	if(NULL != m_sourceBuilding)
	{
		CSC2State::CBuildingStateList &occupyBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
		bool buildingAvailable = false;
		for(size_t i=0; i < occupyBuildingList.size(); i++)
		{
			if(0 == (occupyBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
			{
				buildingAvailable = true;
				break;
			}
		}
		if(!buildingAvailable)
			return false;
	}
	if(m_consumesLarva && 0 == state.m_totalLarvaeCount)
		return false;

	return true;
}

double CSC2BuildUnitCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = 0.0;

	if(m_mineralCost > state.m_minerals)
	{
		double mineralWaitTime = (m_mineralCost - state.m_minerals) / state.m_mineralIncomeRate;
		if(mineralWaitTime > resourceWaitTime)
			resourceWaitTime = mineralWaitTime;
	}

	if(m_gasCost > state.m_gas)
	{
		double gasWaitTime = (m_gasCost - state.m_gas) / state.m_gasIncomeRate;
		if(gasWaitTime > resourceWaitTime)
			resourceWaitTime = gasWaitTime;
	}

	return resourceWaitTime;
}

bool CSC2BuildUnitCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	state.m_minerals -= m_mineralCost;
	state.m_gas -= m_gasCost;
	state.m_supply += m_buildUnit->GetSupplyCost() * m_buildUnitCount;

	CSC2State::SBuildingState *sourceBuildingState = NULL;
	if(NULL != m_sourceBuilding)
	{
		CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			CSC2State::SBuildingState *buildingState = sourceBuildingList[i];
			if(0 == (buildingState->status & m_requireSourceBuildingStatusAbsent)
				&& (NULL == sourceBuildingState || !m_consumesLarva || sourceBuildingState->larvaeCount < buildingState->larvaeCount))
			{
				sourceBuildingState = sourceBuildingList[i];
				if(!m_consumesLarva)
					break;
			}
		}

		if(0.0 < m_applySourceBuildingStatusDuration)
		{
			sourceBuildingState->status |= m_applySourceBuildingStatus;
			events.add(CSC2Event(state.m_time + m_applySourceBuildingStatusDuration / sourceBuildingState->productionBoost, CSC2Event::eBuildingStatusLapse, true, sourceBuildingState->buildingID, 0, m_applySourceBuildingStatus, state.m_time));
		}

		if(m_consumesLarva)
		{
			if(sourceBuildingState->larvaeCount == m_sourceBuilding->GetMaxLarvaeSpawningCount())
				events.add(CSC2Event(state.m_time + m_sourceBuilding->GetLarvaeSpawnTime(), CSC2Event::eBuildingSpawnLarvae, true, sourceBuildingState->buildingID, 0, 1));
			sourceBuildingState->larvaeCount--;
			state.m_totalLarvaeCount--;
		}
	}
	else if(m_consumesLarva)
	{
		for(size_t i=0; i < state.m_larvaeSpawningBuildings.size(); i++)
		{
			CSC2State::SBuildingState *buildingState = state.m_larvaeSpawningBuildings[i];
			if(0 == (buildingState->status & m_requireSourceBuildingStatusAbsent)
				&& (NULL == sourceBuildingState || !m_consumesLarva || sourceBuildingState->larvaeCount < buildingState->larvaeCount))
			{
				sourceBuildingState = state.m_larvaeSpawningBuildings[i];
				if(!m_consumesLarva)
					break;
			}
		}

		CSC2Building *sourceBuilding = m_raceData.m_buildings[sourceBuildingState->buildingTypeID];
		if(0.0 < m_applySourceBuildingStatusDuration)
		{
			sourceBuildingState->status |= m_applySourceBuildingStatus;
			events.add(CSC2Event(state.m_time + m_applySourceBuildingStatusDuration / sourceBuildingState->productionBoost, CSC2Event::eBuildingStatusLapse, true, sourceBuildingState->buildingID, 0, m_applySourceBuildingStatus, state.m_time));
		}

		if(m_consumesLarva)
		{
			if(sourceBuildingState->larvaeCount == sourceBuilding->GetMaxLarvaeSpawningCount())
				events.add(CSC2Event(state.m_time + sourceBuilding->GetLarvaeSpawnTime(), CSC2Event::eBuildingSpawnLarvae, true, sourceBuildingState->buildingID, 0, 1));
			sourceBuildingState->larvaeCount--;
			state.m_totalLarvaeCount--;
		}
	}

	events.add(CSC2Event(
		state.m_time + m_buildUnitCompletionTime / (m_unitOccupiesBuilding ? sourceBuildingState->productionBoost : 1.0),
		CSC2Event::eUnitComplete,
		m_unitOccupiesBuilding,
		sourceBuildingState->buildingID,
		m_buildUnitTypeID,
		m_buildUnitCount,
		state.m_time,
		m_queueType
	));
	state.m_unitUnderConstructionFlags |= ((SC2UnitFlags)1 << m_buildUnitTypeID);
	state.m_unitUnderConstruction[m_buildUnitTypeID] += m_buildUnitCount;
	state.m_supplyCapUnderConstruction += m_buildUnit->GetProvidedSupply() * m_buildUnitCount;

	return true;
}

CSC2UnitAbilityCommand::CSC2UnitAbilityCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirementNames()
	, m_buildingRequirements(0)
	, m_unitRequirementNames()
	, m_unitRequirements(0)
	, m_researchRequirementNames()
	, m_researchRequirements(0)
	, m_sourceUnitName()
	, m_sourceUnitTypeID(0)
	, m_sourceUnit(NULL)
	, m_targetUnitName()
	, m_targetUnitTypeID(0)
	, m_targetUnit(NULL)
	, m_targetBuildingName()
	, m_targetBuildingTypeID(0)
	, m_targetBuilding(NULL)
	, m_requireTargetBuildingStatusAbsentNames()
	, m_requireTargetBuildingStatusAbsent(0)
	, m_mineralCost(0.0)
	, m_gasCost(0.0)
	, m_energyCost(0.0)
	, m_requiredSupply(0)
	, m_consumeSourceUnit(false)
	, m_morphSourceUnitName()
	, m_morphSourceUnitTypeID(0)
	, m_morphSourceUnit(NULL)
	, m_morphSourceUnitTime(0.0)
	, m_consumeTargetUnit(false)
	, m_buildBuildingName()
	, m_buildBuildingTypeID(0)
	, m_buildBuilding(NULL)
	, m_buildBuildingCompletionTime(0.0)
	, m_buildUnitName()
	, m_buildUnitTypeID(0)
	, m_buildUnit(NULL)
	, m_buildUnitCompletionTime(0.0)
	, m_spawnTargetBuildingBonusLarvaeCount(0)
	, m_spawnTargetBuildingBonusLarvaeTime(0.0)
	, m_applyTargetBuildingStatusNames()
	, m_applyTargetBuildingStatusID(0)
	, m_applyTargetBuildingStatus(0)
	, m_applyTargetBuildingStatusDuration(0.0)
	, m_targetProductionBoost(1.0)
{
}

bool CSC2UnitAbilityCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	bool added = false;
	if(m_buildBuilding)
	{
		buildingPrerequisitCommands[m_buildBuildingTypeID]->push_back(command);
		added = true;
	}
	if(m_buildUnit)
	{
		unitPrerequisitCommands[m_buildUnitTypeID]->push_back(command);
		added = true;
	}
	if(m_morphSourceUnit)
	{
		unitPrerequisitCommands[m_morphSourceUnitTypeID]->push_back(command);
		added = true;
	}
	if(m_applyTargetBuildingStatus)
	{
		(*buildingStatusPrerequisitCommands[m_targetBuildingTypeID])[m_applyTargetBuildingStatusID]->push_back(command);
	}

	return added;
}

bool CSC2UnitAbilityCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("IsAutoCastAbility"))
		{
			m_isAutoCastAbility = (content == "True");
		}
		else if (child->GetName() == wxT("BuildingRequirement"))
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
		else if (child->GetName() == wxT("BuildingComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_buildBuildingCompletionTime);
			m_buildBuildingName = content;
		}
		else if (child->GetName() == wxT("UnitComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_buildUnitCompletionTime);
			m_buildUnitName = content;
		}
		else if (child->GetName() == wxT("MineralCost"))
		{
			content.ToCDouble(&m_mineralCost);
		}
		else if (child->GetName() == wxT("GasCost"))
		{
			content.ToCDouble(&m_gasCost);
		}
		else if (child->GetName() == wxT("SourceUnit"))
		{
			m_consumeSourceUnit = (child->GetAttribute("consume", wxT("False")) == wxT("True"));
			child->GetAttribute(wxT("energy"), wxT("0.0")).ToCDouble(&m_energyCost);
			m_sourceUnitName = content;
		}
		else if (child->GetName() == wxT("TargetUnit"))
		{
			m_consumeTargetUnit = (child->GetAttribute("consume", wxT("False")) == wxT("True"));
			m_targetUnitName = content;
		}
		else if (child->GetName() == wxT("TargetBuilding"))
		{
			m_targetBuildingName = content;
		}
		else if (child->GetName() == wxT("RequiresTargetBuildingStatusAbsent"))
		{
			m_requireTargetBuildingStatusAbsentNames.push_back(content);
		}
		else if (child->GetName() == wxT("AppliesTargetBuildingStatus"))
		{
			child->GetAttribute(wxT("duration"), wxT("0.0")).ToCDouble(&m_applyTargetBuildingStatusDuration);
			m_applyTargetBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("SpawnTargetBuildingBonusLarvae"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_spawnTargetBuildingBonusLarvaeTime);
			unsigned long spawnTargetBuildingBonusLarvaeCount;
			content.ToULong(&spawnTargetBuildingBonusLarvaeCount);
			m_spawnTargetBuildingBonusLarvaeCount = (size_t)spawnTargetBuildingBonusLarvaeCount;
		}
		else if (child->GetName() == wxT("MorphSourceUnit"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_morphSourceUnitTime);
			m_morphSourceUnitName = content;
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Unit Ability' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2UnitAbilityCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
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

	if(!m_sourceUnitName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_sourceUnitName == raceData.m_units[i]->GetName())
			{
				m_sourceUnitTypeID = i;
				m_sourceUnit = raceData.m_units[i];
				m_unitRequirements |= ((SC2UnitFlags)1 << i);
				break;
			}
		}

		if(NULL == m_sourceUnit)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_sourceUnitName));
			return false;
		}
	}

	if(!m_targetUnitName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_targetUnitName == raceData.m_units[i]->GetName())
			{
				m_targetUnitTypeID = i;
				m_targetUnit = raceData.m_units[i];
				m_unitRequirements |= ((SC2UnitFlags)1 << i);
				break;
			}
		}

		if(NULL == m_targetUnit)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_targetUnitName));
			return false;
		}
	}

	if(!m_targetBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_targetBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_targetBuildingTypeID = i;
				m_targetBuilding = raceData.m_buildings[i];
				break;
			}
		}
		if(NULL == m_targetBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_targetBuildingName));
			return false;
		}

		for(size_t i=0; i < m_requireTargetBuildingStatusAbsentNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_requireTargetBuildingStatusAbsentNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_requireTargetBuildingStatusAbsentNames[i]));
				return false;
			}
			m_requireTargetBuildingStatusAbsent |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}

		for(size_t i=0; i < m_applyTargetBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_applyTargetBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_applyTargetBuildingStatusNames[i]));
				return false;
			}
			m_applyTargetBuildingStatusID = statusIndex;
			m_applyTargetBuildingStatus |= ((SC2BuildingStatusFlags)1 << statusIndex);
			m_targetProductionBoost *= m_raceData.m_buildingStatuses[statusIndex]->GetProductionBoostFactor();
		}
	}

	if(!m_morphSourceUnitName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_morphSourceUnitName == raceData.m_units[i]->GetName())
			{
				m_morphSourceUnitTypeID = i;
				m_morphSourceUnit = raceData.m_units[i];
				break;
			}
		}
		if(NULL == m_morphSourceUnit)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_morphSourceUnitName));
			return false;
		}
	}

	if(!m_buildBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_buildBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_buildBuildingTypeID = i;
				m_buildBuilding = raceData.m_buildings[i];
				break;
			}
		}

		if(NULL == m_buildBuilding)
		{
			wxMessageBox(wxString::Format("Undefined Building '%s'", m_buildBuildingName));
			return false;
		}
	}

	if(!m_buildUnitName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_units.size(); i++)
		{
			if(m_buildUnitName == raceData.m_units[i]->GetName())
			{
				m_buildUnitTypeID = i;
				m_buildUnit = raceData.m_units[i];
				break;
			}
		}

		if(NULL == m_buildUnit)
		{
			wxMessageBox(wxString::Format("Undefined unit '%s'", m_buildUnitName));
			return false;
		}
	}

	m_requiredSupply = 0;
	if(NULL != m_buildUnit)
		m_requiredSupply += m_buildUnit->GetSupplyCost();
	if(NULL != m_morphSourceUnit)
	{
		if(m_morphSourceUnit->GetSupplyCost() >= m_sourceUnit->GetSupplyCost())
			m_requiredSupply += m_morphSourceUnit->GetSupplyCost() - m_sourceUnit->GetSupplyCost();
		else if(m_sourceUnit->GetSupplyCost() - m_morphSourceUnit->GetSupplyCost() >= m_requiredSupply)
			m_requiredSupply = 0;
		else
			m_requiredSupply -= m_sourceUnit->GetSupplyCost() - m_morphSourceUnit->GetSupplyCost();
	}
	else if(m_consumeSourceUnit && NULL != m_sourceUnit)
	{
		if(m_sourceUnit->GetSupplyCost() >= m_requiredSupply)
			m_requiredSupply = 0;
		else
			m_requiredSupply -= m_sourceUnit->GetSupplyCost();
	}
	if(m_consumeTargetUnit && NULL != m_targetUnit)
	{
		if(m_targetUnit->GetSupplyCost() >= m_requiredSupply)
			m_requiredSupply = 0;
		else
			m_requiredSupply -= m_targetUnit->GetSupplyCost();
	}


	return true;
}

bool CSC2UnitAbilityCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;
	if(m_unitRequirements != (m_unitRequirements & state.m_unitUnderConstructionFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchUnderConstructionFlags))
		return false;
	if(state.m_minerals < m_mineralCost && state.m_mineralIncomeRate == 0.0 && 0 == state.m_workersMovingToMinerals)
		return false;
	if(state.m_gas < m_gasCost && state.m_gasIncomeRate == 0.0 && 0 == state.m_workersMovingToGas)
		return false;
	if(m_requiredSupply > 0 && state.m_supply + m_requiredSupply > state.m_supplyCapUnderConstruction)
		return false;
	if(NULL != m_sourceUnit && m_sourceUnit == m_targetUnit)
	{
		if(2 > state.m_units[m_sourceUnitTypeID]->size() + state.m_unitUnderConstruction[m_sourceUnitTypeID])
			return false;
	}
	else
	{
		if(NULL != m_sourceUnit && 0 == state.m_units[m_sourceUnitTypeID]->size() && 0 == state.m_unitUnderConstruction[m_sourceUnitTypeID])
			return false;
		if(NULL != m_targetUnit && 0 == state.m_units[m_targetUnitTypeID]->size() && 0 == state.m_unitUnderConstruction[m_targetUnitTypeID])
			return false;
	}
	if(NULL != m_targetBuilding && 0 == state.m_buildings[m_targetBuildingTypeID]->size() && 0 == state.m_buildingUnderConstruction[m_targetBuildingTypeID])
		return false;
	if(m_buildUnit && m_buildUnit->IsUnique() && (0 != state.m_units[m_buildUnitTypeID]->size() || 0 != state.m_unitUnderConstruction[m_buildUnitTypeID]))
		return false;
	if(m_morphSourceUnit && m_morphSourceUnit->IsUnique() && (0 != state.m_units[m_morphSourceUnitTypeID]->size() || 0 != state.m_unitUnderConstruction[m_morphSourceUnitTypeID]))
		return false;
	return true;
}

bool CSC2UnitAbilityCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_unitRequirements != (m_unitRequirements & state.m_unitCompletedFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchCompletedFlags))
		return false;
	if(m_requiredSupply > 0 && state.m_supply + m_requiredSupply > state.m_supplyCap)
		return false;
	if(NULL != m_sourceUnit && m_sourceUnit == m_targetUnit)
	{
		if(2 > state.m_units[m_sourceUnitTypeID]->size())
			return false;
	}
	else
	{
		if(NULL != m_sourceUnit && 0 == state.m_units[m_sourceUnitTypeID]->size())
			return false;
		if(NULL != m_targetUnit && 0 == state.m_units[m_targetUnitTypeID]->size())
			return false;
	}
	if(NULL != m_targetBuilding && 0 == state.m_buildings[m_targetBuildingTypeID]->size())
		return false;
	if(0 != m_requireTargetBuildingStatusAbsent)
	{
		bool availableBuilding = false;
		const CSC2State::CBuildingStateList &targetBuildingList = *state.m_buildings[m_targetBuildingTypeID];
		for(size_t i=0; i < targetBuildingList.size(); i++)
		{
			if(0 != (targetBuildingList[i]->status & m_requireTargetBuildingStatusAbsent))
				continue;

			availableBuilding = true;
			break;
		}
		if(!availableBuilding)
			return false;
	}

	return true;
}

double CSC2UnitAbilityCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = 0.0;

	if(m_mineralCost > state.m_minerals)
	{
		double mineralWaitTime = (m_mineralCost - state.m_minerals) / state.m_mineralIncomeRate;
		if(mineralWaitTime > resourceWaitTime)
			resourceWaitTime = mineralWaitTime;
	}

	if(m_gasCost > state.m_gas)
	{
		double gasWaitTime = (m_gasCost - state.m_gas) / state.m_gasIncomeRate;
		if(gasWaitTime > resourceWaitTime)
			resourceWaitTime = gasWaitTime;
	}

	if(m_energyCost > 0)
	{
		double minimumEnergyWaitTime = DBL_MAX;
		const CSC2State::CUnitStateList &sourceUnitList = *state.m_units[m_sourceUnitTypeID];
		for(size_t i=0; i < sourceUnitList.size(); i++)
		{
			double energy = sourceUnitList.CalculateEnergy(i, state.m_time);
			if(m_energyCost <= energy)
			{
				minimumEnergyWaitTime = 0.0;
				break;
			}

			double energyWaitTime = (m_energyCost - energy) / m_sourceUnit->GetEnergyRechargeRate();
			if(energyWaitTime < minimumEnergyWaitTime)
				minimumEnergyWaitTime = energyWaitTime;
		}

		if(minimumEnergyWaitTime > resourceWaitTime)
			resourceWaitTime = minimumEnergyWaitTime;
	}

	return resourceWaitTime;
}

bool CSC2UnitAbilityCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	state.m_minerals -= m_mineralCost;
	state.m_gas -= m_gasCost;
	state.m_supply += m_requiredSupply;

	CSC2State::CUnitStateList &sourceUnitList = *state.m_units[m_sourceUnitTypeID];
	CSC2State::SUnitState *sourceUnit = NULL;
	if(m_consumeSourceUnit)
	{
		size_t sourceIndex = 0;
		if(0.0 < m_sourceUnit->GetMaxEnergy())
		{
			// Use the one with the least energy that's more than the energy cost
			double bestMaxEnergyTime = DBL_MAX;
			for(size_t i=0; i < sourceUnitList.size(); i++)
			{
				if(sourceUnitList.CalculateEnergy(i, state.m_time) >= m_energyCost && sourceUnitList[i]->maxEnergyTime > bestMaxEnergyTime)
				{
					bestMaxEnergyTime = sourceUnitList[i]->maxEnergyTime;
					sourceIndex = i;
				}
			}
		}
		else
			sourceIndex = sourceUnitList.size() - 1;

		state.m_allUnits[sourceUnitList[sourceIndex]->unitID] = NULL;
		delete sourceUnitList[sourceIndex];
		sourceUnitList.erase(sourceIndex);
	}
	else if(m_energyCost > 0)
	{
		size_t sourceIndex = 0;

		// Use the one with the most energy
		double bestMaxEnergyTime = sourceUnitList[0]->maxEnergyTime;
		for(size_t i=1; i < sourceUnitList.size(); i++)
		{
			if(sourceUnitList[i]->maxEnergyTime < bestMaxEnergyTime)
			{
				bestMaxEnergyTime = sourceUnitList[i]->maxEnergyTime;
				sourceIndex = i;
			}
		}

		sourceUnitList.UseEnergy(sourceIndex, m_energyCost, state.m_time);
		sourceUnit = sourceUnitList[sourceIndex];
	}
	else
		sourceUnit = state.m_units[m_sourceUnitTypeID]->m_unitList[state.m_units[m_sourceUnitTypeID]->size() - 1];

	if(m_morphSourceUnit)
	{
		CSC2State::CUnitStateList &sourceUnitList = *state.m_units[m_sourceUnitTypeID];
		for(size_t i=0; i < sourceUnitList.size(); i++)
		{
			if(sourceUnitList[i] == sourceUnit)
				sourceUnitList.m_unitList.erase(i);
		}
		state.m_unitUnderConstructionFlags |= ((SC2UnitFlags)1 << m_morphSourceUnitTypeID);
		state.m_unitUnderConstruction[m_morphSourceUnitTypeID]++;
		state.m_supplyCapUnderConstruction += m_morphSourceUnit->GetProvidedSupply() - m_sourceUnit->GetProvidedSupply();
		events.add(CSC2Event(state.m_time + m_morphSourceUnitTime, CSC2Event::eUnitMorph, false, sourceUnit->unitID, m_morphSourceUnitTypeID, 0, state.m_time));

		if(m_sourceUnit->IsWorker())
		{
			state.m_workerCount--;
			if(state.m_workersOnMinerals > 0)
			{
				state.m_workersOnMinerals--;
				state.RecalculateMineralIncomeRate();
			}
			else if(state.m_workersOnGas > 0)
			{
				state.m_workersOnGas--;
				state.RecalculateGasIncomeRate();
			}
		}
	}
	if(m_consumeTargetUnit)
	{
		CSC2State::CUnitStateList &targetUnitList = *state.m_units[m_targetUnitTypeID];

		// Use the one with the least energy
		size_t targetUnitIndex = 0;
		if(0.0 < m_targetUnit->GetMaxEnergy())
		{
			double bestMaxEnergyTime = targetUnitList[0]->maxEnergyTime;
			for(size_t i=1; i < targetUnitList.size(); i++)
			{
				if(targetUnitList[i]->maxEnergyTime > bestMaxEnergyTime)
				{
					bestMaxEnergyTime = targetUnitList[i]->maxEnergyTime;
					targetUnitIndex = i;
				}
			}
		}
		else
			targetUnitIndex = targetUnitList.size() - 1;

		state.m_allUnits[targetUnitList[targetUnitIndex]->unitID] = NULL;
		delete targetUnitList[targetUnitIndex];
		targetUnitList.erase(targetUnitIndex);
	}

	CSC2State::SBuildingState *targetBuilding = NULL;
	if(m_targetBuilding)
	{
		if(1.0 < m_targetProductionBoost)
		{
			double maxEventTime = 0.0;
			for(size_t i=0; i < events.size(); i++)
			{
				const CSC2Event &event = events.getQueue()[i];
				const CSC2Event::SEvent::SData &eventData = event.m_event.m_data;
				if(!eventData.m_sourceIsBuilding)
					continue;

				CSC2State::SBuildingState *buildingState = state.m_allBuildings[eventData.m_sourceID];
				if(buildingState
					&& buildingState->buildingTypeID == m_targetBuildingTypeID
					&& 0 == (buildingState->status & m_requireTargetBuildingStatusAbsent)
					&& event.m_time > maxEventTime)
				{
					maxEventTime = event.m_time;
					targetBuilding = buildingState;
				}
			}
		}
		if(NULL == targetBuilding)
		{
			CSC2State::CBuildingStateList &targetBuildingList = *state.m_buildings[m_targetBuildingTypeID];
			if(m_requireTargetBuildingStatusAbsent)
			{
				for(size_t i=0; i < targetBuildingList.size(); i++)
				{
					if(0 == (targetBuildingList[i]->status & m_requireTargetBuildingStatusAbsent))
					{
						targetBuilding = targetBuildingList[i];
						break;
					}
				}
			}
			else
				targetBuilding = state.m_buildings[m_targetBuildingTypeID]->m_buildingList[state.m_buildings[m_targetBuildingTypeID]->size() - 1];
		}
	}
	if (1.0 != m_targetProductionBoost)
	{
		CVector<CSC2Event> eventsModified;
		bool eventModified = true;
		while(eventModified)
		{
			eventModified = false;
			for(size_t eventIndex=0; eventIndex < events.size(); eventIndex++)
			{
				const CSC2Event::SEvent::SData &eventData = events.getQueue()[eventIndex].m_event.m_data;
				if(eventData.m_sourceIsBuilding && eventData.m_sourceID == targetBuilding->buildingID)
				{
					eventModified = true;
					CSC2Event event = events.pop(eventIndex);
					event.m_time = state.m_time + (event.m_time - state.m_time) / m_targetProductionBoost;
					eventsModified.push_back(event);
					break;
				}
			}
		}
		for(size_t i=0; i < eventsModified.size(); i++)
			events.add(eventsModified[i]);
	}
	if(m_applyTargetBuildingStatus)
	{
		targetBuilding->status |= m_applyTargetBuildingStatus;
		if(0.0 < m_applyTargetBuildingStatusDuration)
			events.add(CSC2Event(state.m_time + m_applyTargetBuildingStatusDuration, CSC2Event::eBuildingStatusLapse, true, targetBuilding->buildingID, 0, m_applyTargetBuildingStatus, state.m_time));
	}
	if(m_spawnTargetBuildingBonusLarvaeCount > 0)
		events.add(CSC2Event(state.m_time + m_spawnTargetBuildingBonusLarvaeTime, CSC2Event::eBuildingSpawnBonusLarvae, true, targetBuilding->buildingID, 0, m_spawnTargetBuildingBonusLarvaeCount));

	if(m_buildBuilding)
	{
		events.add(CSC2Event(state.m_time + m_buildBuildingCompletionTime, CSC2Event::eBuildingComplete, false, 0, m_buildBuildingTypeID, 0, state.m_time));
		state.m_buildingUnderConstructionFlags |= ((SC2BuildingFlags)1 << m_buildBuildingTypeID);
		state.m_buildingUnderConstruction[m_buildBuildingTypeID]++;
		state.m_supplyCapUnderConstruction += m_buildBuilding->GetProvidedSupply();
		if(m_buildBuilding->IsGeyserBuilding())
			state.m_geysersUnderConstruction++;
	}

	if(m_buildUnit)
	{
		events.add(CSC2Event(state.m_time + m_buildUnitCompletionTime, CSC2Event::eUnitComplete, false, 0, m_buildUnitTypeID, 0, state.m_time));
		state.m_unitUnderConstructionFlags |= ((SC2UnitFlags)1 << m_buildUnitTypeID);
		state.m_unitUnderConstruction[m_buildUnitTypeID]++;
		state.m_supplyCapUnderConstruction += m_buildUnit->GetProvidedSupply();
	}

	return true;
}

bool CSC2UnitAbilityCommand::IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const
{
	return (m_buildUnit && (m_buildUnit->IsWorker() || m_buildUnit->GetMineralIncomeRate() || 0 < m_buildUnit->GetProvidedSupply()))
		|| (m_morphSourceUnit && ((!m_sourceUnit->IsWorker() && m_morphSourceUnit->IsWorker()) || (m_sourceUnit->GetMineralIncomeRate() < m_morphSourceUnit->GetMineralIncomeRate()) || (m_sourceUnit->GetProvidedSupply() < m_morphSourceUnit->GetProvidedSupply())))
		|| (m_targetBuilding && m_spawnTargetBuildingBonusLarvaeCount > 0)
		;
}

size_t CSC2UnitAbilityCommand::GetProvidedSupply() const
{
	size_t providedSupply = 0, requiredSupply = 0;
	if(m_morphSourceUnit)
	{
		providedSupply += m_morphSourceUnit->GetProvidedSupply() - m_sourceUnit->GetProvidedSupply();
		requiredSupply += m_morphSourceUnit->GetSupplyCost() - m_sourceUnit->GetSupplyCost();
	}
	if(m_buildUnit)
	{
		providedSupply += m_buildUnit->GetProvidedSupply();
		requiredSupply += m_buildUnit->GetSupplyCost();
	}

	if(requiredSupply >= providedSupply)
		return 0;
	else
		return providedSupply - requiredSupply;
}

size_t CSC2UnitAbilityCommand::GetRequiredSupply() const
{
	size_t providedSupply = 0, requiredSupply = 0;
	if(m_morphSourceUnit)
	{
		providedSupply += m_morphSourceUnit->GetProvidedSupply() - m_sourceUnit->GetProvidedSupply();
		requiredSupply += m_morphSourceUnit->GetSupplyCost() - m_sourceUnit->GetSupplyCost();
	}
	if(m_buildUnit)
	{
		providedSupply += m_buildUnit->GetProvidedSupply();
		requiredSupply += m_buildUnit->GetSupplyCost();
	}

	if(providedSupply >= requiredSupply)
		return 0;
	else
		return requiredSupply - providedSupply;
}

CSC2ResearchCommand::CSC2ResearchCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirementNames()
	, m_buildingRequirements(0)
	, m_researchRequirementNames()
	, m_researchRequirements(0)
	, m_sourceBuildingName()
	, m_sourceBuildingTypeID(0)
	, m_sourceBuilding(NULL)
	, m_requireSourceBuildingStatusAbsentNames()
	, m_requireSourceBuildingStatusAbsent(0)
	, m_applySourceBuildingStatusNames()
	, m_applySourceBuildingStatusID(0)
	, m_applySourceBuildingStatus(0)
	, m_applySourceBuildingStatusDuration(0.0)
	, m_mineralCost(0.0)
	, m_gasCost(0.0)
	, m_completionTime(0.0)
	, m_completeResearchName()
	, m_completeResearchID(0)
	, m_completeResearch(NULL)
	, m_queueType(VisualItem::qSingle)
{
}

bool CSC2ResearchCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	researchPrerequisitCommands[m_completeResearchID]->push_back(command);

	if(m_applySourceBuildingStatus)
	{
		(*buildingStatusPrerequisitCommands[m_sourceBuildingTypeID])[m_applySourceBuildingStatusID]->push_back(command);
	}

	return true;
}

bool CSC2ResearchCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("BuildingRequirement"))
		{
			m_buildingRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("ResearchRequirement"))
		{
			m_researchRequirementNames.push_back(content);
		}
		else if (child->GetName() == wxT("SourceBuilding"))
		{
			m_sourceBuildingName = content;
		}
		else if (child->GetName() == wxT("RequiresSourceBuildingStatusAbsent"))
		{
			m_requireSourceBuildingStatusAbsentNames.push_back(content);
		}
		else if (child->GetName() == wxT("ResearchComplete"))
		{
			child->GetAttribute(wxT("time"), wxT("0.0")).ToCDouble(&m_completionTime);
			m_completeResearchName = content;
		}
		else if (child->GetName() == wxT("AppliesSourceBuildingStatus"))
		{
			child->GetAttribute(wxT("duration"), wxT("0.0")).ToCDouble(&m_applySourceBuildingStatusDuration);
			m_applySourceBuildingStatusNames.push_back(content);
		}
		else if (child->GetName() == wxT("Queue"))
		{
			if (content == "Primary")
				m_queueType = VisualItem::qDoublePrimary;
			else if (content == "Secondary")
				m_queueType = VisualItem::qDoubleSecondary;
		}
		else if (child->GetName() == wxT("MineralCost"))
		{
			content.ToCDouble(&m_mineralCost);
		}
		else if (child->GetName() == wxT("GasCost"))
		{
			content.ToCDouble(&m_gasCost);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Research' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2ResearchCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
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

	if(!m_sourceBuildingName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_buildings.size(); i++)
		{
			if(m_sourceBuildingName == raceData.m_buildings[i]->GetName())
			{
				m_sourceBuildingTypeID = i;
				m_sourceBuilding = raceData.m_buildings[i];
				break;
			}
		}

		if(NULL == m_sourceBuilding)
		{
			wxMessageBox(wxString::Format("Undefined building '%s'", m_sourceBuildingName));
			return false;
		}

		for(size_t i=0; i < m_requireSourceBuildingStatusAbsentNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_requireSourceBuildingStatusAbsentNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_requireSourceBuildingStatusAbsentNames[i]));
				return false;
			}
			m_requireSourceBuildingStatusAbsent |= ((SC2BuildingFlags)1 << statusIndex);
		}

		for(size_t i=0; i < m_applySourceBuildingStatusNames.size(); i++)
		{
			size_t statusIndex = UINT_MAX;
			for (size_t j = 0; j < m_raceData.m_buildingStatuses.size(); j++)
			{
				if (m_applySourceBuildingStatusNames[i] == m_raceData.m_buildingStatuses[j]->GetName())
				{
					statusIndex = j;
					break;
				}
			}

			if (statusIndex >= m_raceData.m_buildingStatuses.size())
			{
				wxMessageBox(wxString::Format("Undefined building state '%s'", m_applySourceBuildingStatusNames[i]));
				return false;
			}
			m_applySourceBuildingStatusID = statusIndex;
			m_applySourceBuildingStatus |= ((SC2BuildingStatusFlags)1 << statusIndex);
		}
	}

	if(!m_completeResearchName.IsEmpty())
	{
		for(size_t i=0; i < raceData.m_research.size(); i++)
		{
			if(m_completeResearchName == raceData.m_research[i]->GetName())
			{
				m_completeResearchID = i;
				m_completeResearch = raceData.m_research[i];
				break;
			}
		}

		if(NULL == m_completeResearch)
		{
			wxMessageBox(wxString::Format("Undefined research '%s'", m_completeResearchName));
			return false;
		}
	}

	return true;
}

bool CSC2ResearchCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchUnderConstructionFlags))
		return false;
	if(state.m_minerals < m_mineralCost && state.m_mineralIncomeRate == 0.0 && 0 == state.m_workersMovingToMinerals)
		return false;
	if(state.m_gas < m_gasCost && state.m_gasIncomeRate == 0.0 && 0 == state.m_workersMovingToGas)
		return false;

	return true;
}

bool CSC2ResearchCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchCompletedFlags))
		return false;
	if(NULL != m_sourceBuilding)
	{
		const CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
		bool buildingAvailable = false;
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			if(0 == (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
			{
				buildingAvailable = true;
				break;
			}
		}
		if(!buildingAvailable)
			return false;
	}

	return true;
}

double CSC2ResearchCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = 0.0;

	if(m_mineralCost > state.m_minerals)
	{
		double mineralWaitTime = (m_mineralCost - state.m_minerals) / state.m_mineralIncomeRate;
		if(mineralWaitTime > resourceWaitTime)
			resourceWaitTime = mineralWaitTime;
	}

	if(m_gasCost > state.m_gas)
	{
		double gasWaitTime = (m_gasCost - state.m_gas) / state.m_gasIncomeRate;
		if(gasWaitTime > resourceWaitTime)
			resourceWaitTime = gasWaitTime;
	}

	return resourceWaitTime;
}

bool CSC2ResearchCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	state.m_minerals -= m_mineralCost;
	state.m_gas -= m_gasCost;

	CSC2State::SBuildingState *sourceBuilding = NULL;
	double productionBoost = 1.0;
	if(NULL != m_sourceBuilding)
	{
		CSC2State::CBuildingStateList &sourceBuildingList = *state.m_buildings[m_sourceBuildingTypeID];
		for(size_t i=0; i < sourceBuildingList.size(); i++)
		{
			if(0 == (sourceBuildingList[i]->status & m_requireSourceBuildingStatusAbsent))
			{
				sourceBuilding = sourceBuildingList[i];
				break;
			}
		}

		productionBoost = sourceBuilding->productionBoost;

		if(0.0 < m_applySourceBuildingStatusDuration)
		{
			sourceBuilding->status |= m_applySourceBuildingStatus;
			events.add(CSC2Event(state.m_time + m_applySourceBuildingStatusDuration / productionBoost, CSC2Event::eBuildingStatusLapse, true, sourceBuilding->buildingID, 0, m_applySourceBuildingStatus, state.m_time));
		}
	}

	events.add(CSC2Event(
		state.m_time + m_completionTime / productionBoost,
		CSC2Event::eResearchComplete,
		NULL != sourceBuilding,
		sourceBuilding ? sourceBuilding->buildingID : 0,
		m_completeResearchID,
		0,
		state.m_time,
		m_queueType
	));
	state.m_researchUnderConstruction[m_completeResearchID] = true;
	state.m_researchUnderConstructionFlags |= ((SC2ResearchFlags)1 << m_completeResearchID);

	return true;
}

CSC2SequenceCommand::CSC2SequenceCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_mineralCost(0.0)
	, m_gasCost(0.0)
	, m_buildingRequirements(0)
	, m_unitRequirements(0)
	, m_researchRequirements(0)
	, m_commandSequenceNames()
	, m_commandSequence()
{
}

bool CSC2SequenceCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	bool ret = false;
	for(size_t i=0; i < m_commandSequence.size(); i++)
	{
		if(m_commandSequence[i]->AddToPrerequisitCommands(command, buildingPrerequisitCommands, buildingStatusPrerequisitCommands, unitPrerequisitCommands, researchPrerequisitCommands))
			ret = true;
	}

	return ret;
}

bool CSC2SequenceCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("SubCommand"))
		{
			m_commandSequenceNames.push_back(content);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Sequence' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2SequenceCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	for(size_t i=0; i < m_commandSequenceNames.size(); i++)
	{
		const wxString &commandName = m_commandSequenceNames[i];
		
		for(size_t j=0; j < commands.size(); j++)
		{
			if(commandName == commands[j]->GetName())
			{
				m_commandSequence.push_back(commands[j]);
				m_mineralCost += commands[j]->GetMineralCost();
				m_gasCost += commands[j]->GetGasCost();
				m_buildingRequirements |= commands[j]->GetBuildingRequirementFlags();
				m_unitRequirements |= commands[j]->GetUnitRequirementFlags();
				m_researchRequirements |= commands[j]->GetResearchRequirementFlags();
				break;
			}

			if(j >= commands.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined command '%s'", commandName));
				return false;
			}
		}
	}

	return true;
}

bool CSC2SequenceCommand::HasPrerequisits(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingUnderConstructionFlags))
		return false;
	if(m_unitRequirements != (m_unitRequirements & state.m_unitUnderConstructionFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchUnderConstructionFlags))
		return false;
	if(state.m_minerals < m_mineralCost && state.m_mineralIncomeRate == 0.0 && 0 == state.m_workersMovingToMinerals)
		return false;
	if(state.m_gas < m_gasCost && state.m_gasIncomeRate == 0.0 && 0 == state.m_workersMovingToGas)
		return false;

	return true;
}

bool CSC2SequenceCommand::HasRequirements(const CSC2State &state) const
{
	if(m_buildingRequirements != (m_buildingRequirements & state.m_buildingCompletedFlags))
		return false;
	if(m_unitRequirements != (m_unitRequirements & state.m_unitCompletedFlags))
		return false;
	if(m_researchRequirements != (m_researchRequirements & state.m_researchCompletedFlags))
		return false;

	return true;
}

double CSC2SequenceCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = 0.0;

	if(m_mineralCost > state.m_minerals)
	{
		double mineralWaitTime = (m_mineralCost - state.m_minerals) / state.m_mineralIncomeRate;
		if(mineralWaitTime > resourceWaitTime)
			resourceWaitTime = mineralWaitTime;
	}

	if(m_gasCost > state.m_gas)
	{
		double gasWaitTime = (m_gasCost - state.m_gas) / state.m_gasIncomeRate;
		if(gasWaitTime > resourceWaitTime)
			resourceWaitTime = gasWaitTime;
	}

	for(size_t i=0; i < m_commandSequence.size(); i++)
	{
		double commandSequenceResourceWaitTime = m_commandSequence[i]->GetResourceWaitTime(state);
		if(commandSequenceResourceWaitTime > resourceWaitTime)
			resourceWaitTime = commandSequenceResourceWaitTime;
	}

	return resourceWaitTime;
}

bool CSC2SequenceCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	for(size_t i=0; i < m_commandSequence.size(); i++)
	{
		if(!m_commandSequence[i]->HasRequirements(state))
			return false;
		if(!m_commandSequence[i]->ExecuteCommand(state, events))
			return false;
	}

	return true;
}

bool CSC2SequenceCommand::IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const
{
	for(size_t i=0; i < m_commandSequence.size(); i++)
	{
		if(m_commandSequence[i]->IsMacroAbility(buildings, units, research))
			return true;
	}

	return false;
}

size_t CSC2SequenceCommand::GetProvidedSupply() const
{
	size_t providedSupply = 0, requiredSupply = 0;
	for(size_t i=0; i < m_commandSequence.size(); i++)
	{
		providedSupply += m_commandSequence[i]->GetProvidedSupply();
		requiredSupply += m_commandSequence[i]->GetRequiredSupply();
	}

	if(providedSupply > requiredSupply)
		return providedSupply - requiredSupply;

	return 0;
}

size_t CSC2SequenceCommand::GetRequiredSupply() const
{
	size_t providedSupply = 0, requiredSupply = 0, maxRequiredSupply = 0;
	for(size_t i=0; i < m_commandSequence.size(); i++)
	{
		providedSupply += m_commandSequence[i]->GetProvidedSupply();
		requiredSupply += m_commandSequence[i]->GetRequiredSupply();

		if(requiredSupply > providedSupply && requiredSupply - providedSupply > maxRequiredSupply)
			maxRequiredSupply = requiredSupply - providedSupply;
	}

	return maxRequiredSupply;
}

CSC2MultiCommand::CSC2MultiCommand(const CSC2RaceData &raceData)
	: CSC2XMLCommand(raceData)
	, m_buildingRequirements(0)
	, m_unitRequirements(0)
	, m_researchRequirements(0)
	, m_willBuildGasBuilding(false)
	, m_commandMultiNames()
	, m_commandMulti()
	, m_willBuildUnit(false)
	, m_willBuildWorker(false)
{
}

bool CSC2MultiCommand::AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const
{
	bool ret = false;
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->AddToPrerequisitCommands(command, buildingPrerequisitCommands, buildingStatusPrerequisitCommands, unitPrerequisitCommands, researchPrerequisitCommands))
			ret = true;
	}

	return ret;
}

bool CSC2MultiCommand::LoadXML(const wxXmlNode *xmlCommand)
{
	for(wxXmlNode *child = xmlCommand->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("AvailableToGeneticAlgorithm"))
		{
			m_isAvailableToGeneticAlgorithm = (content == "True");
		}
		else if (child->GetName() == wxT("IsAutoCastAbility"))
		{
			m_isAutoCastAbility = (content == "True");
		}
		else if (child->GetName() == wxT("SubCommand"))
		{
			m_commandMultiNames.push_back(content);
		}
		else
		{
			wxMessageBox(wxString::Format("Unexpected 'Multi' command tag: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2MultiCommand::ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands)
{
	m_buildingRequirements = (SC2BuildingFlags)-1;
	m_unitRequirements = (SC2UnitFlags)-1;
	m_researchRequirements = (SC2ResearchFlags)-1;

	m_willBuildGasBuilding = false;
	m_minimumMineralCost = DBL_MAX;
	m_minimumGasCost = DBL_MAX;

	for(size_t i=0; i < m_commandMultiNames.size(); i++)
	{
		const wxString &commandName = m_commandMultiNames[i];
		
		for(size_t j=0; j < commands.size(); j++)
		{
			if(commandName == commands[j]->GetName())
			{
				m_commandMulti.push_back(commands[j]);
				m_buildingRequirements &= commands[j]->GetBuildingRequirementFlags();
				m_unitRequirements &= commands[j]->GetUnitRequirementFlags();
				m_researchRequirements &= commands[j]->GetResearchRequirementFlags();
				m_willBuildUnit |= commands[j]->WillBuildUnit();
				m_willBuildWorker |= commands[j]->IsBuildWorkerCommand();
				if(commands[j]->GetMineralCost() < m_minimumMineralCost)
					m_minimumMineralCost = commands[j]->GetMineralCost();
				if(commands[j]->GetGasCost() < m_minimumGasCost)
					m_minimumGasCost = commands[j]->GetGasCost();
				if(commands[j]->WillBuildGeyserBuilding())
					m_willBuildGasBuilding = true;
				break;
			}

			if(j >= commands.size() - 1)
			{
				wxMessageBox(wxString::Format("Undefined command '%s'", commandName));
				return false;
			}
		}
	}

	return true;
}

bool CSC2MultiCommand::HasPrerequisits(const CSC2State &state) const
{
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->HasPrerequisits(state))
			return true;
	}

	return false;
}

bool CSC2MultiCommand::HasRequirements(const CSC2State &state) const
{
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->HasRequirements(state))
			return true;
	}

	return false;
}

double CSC2MultiCommand::GetResourceWaitTime(const CSC2State &state) const
{
	double resourceWaitTime = DBL_MAX;

	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(!m_commandMulti[i]->HasRequirements(state))
			continue;

		double commandMultiResourceWaitTime = m_commandMulti[i]->GetResourceWaitTime(state);
		if(commandMultiResourceWaitTime < resourceWaitTime)
			resourceWaitTime = commandMultiResourceWaitTime;
	}

	return resourceWaitTime;
}

bool CSC2MultiCommand::ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const
{
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->HasRequirements(state))
			return m_commandMulti[i]->ExecuteCommand(state, events);
	}

	return true;
}

bool CSC2MultiCommand::IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const
{
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->IsMacroAbility(buildings, units, research))
			return true;
	}

	return false;
}

size_t CSC2MultiCommand::GetProvidedSupply() const
{
	size_t maxProvidedSupply = 0;
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->GetProvidedSupply() > maxProvidedSupply)
			maxProvidedSupply = m_commandMulti[i]->GetProvidedSupply();
	}

	return maxProvidedSupply;
}

size_t CSC2MultiCommand::GetRequiredSupply() const
{
	size_t minRequiredSupply = INT_MAX;
	for(size_t i=0; i < m_commandMulti.size(); i++)
	{
		if(m_commandMulti[i]->GetRequiredSupply() < minRequiredSupply)
			minRequiredSupply = m_commandMulti[i]->GetRequiredSupply();
	}

	return minRequiredSupply;
}
