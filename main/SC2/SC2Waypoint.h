#pragma once

#include "../GA/GAEngine.h"
#include "../GA/GASequenceChromosome.h"
#include "FitnessValue.h"
#include "SC2Command.h"
#include "SC2State.h"
#include "GasMicro.h"
#include "TargetMinMax.h"
#include "SC2RaceInfo.h"

class CSC2Waypoint
{
public:
	CSC2Waypoint(const CSC2RaceInfo &raceInfo);
	~CSC2Waypoint() {}

	void SetDefaults();

	double targetValue(const CSC2State &state, bool bSatisfied) const;
	double targetTimePenalty(const CSC2State &state) const;
	void AddExtraValue(CFitnessValue &fitness, const CSC2State &state) const;

	void AddStateDuration(CFitnessValue &fitness, const CSC2State &state, double time) const;

	bool hasTarget() const;
	bool satisfiesTarget(const CSC2State &state) const;

	void AddRequirements();
	void RestrictRequirements();

	void operator+=(const CSC2Waypoint &target);

	void DoPreCalculation();
	void BuildAlphabet(CVector<const CSC2Command *> &alphabet, EGasMicro gasMicro) const;

	const CSC2RaceInfo &m_raceInfo;

	// State
	CDoubleMinMax m_targetTime;
	CSizeTOptionalMinMax m_supplyCount;
	CSizeTOptionalMinMax m_baseCount;
	CSizeTOptionalMinMax m_baseUnderConstructionCount;

	// Resources
	double m_minerals;
	double m_gas;
	double m_mineralsMined;
	double m_gasMined;

	// Targets
	struct STargetRequirement
	{
		STargetRequirement(const CSC2Target *target = NULL, size_t count = 0) : m_target(target), m_count(count) {}
		~STargetRequirement() {}

		const CSC2Target *m_target;
		size_t m_count;
	};
	CVector<STargetRequirement> m_minRequirements;
	CVector<STargetRequirement> m_maxRequirements;
	CVector<STargetRequirement> m_minUnderConstructionRequirements;
	CVector<STargetRequirement> m_maxUnderConstructionRequirements;

	CVector<const CSC2Target *> m_stateDurationRequirements;

protected:
	SC2BuildingFlags m_buildingMinRequirements;
	SC2BuildingFlags m_buildingMaxRequirements;
	CVector<SC2BuildingStatusFlags> m_buildingStatusMinRequirements;
	CVector<SC2BuildingStatusFlags> m_buildingStatusMaxRequirements;
	SC2UnitFlags m_unitMinRequirements;
	SC2UnitFlags m_unitMaxRequirements;
	SC2ResearchFlags m_researchMinRequirements;
	SC2ResearchFlags m_researchMaxRequirements;

	WX_DECLARE_HASH_MAP(const CSC2Target *, size_t, wxPointerHash, wxPointerEqual, wxHashMapSC2TargetToCount);
};
