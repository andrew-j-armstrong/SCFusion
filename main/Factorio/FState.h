#pragma once

#include <stdio.h>
#include "../Core/PriorityQueue.h"

#include "FEvent.h"
#include "FBuilding.h"
#include "FDefines.h"
#include "GridItem.h"

class CFState
{
public:
	CFState();
	CFState(const CFState& state);
	~CFState();

	void SetInitialState(CPriorityQueue<CFEvent>& events);

	void operator=(const CFState& state);

	void RecalculateMineralIncomeRate();
	void RecalculateGasIncomeRate();

	void ProgressTime(double duration);
	void ProcessEvent(CPriorityQueue<CFEvent>& events);

	void PrintSummary(wxString& output) const;
	void PrintDetails(wxString& output) const;
	void FillData(GridItem& item) const;
	void FillMilestoneData(GridItem& item) const;

	// Status
	double m_time;

	// Storage
	CVector<size_t> m_playerInventory;
	CVector<size_t> m_microInventory; // Storage in micromanaged buildings

	CVector<double> m_productionRates;
	CVector<double> m_consumptionRates;
	CVector<double> m_produced;
	
	// Buildings
	struct SBuildingState : public CMemPoolNode<SBuildingState>
	{
		size_t buildingTypeID;
		size_t recipeID;
		size_t buildingID;
		bool micromanaged;
		double maxFuelTime; // time until fuel runs out, in seconds
		double output;
		double outputRate;
		FBuildingStatusFlags status;
	};
	class CBuildingStateList : public CMemPoolNode<CBuildingStateList>
	{
	public:
		CBuildingStateList(const CFBuilding& building)
			: m_building(building)
			, m_buildingList()
		{
		}

		~CBuildingStateList()
		{
		}

		size_t size() const { return m_buildingList.size(); }
		void erase(size_t index, size_t count = 1) { m_buildingList.erase(index, count); }
		SBuildingState* GetAt(size_t index) { return m_buildingList[index]; }
		const SBuildingState* GetAt(size_t index) const { return m_buildingList[index]; }

		SBuildingState* operator[](size_t index) { return m_buildingList[index]; }
		const SBuildingState* operator[](size_t index) const { return m_buildingList[index]; }

		double CalculateEnergy(size_t index, double time) const;
		void UseEnergy(size_t index, double energy, double time);

		const CFBuilding& m_building;
		CVector<SBuildingState*> m_buildingList;
	};
	CVector<CBuildingStateList*> m_buildings;
	CVector<CBuildingStateList*> m_microBuildings;

	
	// Research
	//SC2ResearchFlags m_researchCompletedFlags;
	//SC2ResearchFlags m_researchUnderConstructionFlags;
	//CVector<bool> m_researchCompleted;
	//CVector<bool> m_researchUnderConstruction;
};
