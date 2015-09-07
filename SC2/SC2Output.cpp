#include "stdafx.h"
#include "SC2Output.h"

void CSC2OutputMinimal::ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	if (command->IsAutoCastAbility())
		return;

	if(command->GetPostCommandDuration() > 0.0 && m_lastCommandWaitDuration > 0.0)
	{
		m_lastCommandCount++;
		m_lastCommandWaitDuration += command->GetPostCommandDuration();
		return;
	}
	else if(command == m_lastCommand)
	{
		m_lastCommandCount++;
		return;
	}
	else if(m_lastCommandWaitDuration == 1.0)
		m_output += wxString::Format(L"%3d Wait 1 second\n", m_lastCommandSupplyCount);
	else if(m_lastCommandWaitDuration > 0.0)
		m_output += wxString::Format(L"%3d Wait %g seconds\n", m_lastCommandSupplyCount, m_lastCommandWaitDuration);
	else if(1 < m_lastCommandCount)
		m_output += wxString::Format(L"%3d %d * %s\n", m_lastCommandSupplyCount, m_lastCommandCount, m_lastCommand->GetName());
	else if(1 == m_lastCommandCount)
		m_output += wxString::Format(L"%3d %s\n", m_lastCommandSupplyCount, m_lastCommand->GetName());

	if(command->IsBuildWorkerCommand())
	{
		m_lastCommand = NULL;
		m_lastCommandCount = 0;
		return;
	}

	m_lastCommand = command;
	m_lastCommandCount = 1;
	m_lastCommandSupplyCount = state.m_supply;
	m_lastCommandTime = state.m_time;
	m_lastCommandWaitDuration = command->GetPostCommandDuration();
}

void CSC2OutputMinimal::ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	wxString output;
	switch(event.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eSendScout:
		output = wxString::Format(L"(Scouting worker sent)");
		break;
	case CSC2Event::eKillScout:
		output = wxString::Format(L"(Scouting worker killed)");
		break;
	case CSC2Event::eReturnScout:
		output = wxString::Format(L"(Scouting worker returns)");
		break;
	default:
		return;
	}

	if(0 < m_lastCommandCount)
	{
		if(m_lastCommandWaitDuration == 1.0)
			m_output += wxString::Format(L"%3d Wait 1 second\n", m_lastCommandSupplyCount);
		else if(m_lastCommandWaitDuration > 0.0)
			m_output += wxString::Format(L"%3d Wait %g seconds\n", m_lastCommandSupplyCount, m_lastCommandWaitDuration);
		else if(1 < m_lastCommandCount)
			m_output += wxString::Format(L"%3d %d * %s\n", m_lastCommandSupplyCount, m_lastCommandCount, m_lastCommand->GetName());
		else
			m_output += wxString::Format(L"%3d %s\n", m_lastCommandSupplyCount, m_lastCommand->GetName());

		m_lastCommand = NULL;
		m_lastCommandCount = 0;
	}

	m_output += wxString::Format(wxT("%3d %s\n"), m_lastCommandSupplyCount, output);
}

void CSC2OutputMinimal::ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	if(0 < m_lastCommandCount)
	{
		if(m_lastCommandWaitDuration == 1.0)
			m_output += wxString::Format(L"%3d Wait 1 second\n", m_lastCommandSupplyCount);
		else if(m_lastCommandWaitDuration > 0.0)
			m_output += wxString::Format(L"%3d Wait %g seconds\n", m_lastCommandSupplyCount, m_lastCommandWaitDuration);
		else if(1 < m_lastCommandCount)
			m_output += wxString::Format(L"%3d %d * %s\n", m_lastCommandSupplyCount, m_lastCommandCount, m_lastCommand->GetName());
		else
			m_output += wxString::Format(L"%3d %s\n", m_lastCommandSupplyCount, m_lastCommand->GetName());

		m_lastCommand = NULL;
		m_lastCommandCount = 0;
	}

	m_output += wxString::Format(L"\nWaypoint %d %s:\n", waypointIndex + 1, succeeded ? L"satisfied" : L"failed");
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintDetails(m_output);
	m_output += '\n';
}

void CSC2OutputSimple::ProcessCommand(const CSC2Command *command, const CSC2Waypoint &target, const CSC2State &state)
{
	if (command->IsAutoCastAbility())
		return;

	if(command->GetPostCommandDuration() > 0.0)
	{
		if(m_lastCommandWaitDuration == 0.0)
		{
			m_lastSummary = wxString::Format(wxT("%2d:%05.2f: "), (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
			state.PrintSummary(m_lastSummary);
		}
		m_lastCommandWaitDuration += command->GetPostCommandDuration();
		return;
	}
	else if(0.0 < m_lastCommandWaitDuration)
	{
		if(m_lastCommandWaitDuration == 1.0)
			m_output += wxString::Format(L"%s - Wait 1 second\n", m_lastSummary);
		else
			m_output += wxString::Format(L"%s - Wait %g seconds\n", m_lastSummary, m_lastCommandWaitDuration);

		m_lastCommandWaitDuration = 0.0;
	}

	m_output += wxString::Format(wxT("%2d:%05.2f: "), (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintSummary(m_output);
	m_output.Append(L" - ");
	m_output += wxString::Format(L"%s", command->GetName());
	m_output.Append(L"\n");
}

void CSC2OutputSimple::ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	wxString output;
	switch(event.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eSendScout:
		output = wxString::Format(L"(Scouting worker sent)");
		break;
	case CSC2Event::eKillScout:
		output = wxString::Format(L"(Scouting worker killed)");
		break;
	case CSC2Event::eReturnScout:
		output = wxString::Format(L"(Scouting worker returns)");
		break;
	default:
		return;
	}

	if(0.0 < m_lastCommandWaitDuration)
	{
		if(m_lastCommandWaitDuration == 1.0)
			m_output += wxString::Format(L"%s - Wait 1 second\n", m_lastSummary);
		else
			m_output += wxString::Format(L"%s - Wait %g seconds\n", m_lastSummary, m_lastCommandWaitDuration);

		m_lastCommandWaitDuration = 0.0;
	}

	m_output += wxString::Format(wxT("%2d:%05.2f: "), (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintSummary(m_output);
	m_output.Append(wxT(" - "));
	m_output.Append(output);
	m_output += wxString::Format(wxT("\n"));
}

void CSC2OutputSimple::ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	if(0.0 < m_lastCommandWaitDuration)
	{
		if(m_lastCommandWaitDuration == 1.0)
			m_output += wxString::Format(L"%s - Wait 1 second\n", m_lastSummary);
		else
			m_output += wxString::Format(L"%s - Wait %g seconds\n", m_lastSummary, m_lastCommandWaitDuration);

		m_lastCommandWaitDuration = 0.0;
	}

	m_output += wxString::Format(L"\nWaypoint %d %s:\n", waypointIndex + 1, succeeded ? L"satisfied" : L"failed");
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintDetails(m_output);
	m_output += '\n';
}

void CSC2OutputDetailed::ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintSummary(m_output);
	m_output.Append(L" - ");
	m_output += wxString::Format(L"%s", command->GetName());
	m_output.Append(L"\n");
}

void CSC2OutputDetailed::ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	wxString output;
	switch(event.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eSendScout:
		output = wxString::Format(L"(Scouting worker sent)");
		break;
	case CSC2Event::eKillScout:
		output = wxString::Format(L"(Scouting worker killed)");
		break;
	case CSC2Event::eReturnScout:
		output = wxString::Format(L"(Scouting worker returns)");
		break;
	case CSC2Event::eBuildingComplete:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eUnitComplete:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_units[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eResearchComplete:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_research[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eBuildingConsume:
		output = wxString::Format(L"(%s consumed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eBuildingMorph:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eUnitMorph:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_units[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eBuildingSpawnLarvae:
		output = wxString::Format(L"(Larva spawned)");
		break;
	case CSC2Event::eBuildingSpawnBonusLarvae:
		output = wxString::Format(L"(%d bonus larvae spawned)", event.m_event.m_data.m_data);
		break;
	default:
		return;
	}

	m_output += wxString::Format(wxT("%2d:%05.2f: "), (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintSummary(m_output);
	m_output.Append(wxT(" - "));
	m_output.Append(output);
	m_output += wxString::Format(wxT("\n"));
}

void CSC2OutputDetailed::ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	m_output += wxString::Format(L"\nWaypoint %d %s:\n", waypointIndex + 1, succeeded ? L"satisfied" : L"failed");
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintDetails(m_output);
	m_output += '\n';
}

void CSC2OutputFull::ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintSummary(m_output);
	m_output.Append(L" - ");
	m_output += wxString::Format(L"%s", command->GetName());
	m_output.Append(L"\n");
}

void CSC2OutputFull::ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	wxString output;
	switch(event.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eWorkerStartMiningMinerals:
		output = wxString::Format(L"(Worker starts mining minerals)");
		break;
	case CSC2Event::eWorkerStartMiningGas:
		output = wxString::Format(L"(Worker starts mining gas)");
		break;
	case CSC2Event::eSendScout:
		output = wxString::Format(L"(Scouting worker sent)");
		break;
	case CSC2Event::eKillScout:
		output = wxString::Format(L"(Scouting worker killed)");
		break;
	case CSC2Event::eReturnScout:
		output = wxString::Format(L"(Scouting worker returns)");
		break;
	case CSC2Event::eBuildingComplete:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eUnitComplete:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_units[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eResearchComplete:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_research[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eBuildingConsume:
		output = wxString::Format(L"(%s consumed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eBuildingMorph:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eUnitMorph:
		output = wxString::Format(L"(%s completed)", state.m_raceData.m_units[event.m_event.m_data.m_targetID]->GetName());
		break;
	case CSC2Event::eUnitDies:
		output = wxString::Format(L"(%s expired)", state.m_raceData.m_units[state.m_allUnits[event.m_event.m_data.m_sourceID]->unitTypeID]->GetName());
		break;
	case CSC2Event::eBuildingStatusApply:
		{
			size_t status = event.m_event.m_data.m_data;
			size_t statusIndex = 0;
			while(status > 0)
			{
				if(status & 1)
					output = wxString::Format(L"(%s: %s applied)", state.m_raceData.m_buildings[state.m_allBuildings[event.m_event.m_data.m_sourceID]->buildingTypeID]->GetName(), state.m_raceData.m_buildingStatuses[statusIndex]->GetName());

				status >>= 1;
				statusIndex++;
			}
		}
		break;
	case CSC2Event::eBuildingStatusLapse:
		{
			size_t status = event.m_event.m_data.m_data;
			size_t statusIndex = 0;
			while(status > 0)
			{
				if(status & 1)
					output = wxString::Format(L"(%s: %s lapsed)", state.m_raceData.m_buildings[state.m_allBuildings[event.m_event.m_data.m_sourceID]->buildingTypeID]->GetName(), state.m_raceData.m_buildingStatuses[statusIndex]->GetName());

				status >>= 1;
				statusIndex++;
			}
		}
		break;
	case CSC2Event::eBuildingSpawnLarvae:
		output = wxString::Format(L"(Larva spawned)");
		break;
	case CSC2Event::eBuildingSpawnBonusLarvae:
		output = wxString::Format(L"(%d bonus larvae spawned)", event.m_event.m_data.m_data);
		break;
	default:
		return;
	}

	m_output += wxString::Format(wxT("%2d:%05.2f: "), (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintSummary(m_output);
	m_output.Append(wxT(" - "));
	m_output.Append(output);
	m_output += wxString::Format(wxT("\n"));
}

void CSC2OutputFull::ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state)
{
	m_output += wxString::Format(L"\nWaypoint %d %s:\n", waypointIndex + 1, succeeded ? L"satisfied" : L"failed");
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintDetails(m_output);
	m_output += '\n';
}
