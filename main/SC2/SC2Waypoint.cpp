#include "stdafx.h"
#include "SC2Waypoint.h"

#include "../Core/Hashtable.h"

CSC2Waypoint::CSC2Waypoint(const CSC2RaceInfo &raceInfo)
: m_minerals(0.0), m_gas(0.0)
, m_mineralsMined(0.0), m_gasMined(0.0)
, m_raceInfo(raceInfo)
, m_buildingMinRequirements(0), m_buildingMaxRequirements(0)
, m_unitMinRequirements(0), m_unitMaxRequirements(0)
, m_researchMinRequirements(0), m_researchMaxRequirements(0)
{
	for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
	{
		m_buildingStatusMinRequirements.push_back(0);
		m_buildingStatusMaxRequirements.push_back(0);
	}
}

void CSC2Waypoint::SetDefaults()
{
	m_supplyCount = CSizeTOptionalMinMax();
	m_baseCount = CSizeTOptionalMinMax();
	m_baseUnderConstructionCount = CSizeTOptionalMinMax();
	m_minerals = 0.0;
	m_gas = 0.0;
	m_mineralsMined = 0.0;
	m_gasMined = 0.0;
	m_buildingMinRequirements = 0;
	m_buildingMaxRequirements = 0;
	m_unitMinRequirements = 0;
	m_unitMaxRequirements = 0;
	m_researchMinRequirements = 0;
	m_researchMaxRequirements = 0;

	m_minRequirements.clear();
	m_maxRequirements.clear();
}

double CSC2Waypoint::targetValue(const CSC2State &state, bool bSatisfied) const
{
	double value = 0;

	value += mymin(m_minerals, state.m_minerals);
	value += 1.5 * mymin(m_gas, state.m_gas);
	value += mymin(m_mineralsMined, state.m_mineralsMined);
	value += 1.5 * mymin(m_gasMined, state.m_gasMined);

	if(m_supplyCount.haveMin)
		value += mymin(m_supplyCount.min, state.m_supply);
	if(m_baseCount.haveMin)
		value += 200.0 * mymin(m_baseCount.min, state.m_baseCount);
	if(m_baseUnderConstructionCount.haveMin)
		value += 200.0 * mymin(m_baseUnderConstructionCount.min, state.m_baseCount);

	for(size_t i=0; i < m_minRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_minRequirements[i];
		value += mymin(requirement.m_count, requirement.m_target->GetStateCount(state)) * 100.0;
	}
	for(size_t i=0; i < m_maxRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_maxRequirements[i];
		size_t count = requirement.m_target->GetStateCount(state);
		if(count > requirement.m_count)
			value += 100.0 / (1 + count - requirement.m_count);
		else
			value += 100.0;
	}

	for(size_t i=0; i < m_minUnderConstructionRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_minUnderConstructionRequirements[i];
		value += mymin(requirement.m_count, requirement.m_target->GetStateCountUnderConstruction(state)) * 100.0;
	}
	for(size_t i=0; i < m_maxUnderConstructionRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_maxUnderConstructionRequirements[i];
		size_t count = requirement.m_target->GetStateCountUnderConstruction(state);
		if(count > requirement.m_count)
			value += 100.0 / (1 + count - requirement.m_count);
		else
			value += 100.0;
	}

	return value;
}

double CSC2Waypoint::targetTimePenalty(const CSC2State &state) const
{
	double value = 0.0;

	return value;
}

void CSC2Waypoint::AddExtraValue(CFitnessValue &fitness, const CSC2State &state) const
{
	for(size_t i=0; i < m_minRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_minRequirements[i];
		size_t count = requirement.m_target->GetStateCount(state);
		if(count > requirement.m_count)
			fitness.m_optionalValue += 100.0 * (count - requirement.m_count);
	}

	for(size_t i=0; i < m_minUnderConstructionRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_minUnderConstructionRequirements[i];
		size_t count = requirement.m_target->GetStateCountUnderConstruction(state);
		if(count > requirement.m_count)
			fitness.m_optionalValue += 50.0 * (count - requirement.m_count);
	}

	fitness.m_economyValue += state.m_workerCount * 50.0;
	fitness.m_resourceValue += state.m_minerals + state.m_gas;
}

bool CSC2Waypoint::hasTarget() const
{
	if(0 < m_supplyCount.min) return true;
	if(0 < m_baseCount.min) return true;
	if(0 < m_baseUnderConstructionCount.min) return true;
	if(0 < m_minerals) return true;
	if(0 < m_gas) return true;
	if(0 < m_mineralsMined) return true;
	if(0 < m_gasMined) return true;
	if(0 < m_minRequirements.size()) return true;
	if(0 < m_maxRequirements.size()) return true;
	if(0 < m_minUnderConstructionRequirements.size()) return true;
	if(0 < m_maxUnderConstructionRequirements.size()) return true;

	return false;
}

bool CSC2Waypoint::satisfiesTarget(const CSC2State &state) const
{
	if(!m_supplyCount.IsWithinRange(state.m_supply))
		return false;
	if(!m_baseCount.IsWithinRange(state.m_baseCount))
		return false;
	if(!m_baseUnderConstructionCount.IsWithinRange(state.m_basesUnderConstruction))
		return false;

	if(state.m_minerals < m_minerals)
		return false;
	if(state.m_gas < m_gas)
		return false;
	if(state.m_mineralsMined < m_mineralsMined)
		return false;
	if(state.m_gasMined < m_gasMined)
		return false;

	for(size_t i=0; i < m_minRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_minRequirements[i];
		if(requirement.m_target->GetStateCount(state) < requirement.m_count)
			return false;
	}

	for(size_t i=0; i < m_maxRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_maxRequirements[i];
		if(requirement.m_target->GetStateCount(state) > requirement.m_count)
			return false;
	}

	for(size_t i=0; i < m_minUnderConstructionRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_minUnderConstructionRequirements[i];
		if(requirement.m_target->GetStateCountUnderConstruction(state) < requirement.m_count)
			return false;
	}

	for(size_t i=0; i < m_maxUnderConstructionRequirements.size(); i++)
	{
		const STargetRequirement &requirement = m_maxUnderConstructionRequirements[i];
		if(requirement.m_target->GetStateCountUnderConstruction(state) > requirement.m_count)
			return false;
	}

	return true;
}

void CSC2Waypoint::AddRequirements()
{
}

void CSC2Waypoint::RestrictRequirements()
{
}

void CSC2Waypoint::operator+=(const CSC2Waypoint &target)
{
	m_minerals = mymax(m_minerals, target.m_minerals);
	m_gas = mymax(m_gas, target.m_gas);
	m_mineralsMined = mymax(m_mineralsMined, target.m_mineralsMined);
	m_gasMined = mymax(m_gasMined, target.m_gasMined);

	// TODO: Add supply, base, & base under construction

	wxHashMapSC2TargetToCount minTargets;
	for(size_t i=0; i < m_minRequirements.size(); i++)
	{
		if(minTargets.end() == minTargets.find(m_minRequirements[i].m_target) || minTargets[m_minRequirements[i].m_target] < m_minRequirements[i].m_count)
			minTargets[m_minRequirements[i].m_target] = m_minRequirements[i].m_count;
	}
	for(size_t i=0; i < target.m_minRequirements.size(); i++)
	{
		if(minTargets.end() == minTargets.find(target.m_minRequirements[i].m_target))
			m_minRequirements.push_back(STargetRequirement(target.m_minRequirements[i].m_target, target.m_minRequirements[i].m_count));
		else
		{
			if(minTargets[target.m_minRequirements[i].m_target] < target.m_minRequirements[i].m_count)
				minTargets[target.m_minRequirements[i].m_target] = target.m_minRequirements[i].m_count;
			else
				minTargets.erase(target.m_minRequirements[i].m_target);
		}
	}

	for(size_t i=0; i < m_minRequirements.size(); i++)
	{
		if(minTargets.end() != minTargets.find(m_minRequirements[i].m_target))
			m_minRequirements[i].m_count = minTargets[m_minRequirements[i].m_target];
	}

	wxHashMapSC2TargetToCount maxTargets;
	for(size_t i=0; i < m_maxRequirements.size(); i++)
	{
		if(maxTargets.end() == maxTargets.find(m_maxRequirements[i].m_target) || maxTargets[m_maxRequirements[i].m_target] < m_maxRequirements[i].m_count)
			maxTargets[m_maxRequirements[i].m_target] = m_maxRequirements[i].m_count;
	}
	for(size_t i=0; i < target.m_maxRequirements.size(); i++)
	{
		if(maxTargets.end() == maxTargets.find(target.m_maxRequirements[i].m_target))
			m_maxRequirements.push_back(STargetRequirement(target.m_maxRequirements[i].m_target, target.m_maxRequirements[i].m_count));
		else
		{
			if(maxTargets[target.m_maxRequirements[i].m_target] < target.m_maxRequirements[i].m_count)
				maxTargets[target.m_maxRequirements[i].m_target] = target.m_maxRequirements[i].m_count;
			else
				maxTargets.erase(target.m_maxRequirements[i].m_target);
		}
	}

	for(size_t i=0; i < m_maxRequirements.size(); i++)
	{
		if(maxTargets.end() != maxTargets.find(m_maxRequirements[i].m_target))
			m_maxRequirements[i].m_count = maxTargets[m_maxRequirements[i].m_target];
	}

	wxHashMapSC2TargetToCount minUCTargets;
	for(size_t i=0; i < m_minUnderConstructionRequirements.size(); i++)
	{
		if(minUCTargets.end() == minUCTargets.find(m_minUnderConstructionRequirements[i].m_target) || minUCTargets[m_minUnderConstructionRequirements[i].m_target] < m_minUnderConstructionRequirements[i].m_count)
			minUCTargets[m_minUnderConstructionRequirements[i].m_target] = m_minUnderConstructionRequirements[i].m_count;
	}
	for(size_t i=0; i < target.m_minUnderConstructionRequirements.size(); i++)
	{
		if(minUCTargets.end() == minUCTargets.find(target.m_minUnderConstructionRequirements[i].m_target))
			m_minUnderConstructionRequirements.push_back(STargetRequirement(target.m_minUnderConstructionRequirements[i].m_target, target.m_minUnderConstructionRequirements[i].m_count));
		else
		{
			if(minUCTargets[target.m_minUnderConstructionRequirements[i].m_target] < target.m_minUnderConstructionRequirements[i].m_count)
				minUCTargets[target.m_minUnderConstructionRequirements[i].m_target] = target.m_minUnderConstructionRequirements[i].m_count;
			else
				minUCTargets.erase(target.m_minUnderConstructionRequirements[i].m_target);
		}
	}

	for(size_t i=0; i < m_minUnderConstructionRequirements.size(); i++)
	{
		if(minUCTargets.end() != minUCTargets.find(m_minUnderConstructionRequirements[i].m_target))
			m_minUnderConstructionRequirements[i].m_count = minUCTargets[m_minUnderConstructionRequirements[i].m_target];
	}

	wxHashMapSC2TargetToCount maxUCTargets;
	for(size_t i=0; i < m_maxUnderConstructionRequirements.size(); i++)
	{
		if(maxUCTargets.end() == maxUCTargets.find(m_maxUnderConstructionRequirements[i].m_target) || maxUCTargets[m_maxUnderConstructionRequirements[i].m_target] < m_maxUnderConstructionRequirements[i].m_count)
			maxUCTargets[m_maxUnderConstructionRequirements[i].m_target] = m_maxUnderConstructionRequirements[i].m_count;
	}
	for(size_t i=0; i < target.m_maxUnderConstructionRequirements.size(); i++)
	{
		if(maxUCTargets.end() == maxUCTargets.find(target.m_maxUnderConstructionRequirements[i].m_target))
			m_maxUnderConstructionRequirements.push_back(STargetRequirement(target.m_maxUnderConstructionRequirements[i].m_target, target.m_maxUnderConstructionRequirements[i].m_count));
		else
		{
			if(maxUCTargets[target.m_maxUnderConstructionRequirements[i].m_target] < target.m_maxUnderConstructionRequirements[i].m_count)
				maxUCTargets[target.m_maxUnderConstructionRequirements[i].m_target] = target.m_maxUnderConstructionRequirements[i].m_count;
			else
				maxUCTargets.erase(target.m_maxUnderConstructionRequirements[i].m_target);
		}
	}

	for(size_t i=0; i < m_maxUnderConstructionRequirements.size(); i++)
	{
		if(maxUCTargets.end() != maxUCTargets.find(m_maxUnderConstructionRequirements[i].m_target))
			m_maxUnderConstructionRequirements[i].m_count = maxUCTargets[m_maxUnderConstructionRequirements[i].m_target];
	}
}

void CSC2Waypoint::DoPreCalculation()
{
	m_buildingMinRequirements = 0;
	m_buildingMaxRequirements = 0;
	m_unitMinRequirements = 0;
	m_unitMaxRequirements = 0;
	m_researchMinRequirements = 0;
	m_researchMaxRequirements = 0;

	for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
	{
		if(m_raceInfo.GetData().m_buildings[buildingTypeID]->IsBase())
			m_buildingMaxRequirements |= ((SC2BuildingFlags)1 << buildingTypeID);
	}

	for(size_t unitTypeID=0; unitTypeID < m_raceInfo.GetData().m_units.size(); unitTypeID++)
	{
		if(m_raceInfo.GetData().m_units[unitTypeID]->IsWorker())
			m_unitMaxRequirements |= ((SC2UnitFlags)1 << unitTypeID);
	}

	for(size_t i=0; i < m_minRequirements.size(); i++)
	{
		const CSC2Target *target = m_minRequirements[i].m_target;

		m_buildingMinRequirements |= target->GetBuildingRequirementFlags();
		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
			m_buildingStatusMinRequirements[buildingTypeID] |= target->GetBuildingStatusRequirementFlags(buildingTypeID);
		m_unitMinRequirements |= target->GetUnitRequirementFlags();
		m_researchMinRequirements |= target->GetResearchRequirementFlags();

		m_buildingMaxRequirements |= target->GetBuildingRequirementFlags();
		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
			m_buildingStatusMaxRequirements[buildingTypeID] |= target->GetBuildingStatusRequirementFlags(buildingTypeID);
		m_unitMaxRequirements |= target->GetUnitRequirementFlags();
		m_researchMaxRequirements |= target->GetResearchRequirementFlags();
	}

	for(size_t i=0; i < m_minUnderConstructionRequirements.size(); i++)
	{
		const CSC2Target *target = m_minUnderConstructionRequirements[i].m_target;

		m_buildingMinRequirements |= target->GetBuildingRequirementFlags();
		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
			m_buildingStatusMinRequirements[buildingTypeID] |= target->GetBuildingStatusRequirementFlags(buildingTypeID);
		m_unitMinRequirements |= target->GetUnitRequirementFlags();
		m_researchMinRequirements |= target->GetResearchRequirementFlags();

		m_buildingMaxRequirements |= target->GetBuildingRequirementFlags();
		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
			m_buildingStatusMaxRequirements[buildingTypeID] |= target->GetBuildingStatusRequirementFlags(buildingTypeID);
		m_unitMaxRequirements |= target->GetUnitRequirementFlags();
		m_researchMaxRequirements |= target->GetResearchRequirementFlags();
	}

	for(size_t i=0; i < m_maxRequirements.size(); i++)
	{
		const CSC2Target *target = m_maxRequirements[i].m_target;
		m_buildingMaxRequirements |= target->GetBuildingRequirementFlags();
		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
			m_buildingStatusMaxRequirements[buildingTypeID] |= target->GetBuildingStatusRequirementFlags(buildingTypeID);
		m_unitMaxRequirements |= target->GetUnitRequirementFlags();
		m_researchMaxRequirements |= target->GetResearchRequirementFlags();
	}

	for(size_t i=0; i < m_maxUnderConstructionRequirements.size(); i++)
	{
		const CSC2Target *target = m_maxUnderConstructionRequirements[i].m_target;
		m_buildingMaxRequirements |= target->GetBuildingRequirementFlags();
		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
			m_buildingStatusMaxRequirements[buildingTypeID] |= target->GetBuildingStatusRequirementFlags(buildingTypeID);
		m_unitMaxRequirements |= target->GetUnitRequirementFlags();
		m_researchMaxRequirements |= target->GetResearchRequirementFlags();
	}


	SC2BuildingFlags oldBuildingMinRequirements = 0;
	CVector<SC2BuildingStatusFlags> oldBuildingMinStatusRequirements;
	for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
		oldBuildingMinStatusRequirements.push_back(0);
	SC2UnitFlags oldUnitMinRequirements = 0;
	SC2ResearchFlags oldResearchMinRequirements = 0;
	bool oldRequiresGas = false;

	while(oldBuildingMinRequirements != m_buildingMinRequirements
		|| oldUnitMinRequirements != m_unitMinRequirements
		|| oldResearchMinRequirements != m_researchMinRequirements
		|| oldBuildingMinStatusRequirements != m_buildingStatusMinRequirements)
	{
		SC2BuildingFlags newBuildingMinRequirements = m_buildingMinRequirements;
		CVector<SC2BuildingStatusFlags> newBuildingMinStatusRequirements;
		for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
			newBuildingMinStatusRequirements.push_back(m_buildingStatusMinRequirements[i]);
		SC2UnitFlags newUnitMinRequirements = m_unitMinRequirements;
		SC2ResearchFlags newResearchMinRequirements = m_researchMinRequirements;
		bool newRequiresGas = oldRequiresGas;

		SC2BuildingFlags buildingFlags = m_buildingMinRequirements - oldBuildingMinRequirements;
		for(size_t buildingTypeID = 0; buildingFlags > 0; buildingTypeID++)
		{
			if(buildingFlags & 0x1)
			{
				//wxMessageBox(m_raceInfo.GetData().m_buildings[buildingTypeID]->GetName());
				const CVector<const CSC2Command *> *commandList = m_raceInfo.GetBuildingPrerequisitCommands()[buildingTypeID];
				if(commandList && 0 < commandList->size())
				{
					SC2BuildingFlags addBuildingFlags = (SC2BuildingFlags)-1;
					SC2UnitFlags addUnitFlags = (SC2UnitFlags)-1;
					SC2ResearchFlags addResearchFlags = (SC2ResearchFlags)-1;
					bool addRequiresGas = true;

					for(size_t i=0; i < commandList->size(); i++)
					{
						addBuildingFlags &= (*commandList)[i]->GetBuildingRequirementFlags();
						addUnitFlags &= (*commandList)[i]->GetUnitRequirementFlags();
						addResearchFlags &= (*commandList)[i]->GetResearchRequirementFlags();
						if(0 == (*commandList)[i]->GetGasCost())
							addRequiresGas = false;
					}

					newBuildingMinRequirements |= addBuildingFlags;
					newUnitMinRequirements |= addUnitFlags;
					newResearchMinRequirements |= addResearchFlags;
					if(addRequiresGas)
						newRequiresGas = true;
				}
			}

			buildingFlags >>= 1;
		}

		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
		{
			SC2BuildingStatusFlags buildingStatusFlags = m_buildingStatusMaxRequirements[buildingTypeID] - oldBuildingMinStatusRequirements[buildingTypeID];
			for(size_t buildingStatusID = 0; buildingStatusFlags > 0; buildingStatusID++)
			{
				if(buildingStatusFlags & 0x1)
				{
					const CVector<const CSC2Command *> *commandList = (*m_raceInfo.GetBuildingStatusPrerequisitCommands()[buildingTypeID])[buildingStatusID];
					if(commandList && 0 < commandList->size())
					{
						SC2BuildingFlags addBuildingFlags = 0;
						SC2UnitFlags addUnitFlags = 0;
						SC2ResearchFlags addResearchFlags = 0;
						bool addRequiresGas = false;

						for(size_t i=0; i < commandList->size(); i++)
						{
							addBuildingFlags |= (*commandList)[i]->GetBuildingRequirementFlags();
							addUnitFlags |= (*commandList)[i]->GetUnitRequirementFlags();
							addResearchFlags |= (*commandList)[i]->GetResearchRequirementFlags();
							if(0.0 < (*commandList)[i]->GetGasCost())
								addRequiresGas = true;
						}

						newBuildingMinRequirements |= addBuildingFlags;
						newUnitMinRequirements |= addUnitFlags;
						newResearchMinRequirements |= addResearchFlags;
						if(addRequiresGas)
							newRequiresGas = true;
					}
				}

				buildingStatusFlags >>= 1;
			}
		}

		SC2UnitFlags unitFlags = m_unitMinRequirements - oldUnitMinRequirements;
		for(size_t unitTypeID = 0; unitFlags > 0; unitTypeID++)
		{
			if(unitFlags & 0x1)
			{
				const CVector<const CSC2Command *> *commandList = m_raceInfo.GetUnitPrerequisitCommands()[unitTypeID];
				if(commandList && 0 < commandList->size())
				{
					SC2BuildingFlags addBuildingFlags = (SC2BuildingFlags)-1;
					SC2UnitFlags addUnitFlags = (SC2UnitFlags)-1;
					SC2ResearchFlags addResearchFlags = (SC2ResearchFlags)-1;
					bool addRequiresGas = true;

					for(size_t i=0; i < commandList->size(); i++)
					{
						addBuildingFlags &= (*commandList)[i]->GetBuildingRequirementFlags();
						addUnitFlags &= (*commandList)[i]->GetUnitRequirementFlags();
						addResearchFlags &= (*commandList)[i]->GetResearchRequirementFlags();
						if(0 == (*commandList)[i]->GetGasCost())
							addRequiresGas = false;
					}

					newBuildingMinRequirements |= addBuildingFlags;
					newUnitMinRequirements |= addUnitFlags;
					newResearchMinRequirements |= addResearchFlags;
					if(addRequiresGas)
						newRequiresGas = true;
				}
			}

			unitFlags >>= 1;
		}

		SC2ResearchFlags researchFlags = m_researchMinRequirements - oldResearchMinRequirements;
		for(size_t researchID = 0; researchFlags > 0; researchID++)
		{
			if(researchFlags & 0x1)
			{
				const CVector<const CSC2Command *> *commandList = m_raceInfo.GetResearchPrerequisitCommands()[researchID];
				if(commandList && 0 < commandList->size())
				{
					SC2BuildingFlags addBuildingFlags = (SC2BuildingFlags)-1;
					SC2UnitFlags addUnitFlags = (SC2UnitFlags)-1;
					SC2ResearchFlags addResearchFlags = (SC2ResearchFlags)-1;
					bool addRequiresGas = true;

					for(size_t i=0; i < commandList->size(); i++)
					{
						addBuildingFlags &= (*commandList)[i]->GetBuildingRequirementFlags();
						addUnitFlags &= (*commandList)[i]->GetUnitRequirementFlags();
						addResearchFlags &= (*commandList)[i]->GetResearchRequirementFlags();
						if(0 == (*commandList)[i]->GetGasCost())
							addRequiresGas = false;
					}

					newBuildingMinRequirements |= addBuildingFlags;
					newUnitMinRequirements |= addUnitFlags;
					newResearchMinRequirements |= addResearchFlags;
					if(addRequiresGas)
						newRequiresGas = true;
				}
			}

			researchFlags >>= 1;
		}

		if(newRequiresGas && !oldRequiresGas)
		{
			const CVector<const CSC2Command *> &commandList = m_raceInfo.GetBuildGeyserBuildingCommands();
			if(0 < commandList.size())
			{
				SC2BuildingFlags addBuildingFlags = (SC2BuildingFlags)-1;
				SC2UnitFlags addUnitFlags = (SC2UnitFlags)-1;
				SC2ResearchFlags addResearchFlags = (SC2ResearchFlags)-1;

				for(size_t i=0; i < commandList.size(); i++)
				{
					addBuildingFlags &= commandList[i]->GetBuildingRequirementFlags();
					addUnitFlags &= commandList[i]->GetUnitRequirementFlags();
					addResearchFlags &= commandList[i]->GetResearchRequirementFlags();
				}

				newBuildingMinRequirements |= addBuildingFlags;
				newUnitMinRequirements |= addUnitFlags;
				newResearchMinRequirements |= addResearchFlags;
			}
		}

		oldBuildingMinRequirements = m_buildingMinRequirements;
		for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
			oldBuildingMinStatusRequirements[i] = m_buildingStatusMinRequirements[i];
		oldUnitMinRequirements = m_unitMinRequirements;
		oldResearchMinRequirements = m_researchMinRequirements;
		oldRequiresGas = newRequiresGas;

		m_buildingMinRequirements = newBuildingMinRequirements;
		for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
			m_buildingStatusMinRequirements[i] = newBuildingMinStatusRequirements[i];
		m_unitMinRequirements = newUnitMinRequirements;
		m_researchMinRequirements = newResearchMinRequirements;
	}

	SC2BuildingFlags oldBuildingMaxRequirements = 0;
	CVector<SC2BuildingStatusFlags> oldBuildingMaxStatusRequirements;
	for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
		oldBuildingMaxStatusRequirements.push_back(0);
	SC2UnitFlags oldUnitMaxRequirements = 0;
	SC2ResearchFlags oldResearchMaxRequirements = 0;
	oldRequiresGas = false;

	CHashtable<const CSC2Command *, const CSC2Command *, void *, void *> macroCommandSet;

	while(oldBuildingMaxRequirements != m_buildingMaxRequirements
		|| oldUnitMaxRequirements != m_unitMaxRequirements
		|| oldResearchMaxRequirements != m_researchMaxRequirements
		|| oldBuildingMaxStatusRequirements != m_buildingStatusMaxRequirements)
	{
		SC2BuildingFlags newBuildingMaxRequirements = m_buildingMaxRequirements;
		CVector<SC2BuildingStatusFlags> newBuildingMaxStatusRequirements;
		for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
			newBuildingMaxStatusRequirements.push_back(m_buildingStatusMinRequirements[i]);
		SC2UnitFlags newUnitMaxRequirements = m_unitMaxRequirements;
		SC2ResearchFlags newResearchMaxRequirements = m_researchMaxRequirements;
		bool newRequiresGas = oldRequiresGas;

		SC2BuildingFlags buildingFlags = m_buildingMaxRequirements - oldBuildingMaxRequirements;
		for(size_t buildingTypeID = 0; buildingFlags > 0; buildingTypeID++)
		{
			if(buildingFlags & 0x1)
			{
				const CVector<const CSC2Command *> *commandList = m_raceInfo.GetBuildingPrerequisitCommands()[buildingTypeID];
				if(commandList && 0 < commandList->size())
				{
					SC2BuildingFlags addBuildingFlags = 0;
					SC2UnitFlags addUnitFlags = 0;
					SC2ResearchFlags addResearchFlags = 0;
					bool addRequiresGas = false;

					for(size_t i=0; i < commandList->size(); i++)
					{
						addBuildingFlags |= (*commandList)[i]->GetBuildingRequirementFlags();
						addUnitFlags |= (*commandList)[i]->GetUnitRequirementFlags();
						addResearchFlags |= (*commandList)[i]->GetResearchRequirementFlags();
						if(0.0 < (*commandList)[i]->GetGasCost())
							addRequiresGas = true;
					}

					newBuildingMaxRequirements |= addBuildingFlags;
					newUnitMaxRequirements |= addUnitFlags;
					newResearchMaxRequirements |= addResearchFlags;
					if(addRequiresGas)
						newRequiresGas = true;
				}
			}

			buildingFlags >>= 1;
		}

		for(size_t buildingTypeID=0; buildingTypeID < m_raceInfo.GetData().m_buildings.size(); buildingTypeID++)
		{
			SC2BuildingStatusFlags buildingStatusFlags = m_buildingStatusMaxRequirements[buildingTypeID] - oldBuildingMaxStatusRequirements[buildingTypeID];
			for(size_t buildingStatusID = 0; buildingStatusFlags > 0; buildingStatusID++)
			{
				if(buildingStatusFlags & 0x1)
				{
					const CVector<const CSC2Command *> *commandList = (*m_raceInfo.GetBuildingStatusPrerequisitCommands()[buildingTypeID])[buildingStatusID];
					if(commandList && 0 < commandList->size())
					{
						SC2BuildingFlags addBuildingFlags = 0;
						SC2UnitFlags addUnitFlags = 0;
						SC2ResearchFlags addResearchFlags = 0;
						bool addRequiresGas = false;

						for(size_t i=0; i < commandList->size(); i++)
						{
							addBuildingFlags |= (*commandList)[i]->GetBuildingRequirementFlags();
							addUnitFlags |= (*commandList)[i]->GetUnitRequirementFlags();
							addResearchFlags |= (*commandList)[i]->GetResearchRequirementFlags();
							if(0.0 < (*commandList)[i]->GetGasCost())
								addRequiresGas = true;
						}

						newBuildingMaxRequirements |= addBuildingFlags;
						newUnitMaxRequirements |= addUnitFlags;
						newResearchMaxRequirements |= addResearchFlags;
						if(addRequiresGas)
							newRequiresGas = true;
					}
				}

				buildingStatusFlags >>= 1;
			}
		}

		SC2UnitFlags unitFlags = m_unitMaxRequirements - oldUnitMaxRequirements;
		for(size_t unitTypeID = 0; unitFlags > 0; unitTypeID++)
		{
			if(unitFlags & 0x1)
			{
				const CVector<const CSC2Command *> *commandList = m_raceInfo.GetUnitPrerequisitCommands()[unitTypeID];
				if(commandList && 0 < commandList->size())
				{
					SC2BuildingFlags addBuildingFlags = 0;
					SC2UnitFlags addUnitFlags = 0;
					SC2ResearchFlags addResearchFlags = 0;
					bool addRequiresGas = false;

					for(size_t i=0; i < commandList->size(); i++)
					{
						addBuildingFlags |= (*commandList)[i]->GetBuildingRequirementFlags();
						addUnitFlags |= (*commandList)[i]->GetUnitRequirementFlags();
						addResearchFlags |= (*commandList)[i]->GetResearchRequirementFlags();
						if(0.0 < (*commandList)[i]->GetGasCost())
							addRequiresGas = true;
					}

					newBuildingMaxRequirements |= addBuildingFlags;
					newUnitMaxRequirements |= addUnitFlags;
					newResearchMaxRequirements |= addResearchFlags;
					if(addRequiresGas)
						newRequiresGas = true;
				}
			}

			unitFlags >>= 1;
		}

		SC2ResearchFlags researchFlags = m_researchMaxRequirements - oldResearchMaxRequirements;
		for(size_t researchID = 0; researchFlags > 0; researchID++)
		{
			if(researchFlags & 0x1)
			{
				const CVector<const CSC2Command *> *commandList = m_raceInfo.GetResearchPrerequisitCommands()[researchID];
				if(commandList && 0 < commandList->size())
				{
					SC2BuildingFlags addBuildingFlags = 0;
					SC2UnitFlags addUnitFlags = 0;
					SC2ResearchFlags addResearchFlags = 0;
					bool addRequiresGas = false;

					for(size_t i=0; i < commandList->size(); i++)
					{
						addBuildingFlags |= (*commandList)[i]->GetBuildingRequirementFlags();
						addUnitFlags |= (*commandList)[i]->GetUnitRequirementFlags();
						addResearchFlags |= (*commandList)[i]->GetResearchRequirementFlags();
						if(0.0 < (*commandList)[i]->GetGasCost())
							addRequiresGas = true;
					}

					newBuildingMaxRequirements |= addBuildingFlags;
					newUnitMaxRequirements |= addUnitFlags;
					newResearchMaxRequirements |= addResearchFlags;
					if(addRequiresGas)
						newRequiresGas = true;
				}
			}

			researchFlags >>= 1;
		}

		if(newRequiresGas && !oldRequiresGas)
		{
			const CVector<const CSC2Command *> &buildGeysercommandList = m_raceInfo.GetBuildGeyserBuildingCommands();
			if(0 < buildGeysercommandList.size())
			{
				SC2BuildingFlags addBuildingFlags = 0;
				SC2UnitFlags addUnitFlags = 0;
				SC2ResearchFlags addResearchFlags = 0;

				for(size_t i=0; i < buildGeysercommandList.size(); i++)
				{
					addBuildingFlags |= buildGeysercommandList[i]->GetBuildingRequirementFlags();
					addUnitFlags |= buildGeysercommandList[i]->GetUnitRequirementFlags();
					addResearchFlags |= buildGeysercommandList[i]->GetResearchRequirementFlags();
				}

				newBuildingMaxRequirements |= addBuildingFlags;
				newUnitMaxRequirements |= addUnitFlags;
				newResearchMaxRequirements |= addResearchFlags;
			}

			const CVector<const CSC2Command *> &moveWorkerCommandList = m_raceInfo.GetMoveWorkersToGasCommands();
			if(0 < moveWorkerCommandList.size())
			{
				SC2BuildingFlags addBuildingFlags = 0;
				SC2UnitFlags addUnitFlags = 0;
				SC2ResearchFlags addResearchFlags = 0;

				for(size_t i=0; i < moveWorkerCommandList.size(); i++)
				{
					if(moveWorkerCommandList[i])
					{
						addBuildingFlags |= moveWorkerCommandList[i]->GetBuildingRequirementFlags();
						addUnitFlags |= moveWorkerCommandList[i]->GetUnitRequirementFlags();
						addResearchFlags |= moveWorkerCommandList[i]->GetResearchRequirementFlags();
					}
				}

				newBuildingMaxRequirements |= addBuildingFlags;
				newUnitMaxRequirements |= addUnitFlags;
				newResearchMaxRequirements |= addResearchFlags;
			}
		}

		for(size_t i=0; i < m_raceInfo.GetMacroAbilityCommands().size(); i++)
		{
			const CSC2Command *command = m_raceInfo.GetMacroAbilityCommands()[i];
			if(macroCommandSet.ContainsKey(command))
				continue;

			if(command->IsMacroAbility(newBuildingMaxRequirements, newUnitMaxRequirements, newResearchMaxRequirements))
			{
				macroCommandSet.SetAt(command, NULL);
				newBuildingMaxRequirements |= command->GetBuildingRequirementFlags();
				newUnitMaxRequirements |= command->GetUnitRequirementFlags();
				newResearchMaxRequirements |= command->GetResearchRequirementFlags();
			}
		}

		oldBuildingMaxRequirements = m_buildingMaxRequirements;
		for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
			oldBuildingMaxStatusRequirements[i] = m_buildingStatusMaxRequirements[i];
		oldUnitMaxRequirements = m_unitMaxRequirements;
		oldResearchMaxRequirements = m_researchMaxRequirements;
		oldRequiresGas = newRequiresGas;

		m_buildingMaxRequirements = newBuildingMaxRequirements;
		for(size_t i=0; i < m_raceInfo.GetData().m_buildings.size(); i++)
			m_buildingStatusMaxRequirements[i] = newBuildingMaxStatusRequirements[i];
		m_unitMaxRequirements = newUnitMaxRequirements;
		m_researchMaxRequirements = newResearchMaxRequirements;
	}
}

void CSC2Waypoint::BuildAlphabet(CVector<const CSC2Command *> &alphabet, EGasMicro gasMicro) const
{
	bool requiresGas = false;

	CHashtable<const CSC2Command *, const CSC2Command *, void *, void *> commandSet;

	// Building commands
	SC2BuildingFlags buildingFlags = m_buildingMaxRequirements;
	for(size_t buildingTypeID = 0; buildingFlags > 0; buildingTypeID++)
	{
		if(buildingFlags & 0x1)
		{
			const CVector<const CSC2Command *> *commandList = m_raceInfo.GetBuildingPrerequisitCommands()[buildingTypeID];
			if(commandList)
			{
				for(size_t i=0; i < commandList->size(); i++)
				{
					commandSet.SetAt((*commandList)[i], NULL);
					if(!requiresGas && 0.0 < (*commandList)[i]->GetGasCost())
						requiresGas = true;
				}
			}
		}

		buildingFlags >>= 1;
	}

	// Building status commands
	for(size_t buildingTypeID=0; buildingTypeID < m_buildingStatusMaxRequirements.size(); buildingTypeID++)
	{
		SC2BuildingFlags buildingStatusFlags = m_buildingStatusMaxRequirements[buildingTypeID];
		for(size_t buildingStatusTypeID = 0; buildingStatusFlags > 0; buildingStatusTypeID++)
		{
			if(buildingStatusFlags & 0x1)
			{
				const CVector<const CSC2Command *> *commandList = (*m_raceInfo.GetBuildingStatusPrerequisitCommands()[buildingTypeID])[buildingStatusTypeID];
				if(commandList)
				{
					for(size_t i=0; i < commandList->size(); i++)
					{
						commandSet.SetAt((*commandList)[i], NULL);
						if(!requiresGas && 0.0 < (*commandList)[i]->GetGasCost())
							requiresGas = true;
					}
				}
			}

			buildingStatusFlags >>= 1;
		}
	}

	// Unit commands
	SC2UnitFlags unitFlags = m_unitMaxRequirements;
	for(size_t unitTypeID = 0; unitFlags > 0; unitTypeID++)
	{
		if(unitFlags & 0x1)
		{
			const CVector<const CSC2Command *> *commandList = m_raceInfo.GetUnitPrerequisitCommands()[unitTypeID];
			if(commandList)
			{
				for(size_t i=0; i < commandList->size(); i++)
				{
					commandSet.SetAt((*commandList)[i], NULL);
					if(!requiresGas && 0.0 < (*commandList)[i]->GetGasCost())
						requiresGas = true;
				}
			}
		}

		unitFlags >>= 1;
	}

	// Research commands
	SC2ResearchFlags researchFlags = m_researchMaxRequirements;
	for(size_t researchID = 0; researchFlags > 0; researchID++)
	{
		if(researchFlags & 0x1)
		{
			const CVector<const CSC2Command *> *commandList = m_raceInfo.GetResearchPrerequisitCommands()[researchID];
			if(commandList)
			{
				for(size_t i=0; i < commandList->size(); i++)
				{
					commandSet.SetAt((*commandList)[i], NULL);
					if(!requiresGas && 0.0 < (*commandList)[i]->GetGasCost())
						requiresGas = true;
				}
			}
		}

		researchFlags >>= 1;
	}

	// Move worker commands
	if(requiresGas)
	{
		switch(gasMicro)
		{
		case eGasMicroOneToGasOnly:
			if(m_raceInfo.GetMoveWorkersToGasCommands()[1])
				commandSet.SetAt(m_raceInfo.GetMoveWorkersToGasCommands()[1], NULL);
			break;
		case eGasMicroOneBackAndForth:
			if(m_raceInfo.GetMoveWorkersToGasCommands()[1])
				commandSet.SetAt(m_raceInfo.GetMoveWorkersToGasCommands()[1], NULL);
			if(m_raceInfo.GetMoveWorkersToMineralsCommands()[1])
				commandSet.SetAt(m_raceInfo.GetMoveWorkersToMineralsCommands()[1], NULL);
			break;
		case eGasMicroThreeToGasOnly:
			if(m_raceInfo.GetMoveWorkersToGasCommands()[3])
				commandSet.SetAt(m_raceInfo.GetMoveWorkersToGasCommands()[3], NULL);
			break;
		case eGasMicroThreeBackAndForth:
			if(m_raceInfo.GetMoveWorkersToGasCommands()[3])
				commandSet.SetAt(m_raceInfo.GetMoveWorkersToGasCommands()[3], NULL);
			if(m_raceInfo.GetMoveWorkersToMineralsCommands()[3])
				commandSet.SetAt(m_raceInfo.GetMoveWorkersToMineralsCommands()[3], NULL);
			break;
		}
	}

	// Macro ability commands
	for(size_t i=0; i < m_raceInfo.GetMacroAbilityCommands().size(); i++)
	{
		const CSC2Command *command = m_raceInfo.GetMacroAbilityCommands()[i];
		if((command->GetBuildingRequirementFlags() & m_buildingMaxRequirements) == command->GetBuildingRequirementFlags()
			&& (command->GetUnitRequirementFlags() & m_unitMaxRequirements) == command->GetUnitRequirementFlags()
			&& (command->GetResearchRequirementFlags() & m_researchMaxRequirements) == command->GetResearchRequirementFlags())
			commandSet.SetAt(command, NULL);
	}

	const CSC2Command *command;
	void *val;
	void *pos = commandSet.GetStartPosition();
	while(pos)
	{
		commandSet.GetNextAssoc(pos, command, val);
		if(command->IsAvailableToGeneticAlgorithm())
			alphabet.push_back(command);
	}
}

void CSC2Waypoint::AddStateDuration(CFitnessValue &fitness, const CSC2State &state, double time) const
{
	for(size_t i=0; i < m_stateDurationRequirements.size(); i++)
		fitness.m_requirementsValue -= m_stateDurationRequirements[i]->GetStateDurationPenalty(state, time);
}
