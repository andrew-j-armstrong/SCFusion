#include "stdafx.h"
#include "SC2State.h"
#include "SC2Event.h"
#include "GameCalcs.h"

CSC2State::CSC2State(ESC2Race race, const CSC2RaceData &raceData)
	: m_race(race), m_raceData(raceData)
	, m_time(0.0)
	, m_minerals(0.0)
	, m_gas(0.0)
	, m_mineralsMined(0.0)
	, m_gasMined(0.0)
	, m_bonusMineralIncomeRate(0.0)
	, m_mineralIncomeRate(0.0)
	, m_gasIncomeRate(0.0)
	, m_baseCount(0)
	, m_geyserCount(0)
	, m_basesUnderConstruction(0)
	, m_geysersUnderConstruction(0)
	, m_buildingCompletedFlags(0)
	, m_buildingUnderConstructionFlags(0)
	, m_buildingUnderConstruction()
	, m_buildingMorphing()
	, m_buildings()
	, m_allBuildings()
	, m_larvaeSpawningBuildings()
	, m_totalLarvaeCount(0)
	, m_workerCount(0)
	, m_workersOnMinerals(0)
	, m_workersOnGas(0)
	, m_workersMovingToMinerals(0)
	, m_workersMovingToGas(0)
	, m_unitCompletedFlags(0)
	, m_unitUnderConstructionFlags(0)
	, m_unitUnderConstruction()
	, m_units()
	, m_allUnits()
	, m_researchCompletedFlags(0)
	, m_researchUnderConstructionFlags(0)
	, m_researchCompleted()
	, m_researchUnderConstruction()
	, m_supply(0)
	, m_supplyCap(0)
	, m_supplyCapUnderConstruction(0)
{
	m_buildingUnderConstruction.capacity(m_raceData.m_buildings.size());
	m_buildingMorphing.capacity(m_raceData.m_buildings.size());
	m_buildings.capacity(m_raceData.m_buildings.size());
	for (size_t i = 0; i < m_raceData.m_buildings.size(); i++)
	{
		m_buildingUnderConstruction.push_back(0);
		m_buildingMorphing.push_back(0);
		m_buildings.push_back(new CBuildingStateList(*m_raceData.m_buildings[i]));
	}
	m_unitUnderConstruction.capacity(m_raceData.m_units.size());
	m_units.capacity(m_raceData.m_units.size());
	for (size_t i = 0; i < m_raceData.m_units.size(); i++)
	{
		m_unitUnderConstruction.push_back(0);
		m_units.push_back(new CUnitStateList(*m_raceData.m_units[i]));
	}
	m_researchCompleted.capacity(m_raceData.m_research.size());
	m_researchUnderConstruction.capacity(m_raceData.m_research.size());
	for (size_t i = 0; i < m_raceData.m_research.size(); i++)
	{
		m_researchCompleted.push_back(0);
		m_researchUnderConstruction.push_back(0);
	}

	m_allBuildings.capacity(20); // Capacity for 20 buildings to start with
	m_allUnits.capacity(50); // Capacity for 50 buildings to start with
	m_allBuildings.push_back(NULL); // Index 0 is reserved
	m_allUnits.push_back(NULL); // Index 0 is reserved
}

CSC2State::CSC2State(const CSC2State &state)
	: m_race(state.m_race), m_raceData(state.m_raceData)
	, m_time(state.m_time)
	, m_minerals(state.m_minerals)
	, m_gas(state.m_gas)
	, m_mineralsMined(state.m_mineralsMined)
	, m_gasMined(state.m_gasMined)
	, m_bonusMineralIncomeRate(state.m_bonusMineralIncomeRate)
	, m_mineralIncomeRate(state.m_mineralIncomeRate)
	, m_gasIncomeRate(state.m_gasIncomeRate)
	, m_baseCount(state.m_baseCount)
	, m_geyserCount(state.m_geyserCount)
	, m_basesUnderConstruction(state.m_basesUnderConstruction)
	, m_geysersUnderConstruction(state.m_geysersUnderConstruction)
	, m_buildingCompletedFlags(state.m_buildingCompletedFlags)
	, m_buildingUnderConstructionFlags(state.m_buildingUnderConstructionFlags)
	, m_buildingUnderConstruction(state.m_buildingUnderConstruction)
	, m_buildingMorphing(state.m_buildingMorphing)
	, m_buildings()
	, m_allBuildings()
	, m_totalLarvaeCount(state.m_totalLarvaeCount)
	, m_workerCount(state.m_workerCount)
	, m_workersOnMinerals(state.m_workersOnMinerals)
	, m_workersOnGas(state.m_workersOnGas)
	, m_workersMovingToMinerals(state.m_workersMovingToMinerals)
	, m_workersMovingToGas(state.m_workersMovingToGas)
	, m_unitCompletedFlags(state.m_unitCompletedFlags)
	, m_unitUnderConstructionFlags(state.m_unitUnderConstructionFlags)
	, m_unitUnderConstruction(state.m_unitUnderConstruction)
	, m_units()
	, m_allUnits()
	, m_researchCompletedFlags(state.m_researchCompletedFlags)
	, m_researchUnderConstructionFlags(state.m_researchUnderConstructionFlags)
	, m_researchCompleted(state.m_researchCompleted)
	, m_researchUnderConstruction(state.m_researchUnderConstruction)
	, m_supply(state.m_supply)
	, m_supplyCap(state.m_supplyCap)
	, m_supplyCapUnderConstruction(state.m_supplyCapUnderConstruction)
{
	m_buildings.capacity(m_raceData.m_buildings.size());
	for(size_t i=0; i < m_raceData.m_buildings.size(); i++)
	{
		CBuildingStateList *buildingStateList = new CBuildingStateList(*m_raceData.m_buildings[i]);
		m_buildings.push_back(buildingStateList);
		for(size_t j=0; j < state.m_buildings[i]->size(); j++)
		{
			SBuildingState *buildingState = new SBuildingState(*state.m_buildings[i]->m_buildingList[j]);
			buildingStateList->m_buildingList.push_back(buildingState);
			while(m_allBuildings.size() <= buildingState->buildingID)
				m_allBuildings.push_back(NULL);
			m_allBuildings[buildingState->buildingID] = buildingState;
			if(buildingStateList->m_building.GetMaxLarvaeCount() > 0)
				m_larvaeSpawningBuildings.push_back(buildingState);
		}
	}
	for(size_t i=0; i < state.m_allBuildings.size(); i++)
	{
		while(i >= m_allBuildings.size())
			m_allBuildings.push_back(NULL);

		if(NULL == state.m_allBuildings[i])
			continue;
		else if(i >= m_allBuildings.size() || NULL == m_allBuildings[i])
		{
			SBuildingState *buildingState = new SBuildingState(*state.m_allBuildings[i]);
			m_allBuildings[i] = buildingState;
			if(m_buildings[buildingState->buildingTypeID]->m_building.GetMaxLarvaeCount() > 0)
				m_larvaeSpawningBuildings.push_back(buildingState);
		}	
	}

	m_units.capacity(m_raceData.m_units.size());
	for(size_t i=0; i < m_raceData.m_units.size(); i++)
	{
		CUnitStateList *unitStateList = new CUnitStateList(*m_raceData.m_units[i]);
		m_units.push_back(unitStateList);
		for(size_t j=0; j < state.m_units[i]->size(); j++)
		{
			SUnitState *unitState = new SUnitState(*state.m_units[i]->m_unitList[j]);
			unitStateList->m_unitList.push_back(unitState);
			while(m_allUnits.size() <= unitState->unitID)
				m_allUnits.push_back(NULL);
			m_allUnits[unitState->unitID] = unitState;
		}
	}
	for(size_t i=0; i < state.m_allUnits.size(); i++)
	{
		while(i >= m_allUnits.size())
			m_allUnits.push_back(NULL);

		if(NULL == state.m_allUnits[i])
			continue;
		else if(i >= m_allUnits.size() || NULL == m_allUnits[i])
		{
			SUnitState *unitState = new SUnitState(*state.m_allUnits[i]);
			m_allUnits[i] = unitState;
		}	
	}
}

CSC2State::~CSC2State()
{
	RemoveAllPointer(m_buildings);
	RemoveAllPointer(m_allBuildings);
	RemoveAllPointer(m_units);
	RemoveAllPointer(m_allUnits);
}

void CSC2State::SetInitialState(CPriorityQueue<CSC2Event> &events)
{
	m_time = 0.0;
	m_minerals = 0.0;
	m_gas = 0.0;
	m_mineralsMined = 0.0;
	m_gasMined = 0.0;
	m_bonusMineralIncomeRate = 0.0;
	m_mineralIncomeRate = 0.0;
	m_gasIncomeRate = 0.0;
	m_baseCount = 0;
	m_geyserCount = 0;
	m_basesUnderConstruction = 0;
	m_geysersUnderConstruction = 0;
	m_buildingCompletedFlags = 0;
	m_buildingUnderConstructionFlags = 0;
	for(size_t i=0; i < m_buildingUnderConstruction.size(); i++)
		m_buildingUnderConstruction[i] = 0;
	for(size_t i=0; i < m_buildingMorphing.size(); i++)
		m_buildingMorphing[i] = 0;
	for(size_t i=0; i < m_buildings.size(); i++)
		m_buildings[i]->m_buildingList.clear();
	RemoveAllPointer(m_allBuildings);
	m_allBuildings.push_back(NULL); // Index 0 is reserved
	m_workerCount = 0;
	m_workersOnMinerals = 0;
	m_workersOnGas = 0;
	m_workersMovingToMinerals = 0;
	m_workersMovingToGas = 0;
	m_unitCompletedFlags = 0;
	m_unitUnderConstructionFlags = 0;
	for(size_t i=0; i < m_unitUnderConstruction.size(); i++)
		m_unitUnderConstruction[i] = 0;
	for(size_t i=0; i < m_units.size(); i++)
		m_units[i]->m_unitList.clear();
	RemoveAllPointer(m_allUnits);
	m_allUnits.push_back(NULL); // Index 0 is reserved
	m_researchCompletedFlags = 0;
	m_researchUnderConstructionFlags = 0;
	for(size_t i=0; i < m_researchUnderConstruction.size(); i++)
		m_researchUnderConstruction[i] = 0;
	for(size_t i=0; i < m_researchCompleted.size(); i++)
		m_researchCompleted[i] = 0;
	m_supply = 0;
	m_supplyCap = 0;
	m_supplyCapUnderConstruction = 0;

	for(size_t i=0; i < m_raceData.m_buildings.size(); i++)
	{
		const CSC2Building *building = m_raceData.m_buildings[i];
		if(building->GetGameStartCount() > 0)
		{
			CBuildingStateList &buildingStateList = *m_buildings[i];
			m_buildingUnderConstructionFlags |= ((SC2BuildingFlags)1 << i);
			m_buildingCompletedFlags |= ((SC2BuildingFlags)1 << i);
			for(size_t j=0; j < building->GetGameStartCount(); j++)
			{
				SBuildingState *buildingState = new SBuildingState();
				buildingState->productionBoost = building->GetGameStartProductionBoost();
				buildingState->buildingID = m_allBuildings.size();
				buildingState->buildingTypeID = i;
				buildingState->larvaeCount = building->GetGameStartLarvaeCount();
				m_totalLarvaeCount += building->GetGameStartLarvaeCount();
				buildingState->larvaeDelayTime = 0.0;
				if(building->GetLarvaeSpawnTime() > 0.0)
				{
					m_larvaeSpawningBuildings.push_back(buildingState);
					if(buildingState->larvaeCount < building->GetMaxLarvaeSpawningCount())
						events.add(CSC2Event(m_time + building->GetLarvaeSpawnTime(), CSC2Event::eBuildingSpawnLarvae, true, buildingState->buildingID, 0, 1));
				}
				buildingState->status = (building->GetInitialStatus() | building->GetGameStartStatus());
				for (size_t k = 0; k < building->GetGameStartStatusList().size(); k++)
				{
					if (building->GetGameStartStatusDurations()[k] > 0.0)
						events.add(CSC2Event(m_time + building->GetGameStartStatusDurations()[k], CSC2Event::eBuildingStatusLapse, true, buildingState->buildingID, 0, 1 << building->GetGameStartStatusList()[k]));
				}
				for (size_t k = 0; k < building->GetInitialStatusList().size(); k++)
				{
					if (building->GetInitialStatusDurations()[k] > 0.0)
						events.add(CSC2Event(m_time + building->GetInitialStatusDurations()[k], CSC2Event::eBuildingStatusLapse, true, buildingState->buildingID, 0, 1 << building->GetInitialStatusList()[k]));
				}
				m_allBuildings.push_back(buildingState);
				if(buildingStateList.m_building.GetEnergyRechargeRate() > 0)
					buildingState->maxEnergyTime = m_time + (building->GetMaxEnergy() - building->GetStartingEnergy()) / building->GetEnergyRechargeRate();
				buildingStateList.m_buildingList.push_back(buildingState);
			}

			if (building->IsBase())
				m_baseCount += building->GetGameStartCount();
			if(building->IsGeyserBuilding())
				m_geyserCount += building->IsGeyserBuilding();
			m_supplyCap += building->GetProvidedSupply() * building->GetGameStartCount();
		}
	}

	for(size_t i=0; i < m_raceData.m_units.size(); i++)
	{
		const CSC2Unit *unit = m_raceData.m_units[i];
		if(unit->GetGameStartCount() > 0)
		{
			CUnitStateList &unitStateList = *m_units[i];
			m_unitUnderConstructionFlags |= ((SC2UnitFlags)1 << i);
			m_unitCompletedFlags |= ((SC2UnitFlags)1 << i);
			for(size_t j=0; j < unit->GetGameStartCount(); j++)
			{
				SUnitState *unitState = new SUnitState();
				unitState->unitTypeID = i;
				unitState->unitID = m_allUnits.size();
				m_allUnits.push_back(unitState);
				m_bonusMineralIncomeRate += unitStateList.m_unit.GetMineralIncomeRate();
				if(unitStateList.m_unit.GetEnergyRechargeRate() > 0)
					unitState->maxEnergyTime = m_time + (unitStateList.m_unit.GetMaxEnergy() - unitStateList.m_unit.GetStartingEnergy()) / unitStateList.m_unit.GetEnergyRechargeRate();
				unitStateList.m_unitList.push_back(unitState);
			}

			if(unit->IsWorker())
				m_workerCount += unit->GetGameStartCount();
			m_supply += unit->GetSupplyCost() * unit->GetGameStartCount();
			m_supplyCap += unit->GetProvidedSupply() * unit->GetGameStartCount();
		}
	}

	m_minerals = 50.0;
	m_supplyCapUnderConstruction = m_supplyCap;
}

void CSC2State::StartMining()
{
	m_workersOnMinerals = m_workerCount;
	m_workersOnGas = 0;

	RecalculateMineralIncomeRate();
	RecalculateGasIncomeRate();
}

void CSC2State::ProcessEvent(CPriorityQueue<CSC2Event> &events)
{
	CSC2Event entry = events.pop();

	wxASSERT(entry.m_time == m_time);
	//ProgressTime(entry.m_time - m_time);

	switch(entry.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eSendScout:
		if(m_workersOnMinerals > 0)
		{
			m_workersOnMinerals--;
			RecalculateMineralIncomeRate();
		}
		else if(m_workersOnGas > 0)
		{
			m_workersOnGas--;
			RecalculateGasIncomeRate();
		}
		break;
	case CSC2Event::eKillScout:
		m_workerCount--;
		m_supply--;
		break;
	case CSC2Event::eReturnScout:
		m_workersOnMinerals++;
		RecalculateMineralIncomeRate();
		break;
	case CSC2Event::eWorkerStartMiningMinerals:
		m_workersOnMinerals++;
		m_workersMovingToMinerals--;
		RecalculateMineralIncomeRate();
		break;
	case CSC2Event::eWorkerStartMiningGas:
		m_workersOnGas++;
		m_workersMovingToGas--;
		RecalculateGasIncomeRate();
		break;

	case CSC2Event::eBuildingComplete:
		{
			size_t buildingTypeID = entry.m_event.m_data.m_targetID;
			CBuildingStateList &buildingStateList = *m_buildings[buildingTypeID];
			const CSC2Building &building = buildingStateList.m_building;
			SBuildingState *buildingState = new SBuildingState();
			buildingState->productionBoost = building.GetInitialProductionBoost();
			buildingState->buildingTypeID = buildingTypeID;
			buildingState->buildingID = m_allBuildings.size();
			buildingState->larvaeCount = building.GetInitialLarvaeCount();
			m_totalLarvaeCount += building.GetInitialLarvaeCount();
			buildingState->larvaeDelayTime = 0.0;
			if(building.GetLarvaeSpawnTime() > 0.0)
			{
				m_larvaeSpawningBuildings.push_back(buildingState);
				if(buildingState->larvaeCount < building.GetMaxLarvaeSpawningCount())
					events.add(CSC2Event(m_time + building.GetLarvaeSpawnTime(), CSC2Event::eBuildingSpawnLarvae, true, buildingState->buildingID, 0, 1));
			}
			buildingState->status = building.GetInitialStatus() | entry.m_event.m_data.m_data;
			for (size_t i = 0; i < building.GetInitialStatusList().size(); i++)
			{
				if (building.GetInitialStatusDurations()[i] > 0.0)
					events.add(CSC2Event(m_time + building.GetInitialStatusDurations()[i], CSC2Event::eBuildingStatusLapse, true, buildingState->buildingID, 0, 1 << building.GetInitialStatusList()[i]));
			}
			m_allBuildings.push_back(buildingState);
			if(building.GetEnergyRechargeRate() > 0.0)
				buildingState->maxEnergyTime = m_time + (building.GetMaxEnergy() - building.GetStartingEnergy()) / building.GetEnergyRechargeRate();
			buildingStateList.m_buildingList.push_back(buildingState);
			m_buildingCompletedFlags |= ((SC2BuildingFlags)1 << buildingTypeID);
			m_buildingUnderConstruction[buildingTypeID]--;
			if(building.IsGeyserBuilding())
			{
				m_geysersUnderConstruction--;
				m_geyserCount++;
			}
			m_supplyCap += building.GetProvidedSupply();
		}
		break;

	case CSC2Event::eBuildingMorph:
		{
			SBuildingState *buildingState = m_allBuildings[entry.m_event.m_data.m_sourceID];
			size_t oldBuildingTypeID = buildingState->buildingTypeID;
			CBuildingStateList *oldBuildingStateList = m_buildings[oldBuildingTypeID];
			for(size_t i=0; i < oldBuildingStateList->size(); i++)
			{
				if(oldBuildingStateList->GetAt(i) == buildingState)
				{
					oldBuildingStateList->erase(i);
					break;
				}
			}
			const CSC2Building &oldBuilding = *m_raceData.m_buildings[oldBuildingTypeID];
			buildingState->buildingTypeID = entry.m_event.m_data.m_targetID;
			const CSC2Building &newBuilding = *m_raceData.m_buildings[buildingState->buildingTypeID];
			m_buildings[buildingState->buildingTypeID]->m_buildingList.push_back(buildingState);
			if(0.0 < newBuilding.GetEnergyRechargeRate() && 0.0 == oldBuilding.GetEnergyRechargeRate())
				buildingState->maxEnergyTime = m_time + (newBuilding.GetMaxEnergy() - newBuilding.GetStartingEnergy()) / newBuilding.GetEnergyRechargeRate();
			if(newBuilding.GetLarvaeSpawnTime() > 0.0 && oldBuilding.GetLarvaeSpawnTime() == 0.0)
			{
				buildingState->larvaeCount = newBuilding.GetInitialLarvaeCount();
				if(buildingState->larvaeCount < newBuilding.GetMaxLarvaeSpawningCount())
					events.add(CSC2Event(m_time + newBuilding.GetLarvaeSpawnTime(), CSC2Event::eBuildingSpawnLarvae, true, buildingState->buildingID, 0, 1));
			}
			else if(newBuilding.GetLarvaeSpawnTime() == 0.0 && oldBuilding.GetLarvaeSpawnTime() > 0.0)
			{
				buildingState->larvaeCount = 0;
				bool removed = true;
				while(removed)
				{
					removed = false;
					for(size_t i=0; i < events.size(); i++)
					{
						const CSC2Event::SEvent::SData &eventData = events[i].m_event.m_data;
						if((eventData.m_eventCategory == CSC2Event::eBuildingSpawnLarvae || eventData.m_eventCategory == CSC2Event::eBuildingSpawnBonusLarvae)
							&& eventData.m_sourceID == buildingState->buildingID)
						{
							events.pop(i);
							removed = true;
						}
					}
				}
			}
			m_buildingCompletedFlags |= ((SC2BuildingFlags)1 << buildingState->buildingTypeID);
			m_buildingUnderConstruction[buildingState->buildingTypeID]--;
			m_buildingMorphing[oldBuildingTypeID]--;
			if(newBuilding.IsGeyserBuilding() && !oldBuilding.IsGeyserBuilding())
			{
				m_geysersUnderConstruction--;
				m_geyserCount++;
			}
			else if(newBuilding.IsGeyserBuilding() && !oldBuilding.IsGeyserBuilding())
			{
				m_geyserCount--;
			}
			m_supplyCap += newBuilding.GetProvidedSupply() - oldBuilding.GetProvidedSupply();
		}
		break;

	case CSC2Event::eBuildingConsume:
		{
			SBuildingState *buildingState = m_allBuildings[entry.m_event.m_data.m_sourceID];
			CBuildingStateList &buildingStateList = *m_buildings[buildingState->buildingTypeID];
			for(size_t i=0; i < buildingStateList.size(); i++)
			{
				if(buildingStateList[i] == buildingState)
				{
					buildingStateList.m_buildingList.erase(i);
					break;
				}
			}
			// m_buildingsCompletedFlags...  Hmm...
			if(buildingStateList.m_building.IsBase())
				m_baseCount--;
			if(buildingStateList.m_building.IsGeyserBuilding())
				m_geyserCount--;
			m_supplyCap -= buildingStateList.m_building.GetProvidedSupply();
		}
		break;

	case CSC2Event::eBuildingStatusApply:
		{
			SBuildingState *buildingState = m_allBuildings[entry.m_event.m_data.m_sourceID];
			buildingState->status |= (unsigned short)entry.m_event.m_data.m_data;
			double productionBoostFactor = 1.0;
			unsigned short statusFlags = entry.m_event.m_data.m_data;
			for (size_t i = 0; statusFlags != 0; i++, statusFlags >>= 1)
			{
				if (statusFlags & 0x1)
					productionBoostFactor *= m_raceData.m_buildingStatuses[i]->GetProductionBoostFactor();
			}
			buildingState->productionBoost *= productionBoostFactor;
			if (1.0 != productionBoostFactor)
			{
				CVector<CSC2Event> eventsModified;
				bool eventModified = true;
				while(eventModified)
				{
					eventModified = false;
					for(size_t eventIndex=0; eventIndex < events.size(); eventIndex++)
					{
						const CSC2Event::SEvent::SData &eventData = events.getQueue()[eventIndex].m_event.m_data;
						if(eventData.m_sourceIsBuilding && eventData.m_sourceID == buildingState->buildingID)
						{
							eventModified = true;
							CSC2Event event = events.pop(eventIndex);
							event.m_time = m_time + (event.m_time - m_time) / productionBoostFactor;
							eventsModified.push_back(event);
							break;
						}
					}
				}
				for(size_t i=0; i < eventsModified.size(); i++)
					events.add(eventsModified[i]);
			}
		}
		break;

	case CSC2Event::eBuildingStatusLapse:
		{
			SBuildingState *buildingState = m_allBuildings[entry.m_event.m_data.m_sourceID];
			buildingState->status &= (~(unsigned short)entry.m_event.m_data.m_data);
			double productionBoostFactor = 1.0;
			unsigned short statusFlags = entry.m_event.m_data.m_data;
			for (size_t i = 0; statusFlags != 0; i++, statusFlags >>= 1)
			{
				if (statusFlags & 0x1)
					productionBoostFactor *= m_raceData.m_buildingStatuses[i]->GetProductionBoostFactor();
			}
			buildingState->productionBoost /= productionBoostFactor;
			if (1.0 != productionBoostFactor)
			{
				CVector<CSC2Event> eventsModified;
				bool eventModified = true;
				while(eventModified)
				{
					eventModified = false;
					for(size_t eventIndex=0; eventIndex < events.size(); eventIndex++)
					{
						const CSC2Event::SEvent::SData &eventData = events.getQueue()[eventIndex].m_event.m_data;
						if(eventData.m_sourceIsBuilding && eventData.m_sourceID == buildingState->buildingID)
						{
							eventModified = true;
							CSC2Event event = events.pop(eventIndex);
							event.m_time = m_time + (event.m_time - m_time) * productionBoostFactor;
							eventsModified.push_back(event);
							break;
						}
					}
				}
				for(size_t i=0; i < eventsModified.size(); i++)
					events.add(eventsModified[i]);
			}
		}
		break;

	case CSC2Event::eUnitComplete:
		{
			size_t buildUnitCount = entry.m_event.m_data.m_data;
			if(0 == buildUnitCount)
				buildUnitCount = 1;
			for(size_t i=0; i < buildUnitCount; i++)
			{
				size_t unitTypeID = entry.m_event.m_data.m_targetID;
				CUnitStateList &unitStateList = *m_units[unitTypeID];
				const CSC2Unit &unit = unitStateList.m_unit;
				SUnitState *unitState = new SUnitState();
				unitState->unitTypeID = unitTypeID;
				unitState->unitID = m_allUnits.size();
				m_allUnits.push_back(unitState);
				if(unitStateList.m_unit.GetEnergyRechargeRate() > 0)
					unitState->maxEnergyTime = m_time + (unit.GetMaxEnergy() - unit.GetStartingEnergy()) / unit.GetEnergyRechargeRate();
				unitStateList.m_unitList.push_back(unitState);
				if(0.0 < unit.GetMineralIncomeRate())
				{
					m_bonusMineralIncomeRate += unit.GetMineralIncomeRate();
					RecalculateMineralIncomeRate();
				}

				if(unit.GetExpirationTime())
					events.add(CSC2Event(m_time + unit.GetExpirationTime(), CSC2Event::eUnitDies, false, unitState->unitID));

				m_unitCompletedFlags |= ((SC2UnitFlags)1 << unitTypeID);
				m_unitUnderConstruction[unitTypeID]--;
				if(unit.IsWorker())
				{
					m_workerCount++;
					m_workersMovingToMinerals++;
					events.add(CSC2Event(m_time + 2.0, CSC2Event::eWorkerStartMiningMinerals));
				}
				m_supplyCap += buildUnitCount * unit.GetProvidedSupply();
			}
		}
		break;

	case CSC2Event::eUnitMorph:
		{
			SUnitState *unitState = m_allUnits[entry.m_event.m_data.m_sourceID];
			size_t oldUnitTypeID = unitState->unitTypeID;
			const CSC2Unit &oldUnit = *m_raceData.m_units[oldUnitTypeID];
			unitState->unitTypeID = entry.m_event.m_data.m_targetID;
			const CSC2Unit &newUnit = *m_raceData.m_units[unitState->unitTypeID];
			m_units[unitState->unitTypeID]->m_unitList.push_back(unitState);
			if(0.0 < newUnit.GetEnergyRechargeRate() && 0.0 == oldUnit.GetEnergyRechargeRate())
				unitState->maxEnergyTime = m_time + (newUnit.GetMaxEnergy() - newUnit.GetStartingEnergy()) / newUnit.GetEnergyRechargeRate();
			m_unitCompletedFlags |= ((SC2UnitFlags)1 << unitState->unitTypeID);
			m_unitUnderConstruction[unitState->unitTypeID]--;
			if(newUnit.IsWorker())
			{
				m_workerCount++;
				m_workersMovingToMinerals++;
				events.add(CSC2Event(m_time + 2.0, CSC2Event::eWorkerStartMiningMinerals));
			}
			if(0.0 < newUnit.GetMineralIncomeRate())
			{
				m_bonusMineralIncomeRate += newUnit.GetMineralIncomeRate();
				RecalculateMineralIncomeRate();
			}
			m_supplyCap += newUnit.GetProvidedSupply() - oldUnit.GetProvidedSupply();
		}
		break;

	case CSC2Event::eUnitDies:
		{
			SUnitState *unitState = m_allUnits[entry.m_event.m_data.m_sourceID];
			size_t oldUnitTypeID = unitState->unitTypeID;
			CSC2State::CUnitStateList &oldUnitList = *m_units[oldUnitTypeID];
			for(size_t i=0; i < oldUnitList.size(); i++)
			{
				if(oldUnitList[i] == unitState)
					oldUnitList.m_unitList.erase(i);
			}
			const CSC2Unit &oldUnit = *m_raceData.m_units[oldUnitTypeID];
			if(oldUnit.IsWorker())
			{
				m_workerCount--;
				if(m_workersOnMinerals > 0)
				{
					m_workersOnMinerals--;
					RecalculateMineralIncomeRate();
				}
				else if(m_workersOnGas > 0)
				{
					m_workersOnGas--;
					RecalculateGasIncomeRate();
				}
			}
			if(0.0 < oldUnit.GetMineralIncomeRate())
			{
				m_bonusMineralIncomeRate -= oldUnit.GetMineralIncomeRate();
				RecalculateMineralIncomeRate();
			}
			m_supply -= oldUnit.GetSupplyCost();
			m_supplyCap -= oldUnit.GetProvidedSupply();

			m_allUnits[entry.m_event.m_data.m_sourceID] = NULL;
			delete unitState;
		}
		break;

	case CSC2Event::eResearchComplete:
		{
			size_t researchID = entry.m_event.m_data.m_targetID;
			m_researchCompleted[researchID] = true;
			m_researchCompletedFlags |= ((SC2ResearchFlags)1 << researchID);
			m_researchUnderConstruction[researchID] = false;
		}
		break;

	case CSC2Event::eBuildingSpawnLarvae:
		{
			SBuildingState *buildingState = m_allBuildings[entry.m_event.m_data.m_sourceID];
			const CSC2Building &building = *m_raceData.m_buildings[buildingState->buildingTypeID];
			buildingState->larvaeCount++;
			m_totalLarvaeCount++;
			if(buildingState->larvaeCount < building.GetMaxLarvaeSpawningCount())
				events.add(CSC2Event(m_time + building.GetLarvaeSpawnTime(), CSC2Event::eBuildingSpawnLarvae, true, buildingState->buildingID, 0, 1));
		}
		break;

	case CSC2Event::eBuildingSpawnBonusLarvae:
		{
			SBuildingState *buildingState = m_allBuildings[entry.m_event.m_data.m_sourceID];
			const CSC2Building &building = *m_raceData.m_buildings[buildingState->buildingTypeID];
			if(buildingState->larvaeCount < building.GetMaxLarvaeSpawningCount() && buildingState->larvaeCount + entry.m_event.m_data.m_data >= building.GetMaxLarvaeSpawningCount())
			{
				for(size_t i=0; i < events.size(); i++)
				{
					const CSC2Event::SEvent::SData &eventData = events[i].m_event.m_data;
					if(eventData.m_eventCategory == CSC2Event::eBuildingSpawnLarvae && eventData.m_sourceID == buildingState->buildingID)
					{
						events.pop(i);
						break;
					}
				}
			}

			buildingState->larvaeCount += entry.m_event.m_data.m_data;
			m_totalLarvaeCount += entry.m_event.m_data.m_data;
		}
		break;

	case CSC2Event::eSpawnBase:
		{
			m_baseCount++;
			m_basesUnderConstruction--;
			RecalculateMineralIncomeRate();
			RecalculateGasIncomeRate();
		}
		break;
	}
}

void CSC2State::RecalculateMineralIncomeRate()
{
	m_mineralIncomeRate = CGameCalcs::GetMineralIncomeRate(m_baseCount, m_workersOnMinerals) + m_bonusMineralIncomeRate;
}

void CSC2State::RecalculateGasIncomeRate()
{
	m_gasIncomeRate = CGameCalcs::GetGasIncomeRate2Near(m_baseCount, m_geyserCount, m_workersOnGas);
}

void CSC2State::ProgressTime(double duration)
{
	double mineralsGained = m_mineralIncomeRate * duration;
	m_minerals += mineralsGained;
	m_mineralsMined += mineralsGained;
	double gasGained = m_gasIncomeRate * duration;
	m_gas += gasGained;
	m_gasMined += gasGained;
	m_time += duration;
}

double CSC2State::CBuildingStateList::CalculateEnergy(size_t index, double time) const
{
	double maxEnergyTime = m_buildingList[index]->maxEnergyTime;
	if(maxEnergyTime <= time)
		return m_building.GetMaxEnergy();
	else
		return m_building.GetMaxEnergy() - (maxEnergyTime - time) * m_building.GetEnergyRechargeRate();
}

void CSC2State::CBuildingStateList::UseEnergy(size_t index, double energy, double time)
{
	if(m_buildingList[index]->maxEnergyTime <= time)
		m_buildingList[index]->maxEnergyTime = time + energy / m_building.GetEnergyRechargeRate();
	else
		m_buildingList[index]->maxEnergyTime += energy / m_building.GetEnergyRechargeRate();
}

double CSC2State::CUnitStateList::CalculateEnergy(size_t index, double time) const
{
	double maxEnergyTime = m_unitList[index]->maxEnergyTime;
	if(maxEnergyTime <= time)
		return m_unit.GetMaxEnergy();
	else
		return m_unit.GetMaxEnergy() - (maxEnergyTime - time) * m_unit.GetEnergyRechargeRate();
}

void CSC2State::CUnitStateList::UseEnergy(size_t index, double energy, double time)
{
	if(m_unitList[index]->maxEnergyTime <= time)
		m_unitList[index]->maxEnergyTime = time + energy / m_unit.GetEnergyRechargeRate();
	else
		m_unitList[index]->maxEnergyTime += energy / m_unit.GetEnergyRechargeRate();
}

void CSC2State::PrintSummary(wxString &output) const
{
	if(m_raceData.m_hasLarvae)
		output += wxString::Format("%4.0fM %4.0fG %3dL %3d/%3dS", m_minerals, m_gas, m_totalLarvaeCount, m_supply, m_supplyCap);
	else
		output += wxString::Format("%4.0fM %4.0fG %3d/%3dS", m_minerals, m_gas, m_supply, m_supplyCap);
}

void CSC2State::PrintDetails(wxString &output) const
{
	PrintSummary(output);

	output += wxString::Format(L"\nIncome:   %4.0fM %4.0fG", 60.0 * m_mineralIncomeRate, 60.0 * m_gasIncomeRate);

	output.Append(L"\nBuildings: ");

	for(size_t i=0; i < m_buildings.size(); i++)
	{
		const CBuildingStateList *buildingStateList = m_buildings[i];

		if(buildingStateList->size() > 0)
			output += wxString::Format(L" %u %s", buildingStateList->size(), buildingStateList->m_building.GetName());
	}

	output.Append(L"\nUnits:     ");

	for(size_t i=0; i < m_units.size(); i++)
	{
		const CUnitStateList *unitStateList = m_units[i];

		if(unitStateList->size() > 0)
			output += wxString::Format(L" %u %s", unitStateList->size(), unitStateList->m_unit.GetName());
	}

	output.Append(L"\nResearch:  ");

	for(size_t i=0; i < m_researchCompleted.size(); i++)
	{
		if(m_researchCompleted[i])
			output += wxString::Format(L" %s", m_raceData.m_research[i]->GetName());
	}

	output.Append(L"\n");
}
