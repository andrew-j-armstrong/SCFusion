#pragma once

#include <iostream>
#include <stdio.h>
#include "../Core/Vector.h"
#include "../Core/PriorityQueue.h"
#include "FitnessValue.h"
#include "GasMicro.h"
#include "SC2State.h"
#include "SC2Event.h"
#include "SC2Output.h"
#include "SC2RaceInfo.h"

#define START_TIME 2.0 // Assume it takes 2 secs to start building/mining anything

class CSC2FitnessCalc
{
public:
	CSC2FitnessCalc(ESC2Race race, const CSC2RaceInfo &raceInfo, double timeLimit, EGasMicro gasMicro, double minimumCommandDuration) : m_race(race), m_raceInfo(raceInfo), m_timeLimit(timeLimit), m_gasMicro(gasMicro), m_minimumCommandDuration(minimumCommandDuration), m_initialState(race, raceInfo.GetData()), m_initialEvents() {}
	~CSC2FitnessCalc();

	void SetInitialBuildOrder(const CVector<const CSC2Command *> &initialBuildOrder) { m_initialBuildOrder = initialBuildOrder; }
	bool InitialiseBuildOrder();
	void InitData(CSC2State &state, CPriorityQueue<CSC2Event> &events) const { state = m_initialState; InitEvents(events); }
	void InitEvents(CPriorityQueue<CSC2Event> &events) const;
	void BuildAlphabet(CVector<const CSC2Command *> &alphabet);
	void AddWaypoint(const CSC2Waypoint &target);

	void ClearCustomEvents() { m_customEvents.erase(0, m_customEvents.size()); }
	void AddCustomEvent(const CSC2Event &event) { m_customEvents.push_back(event); }

	bool ValidateAndCalculateFitness(CGASequenceChromosome<const CSC2Command *> &chromosome, CFitnessValue &fitness) const;
	bool SatisfiesTarget(const CGASequenceChromosome<const CSC2Command *> &chromosome) const { return false; }

	CSC2State *PrintGame(CSC2Output &output, const CGASequenceChromosome<const CSC2Command *> &chromosome) const;

	const CVector<const CSC2Waypoint *> &GetTargets() const { return m_waypoints; }

protected:
	template<typename TResult>
	void ValidateAndCalculateFitness(CVector<const CSC2Command *> &buildOrder, CSC2State &state, CPriorityQueue<CSC2Event> &events, TResult &result) const;

	ESC2Race m_race;
	const CSC2RaceInfo &m_raceInfo;
	double m_timeLimit;
	EGasMicro m_gasMicro;
	double m_minimumCommandDuration;
	CVector<const CSC2Waypoint *> m_waypoints;
	CVector<CSC2Event> m_customEvents;
	CVector<const CSC2Command *> m_initialBuildOrder;
	CSC2State m_initialState;
	CPriorityQueue<CSC2Event> m_initialEvents;
};
