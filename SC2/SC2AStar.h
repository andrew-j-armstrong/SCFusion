#pragma once

#include "../Core/Vector.h"
#include "../Core/HashFunction.h"
#include "../AStar/ASEngine.h"
#include "../AStar/ASSingleParent.h"
#include "../AStar/ASPriorityQueueOpenListManager.h"
#include "../AStar/ASBuildStateNode.h"
#include "../AStar/ASNoDuplicateManager.h"

#include "ProtossEvent.h"
#include "ProtossTarget.h"

#include "TerranEvent.h"
#include "TerranTarget.h"

#include "ZergEvent.h"
#include "ZergTarget.h"


template<typename TTarget, typename TState, typename TEvent>
class CSC2ASConfig
{
public:
	CSC2ASConfig(const CFitnessCalc<TTarget, TState, TEvent> &fitnessCalc, const CVector<ESC2Command> &alphabet) : m_fitnessCalc(fitnessCalc), m_alphabet(alphabet) {}

	const CFitnessCalc<TTarget, TState, TEvent> &GetFitnessCalc() const { return m_fitnessCalc; }
	const CVector<ESC2Command> &GetAlphabet() const { return m_alphabet; }

protected:
	const CFitnessCalc<TTarget, TState, TEvent> &m_fitnessCalc;
	const CVector<ESC2Command> &m_alphabet;
};

class CSC2ASCost
{
public:
	CSC2ASCost() : m_time(0.0) {}
	CSC2ASCost(double time) : m_time(time) {}
	CSC2ASCost(const CSC2ASCost &cost) : m_time(cost.GetTime()) {}

	CSC2ASCost operator+(const CSC2ASCost &cost) const { return CSC2ASCost(m_time + cost.GetTime()); }
	void operator+=(const CSC2ASCost &cost) { m_time += cost.GetTime(); }

	bool operator< (const CSC2ASCost &cost) const { return m_time < cost.GetTime(); }

	double GetTime() const { return m_time; }

protected:
	double m_time;
};

template<typename TTarget, typename TState, typename TEvent>
class CSC2ASState
{
public:
	CSC2ASState() : m_waypointIndex(0), m_state(), m_events() { }
	CSC2ASState(const CSC2ASState<TTarget, TState, TEvent> &state) : m_waypointIndex(0), m_state(), m_events() { *this = state; }

	~CSC2ASState() {}

	void operator= (const CSC2ASState<TTarget, TState, TEvent> &state)
	{
		m_waypointIndex = state.GetWaypointIndex();
		m_state = state.GetState();
		m_events = state.GetEvents();
	}

	bool operator==(const CSC2ASState<TTarget, TState, TEvent> &state) const
	{
		return (m_waypointIndex == state.GetWaypointIndex()
			&& m_state == state.GetState()
			&& m_events == state.GetEvents());
	}

	void Init(const CSC2ASConfig<TTarget, TState, TEvent> &config)
	{
		m_waypointIndex = 0;
		config.GetFitnessCalc().InitData(m_state, m_events);
	}

	operator unsigned long() const { return MurmurHash2(this, sizeof(this), 5432); }

	bool IsCompleted(const CSC2ASConfig<TTarget, TState, TEvent> &config) const
	{
		return m_waypointIndex == config.GetFitnessCalc().GetTargets().size();
	}

	void GetTransitions(const CSC2ASConfig<TTarget, TState, TEvent> &config, CVector<ESC2Command> &transitions) const
	{
		for(size_t i=0; i < config.GetAlphabet().size(); i++)
		{
			if(m_state.WillHaveRequirements(config.GetAlphabet()[i]))
				transitions.push_back(config.GetAlphabet()[i]);
		}
	}

	CSC2ASCost CalculateExpectedCost(const CSC2ASConfig<TTarget, TState, TEvent> &config);

	CSC2ASCost ApplyTransition(const CSC2ASConfig<TTarget, TState, TEvent> &config, CSC2ASCost curCost, const ESC2Command &transition);

	size_t GetWaypointIndex() const { return m_waypointIndex; }
	const TState &GetState() const { return m_state; }
	const CPriorityQueue<TEvent> &GetEvents() const { return m_events; }

protected:
	size_t m_waypointIndex;
	TState m_state;
	CPriorityQueue<TEvent> m_events;
};

template<typename TTarget, typename TState, typename TEvent>
CSC2ASCost CSC2ASState<TTarget, TState, TEvent>::ApplyTransition(const CSC2ASConfig<TTarget, TState, TEvent> &config, CSC2ASCost curCost, const ESC2Command &transition)
{
	if(!m_state.WillHaveRequirements(transition))
		return CSC2ASCost(9999999999);

	TState::CResourceCost cost;
	m_state.GetCost(cost, transition);

	bool bHaveCost = m_state.HasResources(cost);
	bool bHaveStateRequirements = m_state.HasRequirements(transition);

	const TTarget *target = config.GetFitnessCalc().GetTargets()[m_waypointIndex];

	while(!bHaveCost || !bHaveStateRequirements)
	{
		double resourceWaitTime;
		bool bWillGetResources = true;

		bool bWaitForEvent = !bHaveStateRequirements;

		if(!bWaitForEvent)
		{
			bWillGetResources = m_state.GetResourceWaitTime(cost, resourceWaitTime);
			bWaitForEvent = !bWillGetResources || (m_events.size() > 0 && m_events[0].time() < m_state.m_time + resourceWaitTime);
		}

		if(bWaitForEvent)
		{
			if(0 == m_events.size())
				return CSC2ASCost(9999999999);	// Will never get resources / building state requirements
			else
			{
				// Process the event
				m_state.ProcessEvent(m_events);

				if(target->satisfiesTarget(m_state))
				{
					m_waypointIndex++;
					if(m_waypointIndex >= config.GetFitnessCalc().GetTargets().size())
						return CSC2ASCost(m_state.m_time);
					target = config.GetFitnessCalc().GetTargets()[m_waypointIndex];
				}

				bHaveStateRequirements = m_state.HasRequirements(transition);
				if(bHaveStateRequirements)
					bHaveCost = m_state.HasResources(cost);
			}
		}
		else if(!bHaveStateRequirements)
			return CSC2ASCost(9999999999);	// Will never have requirements
		else if(!bWillGetResources)
			return CSC2ASCost(9999999999);	// Will never get resources
		else
		{
			if(m_state.m_time + resourceWaitTime > target->m_targetTime.max)
				return CSC2ASCost(9999999999);

			m_state.ProgressTime(resourceWaitTime);
			bHaveCost = true;
		}
	}

	m_state.SpendResources(cost);
	m_state.ExecuteCommand(target->m_targetTime.max, transition, m_events);

	if(target->satisfiesTarget(m_state))
		m_waypointIndex++;

	return CSC2ASCost(m_state.m_time);
}

template<> CSC2ASCost CSC2ASState<CProtossTarget, CProtossState, CProtossEvent>::CalculateExpectedCost(const CSC2ASConfig<CProtossTarget, CProtossState, CProtossEvent> &config);
template<> CSC2ASCost CSC2ASState<CTerranTarget, CTerranState, CTerranEvent>::CalculateExpectedCost(const CSC2ASConfig<CTerranTarget, CTerranState, CTerranEvent> &config);
template<> CSC2ASCost CSC2ASState<CZergTarget, CZergState, CZergEvent>::CalculateExpectedCost(const CSC2ASConfig<CZergTarget, CZergState, CZergEvent> &config);

typedef CSC2ASState<CProtossTarget, CProtossState, CProtossEvent> CProtossASState;
typedef CSC2ASState<CTerranTarget, CTerranState, CTerranEvent> CTerranASState;
typedef CSC2ASState<CZergTarget, CZergState, CZergEvent> CZergASState;

typedef CSC2ASConfig<CProtossTarget, CProtossState, CProtossEvent> CProtossASConfig;
typedef CSC2ASConfig<CTerranTarget, CTerranState, CTerranEvent> CTerranASConfig;
typedef CSC2ASConfig<CZergTarget, CZergState, CZergEvent> CZergASConfig;

typedef CASEngine<CProtossASConfig, CSC2ASState<CProtossTarget, CProtossState, CProtossEvent>, CASSingleParent, CASBuildStateNode, ESC2Command, CSC2ASCost, CASNoDuplicateManager, CASPriorityQueueOpenListManager> CASProtossEngine;
typedef CASEngine<CTerranASConfig, CSC2ASState<CTerranTarget, CTerranState, CTerranEvent>, CASSingleParent, CASBuildStateNode, ESC2Command, CSC2ASCost, CASNoDuplicateManager, CASPriorityQueueOpenListManager> CASTerranEngine;
typedef CASEngine<CZergASConfig, CSC2ASState<CZergTarget, CZergState, CZergEvent>, CASSingleParent, CASBuildStateNode, ESC2Command, CSC2ASCost, CASNoDuplicateManager, CASPriorityQueueOpenListManager> CASZergEngine;
