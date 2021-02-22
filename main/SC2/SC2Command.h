#pragma once

#include "../Core/PriorityQueue.h"

#include "../GA/GASequenceChromosome.h"

#include "SC2Event.h"
#include "SC2Race.h"
#include "SC2Defines.h"
#include "SC2State.h"
#include "SC2Building.h"
#include "SC2Unit.h"
#include "SC2Research.h"
#include "SC2RaceData.h"

class CSC2Command
{
public:
	CSC2Command(const CSC2RaceData &raceData);
	virtual ~CSC2Command() {}

	virtual bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const = 0;
	virtual double GetMineralCost() const = 0;
	virtual double GetGasCost() const = 0;
	virtual bool WillSpawnBase() const = 0;
	virtual bool WillBuildGeyserBuilding() const = 0;
	virtual bool RequiresGeyser() const = 0;
	virtual bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const = 0;
	virtual bool IsBuildWorkerCommand() const = 0;
	virtual size_t GetProvidedSupply() const = 0;
	virtual size_t GetRequiredSupply() const = 0;
	virtual SC2BuildingFlags GetBuildingRequirementFlags() const = 0;
	virtual SC2UnitFlags GetUnitRequirementFlags() const = 0;
	virtual SC2ResearchFlags GetResearchRequirementFlags() const = 0;

	virtual bool HasSource() const = 0;
	virtual ESC2SourceType GetSourceType() const = 0;
	virtual size_t GetSourceID(const CSC2State &state) const = 0;
	virtual bool WillConsumeSource() const = 0;
	virtual double GetMorphSourceDuration() const = 0;

	virtual bool WillBuildBuilding() const = 0;
	virtual bool WillBuildUnit() const = 0;
	virtual size_t GetBuildBuildingTypeID() const = 0;
	virtual size_t GetBuildUnitTypeID() const = 0;

	virtual bool HasPrerequisits(const CSC2State &state) const = 0;
	virtual bool HasRequirements(const CSC2State &state) const = 0;
	virtual double GetResourceWaitTime(const CSC2State &state) const = 0;
	virtual bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const = 0;
	virtual double GetPostCommandDuration() const { return 0.0; }

	const wxString &GetName() const { return m_name; }
	bool IsAvailableToGeneticAlgorithm() const { return m_isAvailableToGeneticAlgorithm; }
	bool IsAutoCastAbility() const { return m_isAutoCastAbility; }

	const CSC2Command *GetPrerequisitCommand(const CSC2State &state
		, EGasMicro gasMicro
		, const CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands
		, const CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands
		, const CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands
		, const CVector<const CSC2Command *> &buildGeyserBuildingCommands
		, const CVector<const CSC2Command *> &moveWorkersToMineralsCommands
		, const CVector<const CSC2Command *> &moveWorkersToGasCommands
		, const CVector<const CSC2Command *> &provideSupplyCommands) const;

protected:
	const CSC2RaceData &m_raceData;
	wxString m_name;
	bool m_isAvailableToGeneticAlgorithm;
	bool m_isAutoCastAbility;
};

class CSC2WaitCommand : public CSC2Command
{
public:
	CSC2WaitCommand(const CSC2RaceData &raceData, double duration);
	~CSC2WaitCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const { return false; }
	double GetMineralCost() const { return 0.0; }
	double GetGasCost() const { return 0.0; }
	bool WillSpawnBase() const { return false; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const { return false; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const { return 0; }
	size_t GetRequiredSupply() const { return 0; }
	bool RequiresGeyser() const { return false; }
	SC2BuildingFlags GetBuildingRequirementFlags() const { return (SC2BuildingFlags)0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return (SC2ResearchFlags)0; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const{ wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool HasPrerequisits(const CSC2State &state) const { return true; }
	bool HasRequirements(const CSC2State &state) const { return true; }
	double GetResourceWaitTime(const CSC2State &state) const { return 0.0; }
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const { return true; }
	double GetPostCommandDuration() const { return m_duration; }

protected:
	double m_duration;
};

class CSC2XMLCommand : public CSC2Command
{
public:
	CSC2XMLCommand(const CSC2RaceData &raceData);
	~CSC2XMLCommand() {}

	virtual bool LoadXML(const wxXmlNode *xmlCommand) = 0;
	virtual bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands) = 0;

protected:

};

class CSC2MoveWorkersCommand : public CSC2XMLCommand
{
public:
	CSC2MoveWorkersCommand(const CSC2RaceData &raceData);
	CSC2MoveWorkersCommand(const CSC2RaceData &raceData, bool moveToGas, size_t moveWorkerCount);
	~CSC2MoveWorkersCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return 0.0; }
	double GetGasCost() const { return 0.0; }
	bool WillSpawnBase() const { return false; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool RequiresGeyser() const { return true; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const { return false; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const { return 0; }
	size_t GetRequiredSupply() const { return 0; }
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return (SC2ResearchFlags)0; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const{ wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

	bool GetMoveToGas() const { return m_moveToGas; }
	size_t GetMoveWorkerCount() const { return m_moveWorkerCount; }

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;

	bool m_moveToGas;
	size_t m_moveWorkerCount;
};

class CSC2BuildBuildingCommand : public CSC2XMLCommand
{
public:
	CSC2BuildBuildingCommand(const CSC2RaceData &raceData);
	~CSC2BuildBuildingCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_mineralCost; }
	double GetGasCost() const { return m_gasCost; }
	bool WillSpawnBase() const { return m_spawnBase; }
	bool WillBuildGeyserBuilding() const { return m_buildBuilding->IsGeyserBuilding(); }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const { return m_spawnBase || m_buildBuilding->GetProvidedSupply() > 0 || m_buildBuilding->IsBase() || m_buildBuilding->GetMaxLarvaeCount() > 0; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const { return m_buildBuilding->GetProvidedSupply(); }
	size_t GetRequiredSupply() const { return 0; }
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return (SC2ResearchFlags)0; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const{ wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return true; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { return m_buildBuildingTypeID; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;

	wxString m_targetBuildingName;
	size_t m_targetBuildingTypeID;
	const CSC2Building *m_targetBuilding;

	double m_mineralCost;
	double m_gasCost;

	bool m_occupyWorker;
	double m_occupyWorkerTime;

	bool m_consumeWorker;
	size_t m_consumeWorkerUnitID;
	bool m_consumeTargetBuilding;
	double m_workerTravelTime;
	double m_buildBuildingCompletionTime;
	wxString m_buildBuildingName;
	size_t m_buildBuildingTypeID;
	const CSC2Building *m_buildBuilding;

	std::vector<wxString> m_buildBuildingInitialStatusNames;
	SC2BuildingStatusFlags m_buildBuildingInitialStatus;

	double m_spawnBaseTime;
	bool m_spawnBase;
};

class CSC2BuildingAbilityCommand : public CSC2XMLCommand
{
public:
	CSC2BuildingAbilityCommand(const CSC2RaceData &raceData);
	~CSC2BuildingAbilityCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_mineralCost; }
	double GetGasCost() const { return m_gasCost; }
	bool WillBuildGeyserBuilding() const { return NULL != m_buildBuilding && (m_buildBuilding->IsGeyserBuilding() || m_buildBuilding->GetMaxLarvaeCount() > 0); }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const;
	bool WillSpawnBase() const { return m_spawnBase; }
	bool IsBuildWorkerCommand() const { return m_buildUnit && m_buildUnit->IsWorker(); }
	size_t GetProvidedSupply() const;
	size_t GetRequiredSupply() const;
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return m_researchRequirements; }

	bool HasSource() const { return true; }
	ESC2SourceType GetSourceType() const{ return eSourceTypeBuilding; }
	size_t GetSourceID(const CSC2State &state) const;
	bool WillConsumeSource() const { return false; }
	double GetMorphSourceDuration() const { return 0.0; }

	bool WillBuildBuilding() const { return m_buildBuilding ? true : false; }
	bool WillBuildUnit() const { return m_buildUnit ? true : false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(m_buildBuilding); return m_buildBuildingTypeID; }
	size_t GetBuildUnitTypeID() const { wxASSERT(m_buildUnit); return m_buildUnitTypeID; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;
	std::vector<wxString> m_researchRequirementNames;
	SC2ResearchFlags m_researchRequirements;

	wxString m_sourceBuildingName;
	size_t m_sourceBuildingTypeID;
	const CSC2Building *m_sourceBuilding;
	std::vector<wxString> m_requireSourceBuildingStatusAbsentNames;
	SC2BuildingStatusFlags m_requireSourceBuildingStatusAbsent;
	std::vector<wxString> m_requireSourceBuildingStatusNames;
	SC2BuildingStatusFlags m_requireSourceBuildingStatus;

	wxString m_targetBuildingName;
	size_t m_targetBuildingTypeID;
	const CSC2Building *m_targetBuilding;
	bool m_canTargetSelf;
	std::vector<wxString> m_requireTargetBuildingStatusAbsentNames;
	SC2BuildingStatusFlags m_requireTargetBuildingStatusAbsent;

	double m_mineralCost;
	double m_gasCost;
	double m_energyCost;

	std::vector<wxString> m_removeSourceBuildingStatusNames;
	CVector<size_t> m_removeSourceBuildingStatusIDs;
	SC2BuildingStatusFlags m_removeSourceBuildingStatus;
	std::vector<wxString> m_applySourceBuildingStatusNames;
	CVector<size_t> m_applySourceBuildingStatusIDs;
	CVector<SC2BuildingStatusFlags> m_applySourceBuildingStatus;
	CVector<double> m_applySourceBuildingStatusDelay;
	CVector<double> m_applySourceBuildingStatusDuration;
	double m_sourceProductionBoost;

	std::vector<wxString> m_applyTargetBuildingStatusNames;
	size_t m_applyTargetBuildingStatusID;
	SC2BuildingStatusFlags m_applyTargetBuildingStatus;
	double m_applyTargetBuildingStatusDuration;
	double m_targetProductionBoost;

	wxString m_morphSourceBuildingName;
	size_t m_morphSourceBuildingTypeID;
	const CSC2Building *m_morphSourceBuilding;
	double m_morphSourceBuildingTime;

	bool m_consumeTargetBuilding;
	wxString m_morphTargetBuildingName;
	size_t m_morphTargetBuildingTypeID;
	const CSC2Building *m_morphTargetBuilding;
	double m_morphTargetBuildingTime;

	wxString m_buildBuildingName;
	size_t m_buildBuildingTypeID;
	const CSC2Building *m_buildBuilding;
	double m_buildBuildingTime;

	wxString m_buildUnitName;
	size_t m_buildUnitTypeID;
	const CSC2Unit *m_buildUnit;
	double m_buildUnitTime;

	double m_spawnBaseTime;
	bool m_spawnBase;
};

class CSC2CancelBuildingCommand : public CSC2XMLCommand
{
public:
	CSC2CancelBuildingCommand(const CSC2RaceData &raceData);
	~CSC2CancelBuildingCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return 0.0; }
	double GetGasCost() const { return 0.0; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const { return true; }
	bool WillSpawnBase() const { return false; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const { return 0; }
	size_t GetRequiredSupply() const { return 0; }
	SC2BuildingFlags GetBuildingRequirementFlags() const { return (SC2BuildingFlags)0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return (SC2ResearchFlags)0; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const{ wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const { return 0.0; }
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	wxString m_cancelBuildingName;
	size_t m_cancelBuildingTypeID;
	const CSC2Building *m_cancelBuilding;

	double m_refundMinerals;
	double m_refundGas;

	wxString m_producedWorkerName;
	size_t m_producedWorkerTypeID;
	const CSC2Unit *m_producedWorker;
	double m_producedWorkerTravelTime;
};

class CSC2BuildUnitCommand : public CSC2XMLCommand
{
public:
	CSC2BuildUnitCommand(const CSC2RaceData &raceData);
	~CSC2BuildUnitCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_mineralCost; }
	double GetGasCost() const { return m_gasCost; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const { return m_buildUnit->GetProvidedSupply() > 0 || m_buildUnit->IsWorker(); }
	bool WillSpawnBase() const { return false; }
	bool IsBuildWorkerCommand() const { return m_buildUnit && m_buildUnit->IsWorker(); }
	size_t GetProvidedSupply() const { return m_buildUnit->GetProvidedSupply(); }
	size_t GetRequiredSupply() const { return m_buildUnit->GetSupplyCost(); }
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return m_researchRequirements; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const{ wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;
	std::vector<wxString> m_researchRequirementNames;
	SC2ResearchFlags m_researchRequirements;

	wxString m_sourceBuildingName;
	size_t m_sourceBuildingTypeID;
	const CSC2Building *m_sourceBuilding;
	std::vector<wxString> m_requireSourceBuildingStatusAbsentNames;
	SC2BuildingStatusFlags m_requireSourceBuildingStatusAbsent;
	
	double m_mineralCost;
	double m_gasCost;

	std::vector<wxString> m_applySourceBuildingStatusNames;
	size_t m_applySourceBuildingStatusID;
	SC2BuildingStatusFlags m_applySourceBuildingStatus;
	double m_applySourceBuildingStatusDuration;

	bool m_consumesLarva;
	wxString m_buildUnitName;
	size_t m_buildUnitTypeID;
	const CSC2Unit *m_buildUnit;
	size_t m_buildUnitCount;
	double m_buildUnitCompletionTime;
	bool m_unitOccupiesBuilding;
};

class CSC2UnitAbilityCommand : public CSC2XMLCommand
{
public:
	CSC2UnitAbilityCommand(const CSC2RaceData &raceData);
	~CSC2UnitAbilityCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_mineralCost; }
	double GetGasCost() const { return m_gasCost; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const;
	bool WillSpawnBase() const { return false; }
	bool IsBuildWorkerCommand() const { return m_buildUnit && m_buildUnit->IsWorker(); }
	size_t GetProvidedSupply() const;
	size_t GetRequiredSupply() const;
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return m_unitRequirements; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return m_researchRequirements; }

	bool HasSource() const { return true; }
	ESC2SourceType GetSourceType() const{ return eSourceTypeUnit; }
	size_t GetSourceID(const CSC2State &state) const { return m_sourceUnitTypeID; }
	bool WillConsumeSource() const { return false; }
	double GetMorphSourceDuration() const { return 0.0; }


	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;
	std::vector<wxString> m_unitRequirementNames;
	SC2UnitFlags m_unitRequirements;
	std::vector<wxString> m_researchRequirementNames;
	SC2ResearchFlags m_researchRequirements;

	wxString m_sourceUnitName;
	size_t m_sourceUnitTypeID;
	const CSC2Unit *m_sourceUnit;

	wxString m_targetUnitName;
	size_t m_targetUnitTypeID;
	const CSC2Unit *m_targetUnit;

	wxString m_targetBuildingName;
	size_t m_targetBuildingTypeID;
	const CSC2Building *m_targetBuilding;
	std::vector<wxString> m_requireTargetBuildingStatusAbsentNames;
	SC2BuildingStatusFlags m_requireTargetBuildingStatusAbsent;

	double m_mineralCost;
	double m_gasCost;
	double m_energyCost;
	size_t m_requiredSupply;

	bool m_consumeSourceUnit;
	wxString m_morphSourceUnitName;
	size_t m_morphSourceUnitTypeID;
	const CSC2Unit *m_morphSourceUnit;
	double m_morphSourceUnitTime;

	bool m_consumeTargetUnit;

	wxString m_buildBuildingName;
	size_t m_buildBuildingTypeID;
	const CSC2Building *m_buildBuilding;
	double m_buildBuildingCompletionTime;

	wxString m_buildUnitName;
	size_t m_buildUnitTypeID;
	const CSC2Unit *m_buildUnit;
	double m_buildUnitCompletionTime;

	size_t m_spawnTargetBuildingBonusLarvaeCount;
	double m_spawnTargetBuildingBonusLarvaeTime;

	std::vector<wxString> m_applyTargetBuildingStatusNames;
	size_t m_applyTargetBuildingStatusID;
	SC2BuildingStatusFlags m_applyTargetBuildingStatus;
	double m_applyTargetBuildingStatusDuration;
	double m_targetProductionBoost;
};

class CSC2ResearchCommand : public CSC2XMLCommand
{
public:
	CSC2ResearchCommand(const CSC2RaceData &raceData);
	~CSC2ResearchCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_mineralCost; }
	double GetGasCost() const { return m_gasCost; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const { return false; }
	bool WillSpawnBase() const { return false; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const { return 0; }
	size_t GetRequiredSupply() const { return 0; }
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return (SC2UnitFlags)0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return m_researchRequirements; }

	bool HasSource() const { return true; }
	ESC2SourceType GetSourceType() const{ return eSourceTypeBuilding; }
	size_t GetSourceID(const CSC2State &state) const { return m_sourceBuildingTypeID; }
	bool WillConsumeSource() const { return false; }
	double GetMorphSourceDuration() const { return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;
	std::vector<wxString> m_researchRequirementNames;
	SC2ResearchFlags m_researchRequirements;

	wxString m_sourceBuildingName;
	size_t m_sourceBuildingTypeID;
	const CSC2Building *m_sourceBuilding;
	std::vector<wxString> m_requireSourceBuildingStatusAbsentNames;
	SC2BuildingStatusFlags m_requireSourceBuildingStatusAbsent;

	std::vector<wxString> m_applySourceBuildingStatusNames;
	size_t m_applySourceBuildingStatusID;
	SC2BuildingStatusFlags m_applySourceBuildingStatus;
	double m_applySourceBuildingStatusDuration;

	double m_mineralCost;
	double m_gasCost;

	double m_completionTime;
	wxString m_completeResearchName;
	size_t m_completeResearchID;
	const CSC2Research *m_completeResearch;
};

class CSC2SequenceCommand : public CSC2XMLCommand
{
public:
	CSC2SequenceCommand(const CSC2RaceData &raceData);
	~CSC2SequenceCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_mineralCost; }
	double GetGasCost() const { return m_gasCost; }
	bool WillBuildGeyserBuilding() const { return false; }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const;
	bool WillSpawnBase() const { return false; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const;
	size_t GetRequiredSupply() const;
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return m_unitRequirements; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return m_researchRequirements; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const { wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	double m_mineralCost;
	double m_gasCost;

	SC2BuildingFlags m_buildingRequirements;
	SC2UnitFlags m_unitRequirements;
	SC2ResearchFlags m_researchRequirements;
	
	std::vector<wxString> m_commandSequenceNames;
	CVector<const CSC2Command *> m_commandSequence;
};

class CSC2MultiCommand : public CSC2XMLCommand
{
public:
	CSC2MultiCommand(const CSC2RaceData &raceData);
	~CSC2MultiCommand() {}

	bool AddToPrerequisitCommands(const CSC2Command *command, CVector<CVector<const CSC2Command *> *> &buildingPrerequisitCommands, CVector<CVector<CVector<const CSC2Command *> *> *> &buildingStatusPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &unitPrerequisitCommands, CVector<CVector<const CSC2Command *> *> &researchPrerequisitCommands) const;
	double GetMineralCost() const { return m_minimumMineralCost; }
	double GetGasCost() const { return m_minimumGasCost; }
	bool WillBuildGeyserBuilding() const { return m_willBuildGasBuilding; }
	bool RequiresGeyser() const { return false; }
	bool IsMacroAbility(SC2BuildingFlags buildings, SC2UnitFlags units, SC2ResearchFlags research) const;
	bool WillSpawnBase() const { return false; }
	bool IsBuildWorkerCommand() const { return false; }
	size_t GetProvidedSupply() const;
	size_t GetRequiredSupply() const;
	SC2BuildingFlags GetBuildingRequirementFlags() const { return m_buildingRequirements; }
	SC2UnitFlags GetUnitRequirementFlags() const { return m_unitRequirements; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return m_researchRequirements; }

	bool HasSource() const { return false; }
	ESC2SourceType GetSourceType() const{ wxASSERT(false); return eSourceTypeNone; }
	size_t GetSourceID(const CSC2State &state) const { wxASSERT(false); return 0; }
	bool WillConsumeSource() const { wxASSERT(false); return false; }
	double GetMorphSourceDuration() const { wxASSERT(false); return 0.0; }

	bool WillBuildBuilding() const { return false; }
	bool WillBuildUnit() const { return false; }
	size_t GetBuildBuildingTypeID() const { wxASSERT(false); return 0; }
	size_t GetBuildUnitTypeID() const { wxASSERT(false); return 0; }

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool HasPrerequisits(const CSC2State &state) const;
	bool HasRequirements(const CSC2State &state) const;
	double GetResourceWaitTime(const CSC2State &state) const;
	bool ExecuteCommand(CSC2State &state, CPriorityQueue<CSC2Event> &events) const;

protected:
	double m_minimumMineralCost;
	double m_minimumGasCost;
	bool m_willBuildGasBuilding;
	SC2BuildingFlags m_buildingRequirements;
	SC2UnitFlags m_unitRequirements;
	SC2ResearchFlags m_researchRequirements;
	
	std::vector<wxString> m_commandMultiNames;
	CVector<const CSC2Command *> m_commandMulti;
};


typedef CGASequenceChromosome<const CSC2Command *> CSC2NewChromosome;
typedef CGASequenceChromosome<const CSC2Command *>::CGASequenceChromosomeMutator CSC2NewMutator;
