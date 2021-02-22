#pragma once

#include <iostream>
#include <stdio.h>
#include "../Core/Vector.h"
#include "../Core/PriorityQueue.h"
#include "FitnessValue.h"
#include "OutputFormat.h"
#include "GasMicro.h"

#define START_TIME 2.0 // Assume it takes 2 secs to start building/mining anything

template <typename TTarget, typename TState, typename TEvent>
class CFitnessCalc
{
public:
	CFitnessCalc(ESC2Race race, double timeLimit, EGasMicro gasMicro) : m_race(race), m_timeLimit(timeLimit), m_gasMicro(gasMicro), m_initialEvents() {}
	~CFitnessCalc();

	void SetInitialBuildOrder(const CVector<ESC2Command> &initialBuildOrder) { m_initialBuildOrder = initialBuildOrder; }
	bool InitialiseBuildOrder();
	void InitData(TState &state, CPriorityQueue<TEvent> &events) const { state = m_initialState; InitEvents(events); }
	void InitEvents(CPriorityQueue<TEvent> &events) const;
	void BuildAlphabet(CVector<ESC2Command> &alphabet, bool allowWaiESC2Commands);
	void AddWaypoint(const TTarget &target);

	void ClearCustomEvents() { m_customEvents.erase(0, m_customEvents.size()); }
	void AddCustomEvent(const TEvent &event) { m_customEvents.push_back(event); }

	bool ValidateAndCalculateFitness(CGASequenceChromosome<ESC2Command> &chromosome, CFitnessValue &fitness) const;
	bool SatisfiesTarget(const CGASequenceChromosome<ESC2Command> &chromosome) const { return false; }

	void PrintGame(EOutputFormat format, wxString &output, const CGASequenceChromosome<ESC2Command> &chromosome, TState &state) const;

	template<typename TOutput>
	void PrintGame(TOutput &output, const CGASequenceChromosome<ESC2Command> &chromosome, TState &state) const;

	const CVector<const TTarget *> &GetTargets() const { return m_targets; }

protected:
	ESC2Race m_race;
	double m_timeLimit;
	EGasMicro m_gasMicro;
	CVector<const TTarget *> m_targets;
	CVector<TEvent> m_customEvents;
	CVector<ESC2Command> m_initialBuildOrder;
	TState m_initialState;
	CPriorityQueue<TEvent> m_initialEvents;
};

template <typename TTarget, typename TState, typename TEvent>
CFitnessCalc<TTarget, TState, TEvent>::~CFitnessCalc()
{
	for(size_t i=0; i < m_targets.size(); i++)
		delete m_targets[i];
}

template <typename TTarget, typename TState, typename TEvent>
void CFitnessCalc<TTarget, TState, TEvent>::AddWaypoint(const TTarget &target)
{
	TTarget *newTarget = new TTarget(target);
	newTarget->AddRequirements();
	newTarget->RestrictRequirements();
	m_targets.push_back(newTarget);
	if(target.m_targetTime.max > m_timeLimit)
		m_timeLimit = target.m_targetTime.max;
}

template <typename TTarget, typename TState, typename TEvent>
void CFitnessCalc<TTarget, TState, TEvent>::BuildAlphabet(CVector<ESC2Command> &alphabet, bool allowWaiESC2Commands)
{
	TTarget target;
	for(size_t i=0; i < m_targets.size(); i++)
		target += *m_targets[i];

	target.BuildAlphabet(alphabet, m_gasMicro, allowWaiESC2Commands);
}

template <typename TTarget, typename TState, typename TEvent>
bool CFitnessCalc<TTarget, TState, TEvent>::InitialiseBuildOrder()
{
	m_initialEvents.clear();

	m_initialState.SetInitialState();

	for(size_t i=0; i < m_customEvents.size(); i++)
		m_initialEvents.add(m_customEvents[i]);

	while(m_initialEvents.size() > 0 && m_initialEvents[0].time() < START_TIME)
		m_initialState.ProcessEvent(m_initialEvents);

	m_initialState.ProgressTime(START_TIME - m_initialState.m_time);

	m_initialState.StartMining();

	for(size_t i=0; i < m_initialBuildOrder.size(); i++)
	{
		ESC2Command &command = m_initialBuildOrder[i];

		TState::CResourceCost cost;
		m_initialState.GetCost(cost, command);

		bool bHaveCost = m_initialState.HasResources(cost);
		bool bHaveStateRequirements = m_initialState.HasRequirements(command);

		while(!bHaveCost || !bHaveStateRequirements)
		{
			double resourceWaitTime;
			bool bWillGetResources = true;

			bool bWaitForEvent = !bHaveStateRequirements;

			if(!bWaitForEvent)
			{
				bWillGetResources = m_initialState.GetResourceWaitTime(cost, resourceWaitTime);
				bWaitForEvent = !bWillGetResources || (m_initialEvents.size() > 0 && m_initialEvents[0].time() < m_initialState.m_time + resourceWaitTime);
			}

			if(bWaitForEvent)
			{
				if(m_initialEvents.size() == 0)
					break; // Will never get resources / building state requirements
				else
				{
					// Process the event
					if(m_initialEvents[0].time() > m_timeLimit)
						break;

					m_initialState.ProcessEvent(m_initialEvents);

					bHaveStateRequirements = m_initialState.HasRequirements(command);
					if(bHaveStateRequirements)
						bHaveCost = m_initialState.HasResources(cost);
				}
			}
			else if(!bHaveStateRequirements)
				break; // Will never get the state requirements
			else if(!bWillGetResources)
				break; // Will never get resources
			else
			{
				if(m_initialState.m_time + resourceWaitTime > m_timeLimit)
					break;
				else
				{
					m_initialState.ProgressTime(resourceWaitTime);
					bHaveCost = true;
				}
			}
		}

		if(!bHaveCost || !bHaveStateRequirements)
			return false;

		m_initialState.SpendResources(cost);
		m_initialState.ExecuteCommand(m_timeLimit, command, m_initialEvents);
	}

	return true;
}

template <typename TTarget, typename TState, typename TEvent>
void CFitnessCalc<TTarget, TState, TEvent>::InitEvents(CPriorityQueue<TEvent> &events) const
{
	events = m_initialEvents;
}

template <typename TTarget, typename TState, typename TEvent>
bool CFitnessCalc<TTarget, TState, TEvent>::ValidateAndCalculateFitness(CGASequenceChromosome<ESC2Command> &chromosome, CFitnessValue &fitness) const
{
	TState state(m_initialState);
	CPriorityQueue<TEvent> events;
	size_t targetIndex = 0;
	InitEvents(events);

	size_t cmdIndex = 0;
	bool bSatisfied = true;
	const TTarget *target;
	for(; targetIndex < m_targets.size() && bSatisfied; targetIndex++)
	{
		target = m_targets[targetIndex];

		bSatisfied = false;

		while(cmdIndex < chromosome.GetValue().size())
		{
			ESC2Command command = chromosome.GetValue()[cmdIndex];

			bool bNewCommandAdded = false;

			bool bHaveBuildingRequirements = state.WillHaveRequirements(command);
			while(!bHaveBuildingRequirements)
			{
				ESC2Command newCommand = state.GetPrerequisitCommand(command);
				if(newCommand)
				{
					bNewCommandAdded = true;
					chromosome.GetValue().insert(cmdIndex, newCommand);
					command = newCommand;
					bHaveBuildingRequirements = state.WillHaveRequirements(command);
					continue;
				}

/*				newCommand = state.GetReplacementCommand(*command);
				if(newCommand)
				{
					bNewCommandAdded = true;
					value[cmdIndex] = newCommand;
					bHaveBuildingRequirements = state.WillHaveRequirements(*command);
					continue;
				}
*/
				break;
			}

			if(!bHaveBuildingRequirements)
			{
				if(bNewCommandAdded)
					break;

/*				// Delete the command
				value.erase(cmdIndex);
				continue;
*/
				break;
			}

			TState::CResourceCost cost;
			state.GetCost(cost, command);

			bool bHaveCost = state.HasResources(cost);
			bool bHaveStateRequirements = state.HasRequirements(command);
			bool bHaveNewCommand = false;

			while(!bHaveCost || !bHaveStateRequirements)
			{
				double resourceWaitTime;
				bool bWillGetResources = true;

				bool bWaitForEvent = !bHaveStateRequirements;

				if(!bWaitForEvent)
				{
					bWillGetResources = state.GetResourceWaitTime(cost, resourceWaitTime);
					bWaitForEvent = !bWillGetResources || (events.size() > 0 && events[0].time() < state.m_time + resourceWaitTime);
				}

				if(bWaitForEvent)
				{
					if(events.size() == 0)
						break; // Will never get resources / building state requirements
					else
					{
						// Process the event
						if(events[0].time() > m_timeLimit)
							break;

						state.ProcessEvent(events);

						if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
						{
							bSatisfied = true;
							break;
						}

						ESC2Command newCommand = state.GetNewCommand(m_gasMicro);
						if(newCommand && newCommand != command)
						{
							bHaveNewCommand = true;
							chromosome.GetValue().insert(cmdIndex, newCommand);
							// command = newCommand; // Will reloop, so no need
							break;
						}

						bHaveStateRequirements = state.HasRequirements(command);
						if(bHaveStateRequirements)
							bHaveCost = state.HasResources(cost);
					}
				}
				else if(!bHaveStateRequirements)
					break; // Will never get the state requirements
				else if(!bWillGetResources)
					break; // Will never get resources
				else
				{
					if(state.m_time + resourceWaitTime > m_timeLimit)
						break;
					else
					{
						state.ProgressTime(resourceWaitTime);
						bHaveCost = true;
					}
				}
			}

			if(bHaveNewCommand)
				continue;

			if(bSatisfied || !bHaveCost || !bHaveStateRequirements)
				break;

			state.SpendResources(cost);
			state.ExecuteCommand(target->m_targetTime.max, command, events);

			if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
			{
				bSatisfied = true;
				break;
			}

			cmdIndex++;

			ESC2Command newCommand = state.GetNewCommand(m_gasMicro);
			if(newCommand && newCommand != chromosome.GetValue()[cmdIndex])
				chromosome.GetValue().insert(cmdIndex, newCommand);
		}

		if(!bSatisfied)
		{
			while(events.size() > 0)
			{
				if(events[0].time() > m_timeLimit)
					break;

				state.ProcessEvent(events);

				if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
				{
					bSatisfied = true;
					break;
				}
			}
		}

		if(!bSatisfied && state.m_time < target->m_targetTime.min && target->satisfiesTarget(state))
		{
			state.ProgressTime(target->m_targetTime.min - state.m_time);
			bSatisfied = true;
		}

		fitness.m_succeeded = bSatisfied;
		fitness.m_requirementsValue += target->targetValue(state, bSatisfied);
		fitness.m_requirementsTime = state.m_time + target->targetTimePenalty(state);
	}

	target->AddExtraValue(fitness, state);

	chromosome.GetValue().truncate(cmdIndex + 1); // No point keeping the build order longer than it needs to be

	return true;
}

template <typename TTarget, typename TState, typename TEvent>
void CFitnessCalc<TTarget, TState, TEvent>::PrintGame(EOutputFormat format, wxString &output, const CGASequenceChromosome<ESC2Command> &chromosome, TState &state) const
{
	CVector<ESC2Command> buildOrder = m_initialBuildOrder;
	buildOrder.append(chromosome.GetValue());

	output = "";

	state.SetInitialState();

	CPriorityQueue<TEvent> events;
	for(size_t i=0; i < m_customEvents.size(); i++)
		events.add(m_customEvents[i]);

	while(events.size() > 0 && events[0].time() < START_TIME)
	{
		if(DisplayEvent(format, events[0].event()))
		{
			switch(format)
			{
			case eOutputFormatFull:
				output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
				state.PrintSummary(output);
				output.Append(L" - ");
				break;
			}
			output += wxString::Format(L"(%s)\n", tostring(format, events[0].event()));
		}

		state.ProcessEvent(events);
	}

	state.ProgressTime(START_TIME - state.m_time);

	state.StartMining();

	switch(format)
	{
	case eOutputFormatYABOT:
		GetInitialYABOTOutput(m_race, output);
		break;
	case eOutputFormatHaploid:
		output += wxString::Format(L"Please check out Haploid's build order tester for details: http://sc2calc.org/build_order/\n\n# Startup mining delay = %d seconds\n# Startup build delay = %d seconds\n", (int)state.m_time, (int)state.m_time);
		break;
	}

	size_t cmdIndex = 0;
	bool bSatisfied = true, bRanOutOfTime = false;
	for(size_t targetIndex=0; targetIndex < m_targets.size() && bSatisfied; targetIndex++)
	{
		const TTarget *target = m_targets[targetIndex];

		bSatisfied = false;

		while(cmdIndex < buildOrder.size())
		{
			ESC2Command command = buildOrder[cmdIndex];

			if(!state.WillHaveRequirements(command))
				break;

			TState::CResourceCost cost;
			state.GetCost(cost, command);

			bool bHaveCost = state.HasResources(cost);
			bool bHaveStateRequirements = state.HasRequirements(command);

			while(!bHaveCost || !bHaveStateRequirements)
			{
				double resourceWaitTime = 0.0;
				bool bWillGetResources = true;

				bool bWaitForEvent = !bHaveStateRequirements;

				if(!bWaitForEvent)
				{
					bWillGetResources = state.GetResourceWaitTime(cost, resourceWaitTime);
					bWaitForEvent = !bWillGetResources || (events.size() > 0 && events[0].time() < state.m_time + resourceWaitTime);
				}

				if(bWaitForEvent)
				{
					if(events.size() == 0)
						break; // Will never get resources / building state requirements
					else
					{
						// Process the event
						if(!bRanOutOfTime && events[0].time() > target->m_targetTime.max)
						{
							bRanOutOfTime = true;

							switch(format)
							{
							case eOutputFormatMinimal:
							case eOutputFormatSimple:
							case eOutputFormatDetailed:
							case eOutputFormatFull:
								output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, bSatisfied ? L"satisfied" : L"failed");
								output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
								state.PrintDetails(output);
								output += '\n';
								break;
							}
						}

						if(events[0].time() > m_timeLimit)
							break;

						state.ProgressTime(events[0].time() - state.m_time);

						if(DisplayEvent(format, events[0].event()))
						{
							switch(format)
							{
							case eOutputFormatFull:
								output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
								state.PrintSummary(output);
								output.Append(L" - ");
								break;
							}
							output += wxString::Format(L"(%s)\n", tostring(format, events[0].event()));
						}

						state.ProcessEvent(events);

						if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
						{
							bSatisfied = true;
							break;
						}

						if(!bHaveStateRequirements)
							bHaveStateRequirements = state.HasRequirements(command);
						if(bHaveStateRequirements && !bHaveCost)
							bHaveCost = state.HasResources(cost);
					}
				}
				else if(!bHaveStateRequirements)
					break; // Will never get the state requirements
				else if(!bWillGetResources)
					break; // Will never get resources
				else
				{
					if(!bRanOutOfTime && state.m_time + resourceWaitTime > target->m_targetTime.max)
					{
						bRanOutOfTime = true;

						switch(format)
						{
						case eOutputFormatMinimal:
						case eOutputFormatSimple:
						case eOutputFormatDetailed:
						case eOutputFormatFull:
							output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, bSatisfied ? L"satisfied" : L"failed");
							output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
							state.PrintDetails(output);
							output += '\n';
							break;
						}
					}

					if(state.m_time + resourceWaitTime > m_timeLimit)
						break;
					else
					{
						state.ProgressTime(resourceWaitTime);
						bHaveCost = true;
					}
				}
			}

			if(bSatisfied || !bHaveCost || !bHaveStateRequirements)
				break;

			if(DisplayCommand(format, command))
			{
				switch(format)
				{
				case eOutputFormatYABOT:
					output += wxString::Format(L"%d %d %d %d 1 ", (int)(state.m_supply), (int)(state.m_minerals), (int)(state.m_gas), (int)state.m_time);
					break;

				case eOutputFormatMinimal:
				case eOutputFormatSimple:
				case eOutputFormatHaploid:
				case eOutputFormatSC2Gears:
					output += wxString::Format(L"%d ", (int)(state.m_supply));
					break;

				case eOutputFormatDetailed:
				case eOutputFormatFull:
					output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
					state.PrintSummary(output);
					output.Append(L" - ");
					break;
				}
				output += wxString::Format(L"%s", tostring(format, command));
				switch(format)
				{
				case eOutputFormatYABOT:
					output.Append(L" | ");
					break;
				default:
					output.Append(L"\n");
					break;
				}
			}

			state.SpendResources(cost);
			state.ExecuteCommand(target->m_targetTime.max, command, events);

			if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
			{
				bSatisfied = true;
				break;
			}

			cmdIndex++;
		}

		if(!bSatisfied)
		{
			while(events.size() > 0)
			{
				if(!bRanOutOfTime && events[0].time() > target->m_targetTime.max)
				{
					bRanOutOfTime = true;

					switch(format)
					{
					case eOutputFormatMinimal:
					case eOutputFormatSimple:
					case eOutputFormatDetailed:
					case eOutputFormatFull:
						output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, bSatisfied ? L"satisfied" : L"failed");
						output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
						state.PrintDetails(output);
						output += '\n';
						break;
					}
				}

				if(events[0].time() > m_timeLimit)
					break;

				state.ProgressTime(events[0].time() - state.m_time);

				if(DisplayEvent(format, events[0].event()))
				{
					switch(format)
					{
					case eOutputFormatFull:
						output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
						state.PrintSummary(output);
						output.Append(L" - ");
						break;
					}
					output += wxString::Format(L"(%s)\n", tostring(format, events[0].event()));
				}

				state.ProcessEvent(events);

				if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
				{
					bSatisfied = true;
					break;
				}
			}
		}

		if(!bSatisfied && state.m_time < target->m_targetTime.min && target->satisfiesTarget(state))
		{
			state.ProgressTime(target->m_targetTime.min - state.m_time);
			bSatisfied = true;
		}

		switch(format)
		{
		case eOutputFormatMinimal:
		case eOutputFormatSimple:
		case eOutputFormatDetailed:
		case eOutputFormatFull:
			output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, bSatisfied ? L"satisfied" : L"failed");
			output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
			state.PrintDetails(output);
			output += '\n';
			break;
		}
	}

	switch(format)
	{
	case eOutputFormatYABOT:
		output += wxString::Format(L" [/s]");
		break;
	}
}

template <typename TTarget, typename TState, typename TEvent>
template <typename TOutput>
void CFitnessCalc<TTarget, TState, TEvent>::PrintGame(TOutput &output, const CGASequenceChromosome<ESC2Command> &chromosome, TState &state) const
{
	CVector<ESC2Command> buildOrder = m_initialBuildOrder;
	buildOrder.append(chromosome.GetValue());

	state.SetInitialState();

	size_t targetIndex = 0;
	const TTarget *target = m_targets[targetIndex];

	CPriorityQueue<TEvent> events;
	for(size_t i=0; i < m_customEvents.size(); i++)
		events.add(m_customEvents[i]);

	while(events.size() > 0 && events[0].time() < START_TIME)
	{
		state.ProgressTime(events[0].time() - state.m_time);
		output.ProcessEvent(events[0], *target, state);
		state.ProcessEvent(events);
	}

	state.ProgressTime(START_TIME - state.m_time);

	state.StartMining();

	size_t cmdIndex = 0;
	bool bSatisfied = true, bRanOutOfTime = false;
	for(; targetIndex < m_targets.size() && bSatisfied; targetIndex++)
	{
		target = m_targets[targetIndex];

		bSatisfied = false;

		while(cmdIndex < buildOrder.size())
		{
			ESC2Command command = buildOrder[cmdIndex];

			if(!state.WillHaveRequirements(command))
				break;

			TState::CResourceCost cost;
			state.GetCost(cost, command);

			bool bHaveCost = state.HasResources(cost);
			bool bHaveStateRequirements = state.HasRequirements(command);

			while(!bHaveCost || !bHaveStateRequirements)
			{
				double resourceWaitTime = 0.0;
				bool bWillGetResources = true;

				bool bWaitForEvent = !bHaveStateRequirements;

				if(!bWaitForEvent)
				{
					bWillGetResources = state.GetResourceWaitTime(cost, resourceWaitTime);
					bWaitForEvent = !bWillGetResources || (events.size() > 0 && events[0].time() < state.m_time + resourceWaitTime);
				}

				if(bWaitForEvent)
				{
					if(events.size() == 0)
						break; // Will never get resources / building state requirements
					else
					{
						// Process the event
						if(!bRanOutOfTime && events[0].time() > target->m_targetTime.max)
						{
							bRanOutOfTime = true;

							output.ProcessTargetComplete(false, targetIndex, *target, state);
						}

						if(events[0].time() > m_timeLimit)
							break;

						state.ProgressTime(events[0].time() - state.m_time);

						output.ProcessEvent(events[0], *target, state);

						state.ProcessEvent(events);

						if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
						{
							bSatisfied = true;
							break;
						}

						if(!bHaveStateRequirements)
							bHaveStateRequirements = state.HasRequirements(command);
						if(bHaveStateRequirements && !bHaveCost)
							bHaveCost = state.HasResources(cost);
					}
				}
				else if(!bHaveStateRequirements)
					break; // Will never get the state requirements
				else if(!bWillGetResources)
					break; // Will never get resources
				else
				{
					if(!bRanOutOfTime && state.m_time + resourceWaitTime > target->m_targetTime.max)
					{
						bRanOutOfTime = true;

						output.ProcessTargetComplete(false, targetIndex, *target, state);
					}

					if(state.m_time + resourceWaitTime > m_timeLimit)
						break;
					else
					{
						state.ProgressTime(resourceWaitTime);
						bHaveCost = true;
					}
				}
			}

			if(bSatisfied || !bHaveCost || !bHaveStateRequirements)
				break;

			output.ProcessCommand(command, *target, state);

			state.SpendResources(cost);
			state.ExecuteCommand(target->m_targetTime.max, command, events);

			if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
			{
				bSatisfied = true;
				break;
			}

			cmdIndex++;
		}

		if(!bSatisfied)
		{
			while(events.size() > 0)
			{
				if(!bRanOutOfTime && events[0].time() > target->m_targetTime.max)
				{
					bRanOutOfTime = true;

					output.ProcessTargetComplete(false, targetIndex, *target, state);
				}

				if(events[0].time() > m_timeLimit)
					break;

				state.ProgressTime(events[0].time() - state.m_time);

				output.ProcessEvent(events[0], *target, state);

				state.ProcessEvent(events);

				if(target->m_targetTime.IsWithinRange(state.m_time) && target->satisfiesTarget(state))
				{
					bSatisfied = true;
					break;
				}
			}
		}

		if(!bSatisfied && state.m_time < target->m_targetTime.min && target->satisfiesTarget(state))
		{
			state.ProgressTime(target->m_targetTime.min - state.m_time);
			bSatisfied = true;
		}

		output.ProcessTargetComplete(bSatisfied, targetIndex, *target, state);
	}
}
