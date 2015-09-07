#include "stdafx.h"
#include "SC2FitnessCalc.h"

CSC2FitnessCalc::~CSC2FitnessCalc()
{
	for(size_t i=0; i < m_waypoints.size(); i++)
		delete m_waypoints[i];
}

void CSC2FitnessCalc::AddWaypoint(const CSC2Waypoint &target)
{
	CSC2Waypoint *newTarget = new CSC2Waypoint(target);
	newTarget->AddRequirements();
	newTarget->RestrictRequirements();
	m_waypoints.push_back(newTarget);
	if(target.m_targetTime.max > m_timeLimit)
		m_timeLimit = target.m_targetTime.max;
}

void CSC2FitnessCalc::BuildAlphabet(CVector<const CSC2Command *> &alphabet)
{
	CSC2Waypoint target(m_raceInfo);
	for(size_t i=0; i < m_waypoints.size(); i++)
		target += *m_waypoints[i];

	target.DoPreCalculation();
	target.BuildAlphabet(alphabet, m_gasMicro);
}

bool CSC2FitnessCalc::InitialiseBuildOrder()
{
	m_initialEvents.clear();

	m_initialState.SetInitialState(m_initialEvents);

	for(size_t i=0; i < m_customEvents.size(); i++)
		m_initialEvents.add(m_customEvents[i]);

	while(m_initialEvents.size() > 0 && m_initialEvents[0].m_time < START_TIME)
		m_initialState.ProcessEvent(m_initialEvents);

	m_initialState.ProgressTime(START_TIME - m_initialState.m_time);

	m_initialState.StartMining();

	for(size_t i=0; i < m_initialBuildOrder.size(); i++)
	{
		const CSC2Command *command = m_initialBuildOrder[i];

		while(!command->HasRequirements(m_initialState))
		{
			if(m_initialEvents.size() == 0)
				return false; // Will never get resources / building state requirements
			else
			{
				// Process the event
				if(m_initialEvents[0].m_time > m_timeLimit)
					return false;

				m_initialState.ProgressTime(m_initialEvents[0].m_time - m_initialState.m_time);
				m_initialState.ProcessEvent(m_initialEvents);
			}
		}

		double resourceWaitTime = command->GetResourceWaitTime(m_initialState);
		while(0.0 < resourceWaitTime)
		{
			if(m_initialEvents.size() == 0 || m_initialEvents[0].m_time > resourceWaitTime)
			{
				if(resourceWaitTime > m_timeLimit)
					return false;

				m_initialState.ProgressTime(resourceWaitTime);
				break;
			}
			else
			{
				// Process the event
				if(m_initialEvents[0].m_time > m_timeLimit)
					return false;

				m_initialState.ProgressTime(m_initialEvents[0].m_time - m_initialState.m_time);
				m_initialState.ProcessEvent(m_initialEvents);

				resourceWaitTime = command->GetResourceWaitTime(m_initialState);
			}
		}

		command->ExecuteCommand(m_initialState, m_initialEvents);
	}

	return true;
}

void CSC2FitnessCalc::InitEvents(CPriorityQueue<CSC2Event> &events) const
{
	events = m_initialEvents;
}

class CValidateCalcFitnessResult
{
public:
	CValidateCalcFitnessResult(CFitnessValue &fitness) : m_fitness(fitness) {}

	inline bool AddRequiredCommands(CVector<const CSC2Command *> &buildOrder, size_t cmdIndex, const CSC2Command *&command, const CSC2RaceInfo &raceInfo, EGasMicro gasMicro, const CSC2State &state)
	{
		bool newCommandAdded;
		do
		{
			const CSC2Command *newCommand = raceInfo.GetRequiredCommand(gasMicro, state);
			if(newCommand && newCommand != command)
			{
				newCommandAdded = true;
				buildOrder.insert(cmdIndex, newCommand);
				command = newCommand;
			}
			else
			{
				newCommandAdded = false;
				break;
			}
		} while(newCommandAdded);

		return newCommandAdded;
	}

	inline bool AddPrerequisitCommandsAndCheckRequirements(CVector<const CSC2Command *> &buildOrder, size_t cmdIndex, const CSC2Command *&command, const CSC2RaceInfo &raceInfo, EGasMicro gasMicro, const CSC2State &state)
	{
		bool havePrerequisitRequirements = command->HasPrerequisits(state);

		while(!havePrerequisitRequirements)
		{
			const CSC2Command *newCommand = command->GetPrerequisitCommand(state, gasMicro, raceInfo.GetBuildingPrerequisitCommands(), raceInfo.GetUnitPrerequisitCommands(), raceInfo.GetResearchPrerequisitCommands(), raceInfo.GetBuildGeyserBuildingCommands(), raceInfo.GetMoveWorkersToMineralsCommands(), raceInfo.GetMoveWorkersToGasCommands(), raceInfo.GetProvideSupplyCommands());
			if(newCommand)
			{
				buildOrder.insert(cmdIndex, newCommand);
				command = newCommand;
				havePrerequisitRequirements = command->HasPrerequisits(state);
				continue;
			}

			break;
		}

		return havePrerequisitRequirements;
	}

	inline void AddStateDuration(const CSC2Waypoint *waypoint, CSC2State &state, double duration)
	{
		waypoint->AddStateDuration(m_fitness, state, duration);
	}

	inline void ProcessCommand(const CSC2Command *command, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
	}

	inline void ProcessEvent(const CSC2Event &event, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
	}

	inline void ProcessWaypointTimeOverlapsed(size_t waypointIndex, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
	}

	inline void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
		m_fitness.m_succeeded = succeeded;
		m_fitness.m_requirementsValue += waypoint->targetValue(state, succeeded);
		m_fitness.m_requirementsTime = state.m_time + waypoint->targetTimePenalty(state);
	}

	inline void ProcessValidationComplete(CVector<const CSC2Command *> &buildOrder, const CSC2Waypoint *waypoint, size_t cmdIndex, const CSC2State &state)
	{
		waypoint->AddExtraValue(m_fitness, state);

		buildOrder.truncate(cmdIndex + 1); // No point keeping the build order longer than it needs to be
	}

protected:
	CFitnessValue &m_fitness;
};

class CValidateCalcOutputResult
{
public:
	CValidateCalcOutputResult(CSC2Output &output) : m_output(output), m_failedWaypoint(false) {}

	inline bool AddRequiredCommands(CVector<const CSC2Command *> &buildOrder, size_t cmdIndex, const CSC2Command *&command, const CSC2RaceInfo &raceInfo, EGasMicro gasMicro, const CSC2State &state)
	{
		return false;
	}

	inline bool AddPrerequisitCommandsAndCheckRequirements(CVector<const CSC2Command *> &buildOrder, size_t cmdIndex, const CSC2Command *&command, const CSC2RaceInfo &raceInfo, EGasMicro gasMicro, const CSC2State &state)
	{
		return command->HasPrerequisits(state);
	}

	inline void AddStateDuration(const CSC2Waypoint *waypoint, CSC2State &state, double duration)
	{
	}

	inline void ProcessCommand(const CSC2Command *command, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
		m_output.ProcessCommand(command, *waypoint, state);
	}

	inline void ProcessEvent(const CSC2Event &event, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
		m_output.ProcessEvent(event, *waypoint, state);
	}

	inline void ProcessWaypointTimeOverlapsed(size_t waypointIndex, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
		if(m_failedWaypoint)
			return;

		m_failedWaypoint = true;

		m_output.ProcessWaypointComplete(false, waypointIndex, *waypoint, state);
	}

	inline void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint *waypoint, const CSC2State &state)
	{
		m_output.ProcessWaypointComplete(succeeded, waypointIndex, *waypoint, state);
	}

	inline void ProcessValidationComplete(CVector<const CSC2Command *> &buildOrder, const CSC2Waypoint *waypoint, size_t cmdIndex, const CSC2State &state)
	{
	}

protected:
	CSC2Output &m_output;
	bool m_failedWaypoint;
};

template<typename TResult>
void CSC2FitnessCalc::ValidateAndCalculateFitness(CVector<const CSC2Command *> &buildOrder, CSC2State &state, CPriorityQueue<CSC2Event> &events, TResult &result) const
{
	size_t cmdIndex = 0;
	const CSC2Waypoint *waypoint = NULL;
	bool satisfied = true, requirementsTested, requirementsSatisfied;
	for(size_t waypointIndex = 0; waypointIndex < m_waypoints.size(); waypointIndex++)
	{
		waypoint = m_waypoints[waypointIndex];

		requirementsTested = true;
		requirementsSatisfied = waypoint->satisfiesTarget(state);

		satisfied = requirementsSatisfied && waypoint->m_targetTime.min <= state.m_time && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time);

		if(!satisfied)
		{
			while(cmdIndex < buildOrder.size())
			{
				const CSC2Command *command = buildOrder[cmdIndex];

				result.AddRequiredCommands(buildOrder, cmdIndex, command, m_raceInfo, m_gasMicro, state);

				if(!result.AddPrerequisitCommandsAndCheckRequirements(buildOrder, cmdIndex, command, m_raceInfo, m_gasMicro, state))
					break;

				bool haveRequirements = command->HasRequirements(state);
				double resourceWaitTime = 0.0;
				if(haveRequirements)
					resourceWaitTime = command->GetResourceWaitTime(state);
				bool haveNewCommand = false;

				while(!haveRequirements || resourceWaitTime > 0.0 || (events.size() > 0 && events[0].m_time == state.m_time))
				{
					if(events.size() > 0 && (!haveRequirements || events[0].m_time <= state.m_time + resourceWaitTime))
					{
						// Process the event
						if(events[0].m_time > waypoint->m_targetTime.max)
							result.ProcessWaypointTimeOverlapsed(waypointIndex, waypoint, state);

						if(events[0].m_time > m_timeLimit)
							break;

						if(events[0].m_time > waypoint->m_targetTime.min && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
						{
							if(!requirementsTested)
							{
								requirementsTested = true;
								requirementsSatisfied = waypoint->satisfiesTarget(state);
							}

							if(requirementsSatisfied)
							{
								result.AddStateDuration(waypoint, state, waypoint->m_targetTime.min - state.m_time);
								state.ProgressTime(waypoint->m_targetTime.min - state.m_time);
								satisfied = true;
								break;
							}
						}

						result.AddStateDuration(waypoint, state, events[0].m_time - state.m_time);
						state.ProgressTime(events[0].m_time - state.m_time);

						result.ProcessEvent(events[0], waypoint, state);

						state.ProcessEvent(events);

						if(waypoint->m_targetTime.min <= state.m_time && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
						{
							requirementsTested = true;
							requirementsSatisfied = waypoint->satisfiesTarget(state);
							if(requirementsSatisfied)
							{
								satisfied = true;
								break;
							}
						}
						else
							requirementsTested = false;

						if(result.AddRequiredCommands(buildOrder, cmdIndex, command, m_raceInfo, m_gasMicro, state))
						{
							haveNewCommand = true;
							break;
						}

						haveRequirements = command->HasRequirements(state);
						if(haveRequirements)
							resourceWaitTime = command->GetResourceWaitTime(state);
					}
					else if(!haveRequirements)
						break;
					else
					{
						if(state.m_time + resourceWaitTime > waypoint->m_targetTime.max)
							result.ProcessWaypointTimeOverlapsed(waypointIndex, waypoint, state);

						if(state.m_time + resourceWaitTime > m_timeLimit)
							break;
						else
						{
							result.AddStateDuration(waypoint, state, resourceWaitTime);
							state.ProgressTime(resourceWaitTime);
							resourceWaitTime = 0.0;
						}
					}
				}

				if(haveNewCommand)
					continue;

				if(satisfied || !haveRequirements || resourceWaitTime > 0.0)
					break;

				result.ProcessCommand(command, waypoint, state);

				if(!command->ExecuteCommand(state, events))
				{
					haveRequirements = false;
					break;
				}

				cmdIndex++;

				if(waypoint->m_targetTime.min <= state.m_time && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
				{
					requirementsTested = true;
					requirementsSatisfied = waypoint->satisfiesTarget(state);
					if(requirementsSatisfied)
					{
						satisfied = true;
						break;
					}
				}
				else
					requirementsTested = false;

				if(0.0 < m_minimumCommandDuration || 0.0 < command->GetPostCommandDuration())
				{
					double minimumPostCommandTime = state.m_time + mymax(m_minimumCommandDuration, command->GetPostCommandDuration());
					while(events.size() > 0 && events[0].m_time <= minimumPostCommandTime)
					{
						// Process the event
						if(events[0].m_time > waypoint->m_targetTime.max)
							result.ProcessWaypointTimeOverlapsed(waypointIndex, waypoint, state);

						if(events[0].m_time > m_timeLimit)
							break;

						if(events[0].m_time > waypoint->m_targetTime.min && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
						{
							if(!requirementsTested)
							{
								requirementsTested = true;
								requirementsSatisfied = waypoint->satisfiesTarget(state);
							}

							if(requirementsSatisfied)
							{
								result.AddStateDuration(waypoint, state, waypoint->m_targetTime.min - state.m_time);
								state.ProgressTime(waypoint->m_targetTime.min - state.m_time);
								satisfied = true;
								break;
							}
						}

						result.AddStateDuration(waypoint, state, events[0].m_time - state.m_time);
						state.ProgressTime(events[0].m_time - state.m_time);

						result.ProcessEvent(events[0], waypoint, state);

						state.ProcessEvent(events);

						if(waypoint->m_targetTime.min <= state.m_time && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
						{
							requirementsTested = true;
							requirementsSatisfied = waypoint->satisfiesTarget(state);
							if(requirementsSatisfied)
							{
								satisfied = true;
								break;
							}
						}
						else
							requirementsTested = false;
					}

					if(satisfied)
						break;

					if(minimumPostCommandTime > state.m_time)
					{
						if(minimumPostCommandTime > waypoint->m_targetTime.max)
							result.ProcessWaypointTimeOverlapsed(waypointIndex, waypoint, state);

						if(minimumPostCommandTime > m_timeLimit)
							break;

						result.AddStateDuration(waypoint, state, minimumPostCommandTime - state.m_time);
						state.ProgressTime(minimumPostCommandTime - state.m_time);

						if(!requirementsTested && waypoint->m_targetTime.min <= state.m_time && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
						{
							requirementsTested = true;
							requirementsSatisfied = waypoint->satisfiesTarget(state);
							if(requirementsSatisfied)
							{
								satisfied = true;
								break;
							}
						}
						else
							requirementsTested = false;
					}
				}
			}
		}

		if(!satisfied)
		{
			while(events.size() > 0)
			{
				if(events[0].m_time > m_timeLimit)
					break;

				if(events[0].m_time > waypoint->m_targetTime.min && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
				{
					if(!requirementsTested)
					{
						requirementsTested = true;
						requirementsSatisfied = waypoint->satisfiesTarget(state);
					}
					if(requirementsSatisfied)
					{
						result.AddStateDuration(waypoint, state, waypoint->m_targetTime.min - state.m_time);
						state.ProgressTime(waypoint->m_targetTime.min - state.m_time);
						satisfied = true;
						break;
					}
				}

				result.AddStateDuration(waypoint, state, events[0].m_time - state.m_time);
				state.ProgressTime(events[0].m_time - state.m_time);

				result.ProcessEvent(events[0], waypoint, state);

				state.ProcessEvent(events);

				if(waypoint->m_targetTime.min <= state.m_time && (waypoint->m_targetTime.max == 0.0 || waypoint->m_targetTime.max >= state.m_time))
				{
					requirementsTested = true;
					requirementsSatisfied = waypoint->satisfiesTarget(state);
					if(requirementsSatisfied)
					{
						satisfied = true;
						break;
					}
				}
				else
					requirementsTested = false;
			}
		}

		if(!satisfied && state.m_time < waypoint->m_targetTime.min)
		{
			if(!requirementsTested)
			{
				requirementsTested = true;
				requirementsSatisfied = waypoint->satisfiesTarget(state);
			}

			if(requirementsSatisfied)
			{
				satisfied = true;
				result.AddStateDuration(waypoint, state, waypoint->m_targetTime.min - state.m_time);
				state.ProgressTime(waypoint->m_targetTime.min - state.m_time);
			}
		}

		result.ProcessWaypointComplete(satisfied, waypointIndex, waypoint, state);

		if(!satisfied)
			break;
	}

	result.ProcessValidationComplete(buildOrder, waypoint, cmdIndex, state);
}

bool CSC2FitnessCalc::ValidateAndCalculateFitness(CGASequenceChromosome<const CSC2Command *> &chromosome, CFitnessValue &fitness) const
{
	CSC2State state(m_initialState);
	CPriorityQueue<CSC2Event> events;
	InitEvents(events);

	CValidateCalcFitnessResult result(fitness);
	ValidateAndCalculateFitness(chromosome.GetValue(), state, events, result);

	return true;
}

CSC2State *CSC2FitnessCalc::PrintGame(CSC2Output &output, const CGASequenceChromosome<const CSC2Command *> &chromosome) const
{
	CSC2State *state = new CSC2State(m_race, m_raceInfo.GetData());

	CPriorityQueue<CSC2Event> events;
	state->SetInitialState(events);

	for(size_t i=0; i < m_customEvents.size(); i++)
		events.add(m_customEvents[i]);

	while(events.size() > 0 && events[0].m_time < START_TIME)
	{
		state->ProgressTime(events[0].m_time - state->m_time);

		output.ProcessEvent(events[0], *m_waypoints[0], *state);

		state->ProcessEvent(events);
	}

	state->ProgressTime(START_TIME - state->m_time);

	state->StartMining();

	CVector<const CSC2Command *> buildOrder(m_initialBuildOrder);
	buildOrder.append(chromosome.GetValue());

	CValidateCalcOutputResult result(output);
	ValidateAndCalculateFitness(buildOrder, *state, events, result);

	return state;
}
