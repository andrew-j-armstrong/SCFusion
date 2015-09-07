#pragma once

#include <stdio.h>
#include "../Core/PriorityQueue.h"

#include "SC2Event.h"
#include "SC2Race.h"
#include "SC2RaceData.h"
#include "SC2Defines.h"
#include "SC2Building.h"
#include "SC2Unit.h"
#include "SC2Research.h"
#include "GasMicro.h"

class CSC2State
{
public:
	CSC2State(ESC2Race race, const CSC2RaceData &raceData);
	CSC2State(const CSC2State &state);
	~CSC2State();

	void SetInitialState(CPriorityQueue<CSC2Event> &events);
	void StartMining();

	void operator=(const CSC2State &state);

	void RecalculateMineralIncomeRate();
	void RecalculateGasIncomeRate();

	void ProgressTime(double duration);
	void ProcessEvent(CPriorityQueue<CSC2Event> &events);

	void PrintSummary(wxString &output) const;
	void PrintDetails(wxString &output) const;

	const ESC2Race m_race;
	const CSC2RaceData &m_raceData;

	// Status
	double m_time;

	// Resources
	double m_minerals;
	double m_gas;
	double m_mineralsMined;
	double m_gasMined;

	// Income Rates
	double m_bonusMineralIncomeRate; // MULEs
	double m_mineralIncomeRate;
	double m_gasIncomeRate;

	// Buildings
	size_t m_baseCount;
	size_t m_geyserCount;
	size_t m_basesUnderConstruction;
	size_t m_geysersUnderConstruction;
	SC2BuildingFlags m_buildingCompletedFlags;
	SC2BuildingFlags m_buildingUnderConstructionFlags;
	CVector<size_t> m_buildingUnderConstruction;
	CVector<size_t> m_buildingMorphing;
	struct SBuildingState : public CMemPoolNode<SBuildingState>
	{
		size_t buildingTypeID;
		size_t buildingID;
		size_t larvaeCount;
		double maxEnergyTime;
		double larvaeDelayTime;
		double productionBoost;
		SC2BuildingStatusFlags status;
	};
	class CBuildingStateList : public CMemPoolNode<CBuildingStateList>
	{
	public:
		CBuildingStateList(const CSC2Building &building)
			: m_building(building)
			, m_buildingList()
		{
		}

		~CBuildingStateList()
		{
		}

		size_t size() const { return m_buildingList.size(); }
		void erase(size_t index, size_t count = 1) { m_buildingList.erase(index, count); }
		SBuildingState *GetAt(size_t index) { return m_buildingList[index]; }
		const SBuildingState *GetAt(size_t index) const { return m_buildingList[index]; }

		SBuildingState *operator[](size_t index) { return m_buildingList[index]; }
		const SBuildingState *operator[](size_t index) const { return m_buildingList[index]; }

		double CalculateEnergy(size_t index, double time) const;
		void UseEnergy(size_t index, double energy, double time);

		const CSC2Building &m_building;
		CVector<SBuildingState *> m_buildingList;
	};
	CVector<CBuildingStateList *> m_buildings;
	CVector<SBuildingState *> m_allBuildings;
	CVector<SBuildingState *> m_larvaeSpawningBuildings;

	// Units
	size_t m_totalLarvaeCount;
	size_t m_workerCount;
	size_t m_workersOnMinerals;
	size_t m_workersOnGas;
	size_t m_workersMovingToMinerals;
	size_t m_workersMovingToGas;
	SC2UnitFlags m_unitCompletedFlags;
	SC2UnitFlags m_unitUnderConstructionFlags;
	CVector<size_t> m_unitUnderConstruction;
	struct SUnitState : public CMemPoolNode<SUnitState>
	{
		size_t unitTypeID;
		size_t unitID;
		double maxEnergyTime;
		SC2UnitStatusFlags status;
	};
	class CUnitStateList : public CMemPoolNode<CUnitStateList>
	{
	public:
		CUnitStateList(const CSC2Unit &unit)
			: m_unit(unit)
			, m_unitList()
		{
		}

		~CUnitStateList()
		{
		}

		size_t size() const { return m_unitList.size(); }
		void erase(size_t index, size_t count = 1) { m_unitList.erase(index, count); }
		SUnitState *operator[](size_t index) { return m_unitList[index]; }
		const SUnitState *operator[](size_t index) const { return m_unitList[index]; }

		double CalculateEnergy(size_t index, double time) const;
		void UseEnergy(size_t index, double energy, double time);

		const CSC2Unit &m_unit;
		CVector<SUnitState *> m_unitList;
	};
	CVector<CUnitStateList *> m_units;
	CVector<SUnitState *> m_allUnits;

	// Research
	SC2ResearchFlags m_researchCompletedFlags;
	SC2ResearchFlags m_researchUnderConstructionFlags;
	CVector<bool> m_researchCompleted;
	CVector<bool> m_researchUnderConstruction;

	// Supply
	size_t m_supply;
	size_t m_supplyCap;
	size_t m_supplyCapUnderConstruction;
};
