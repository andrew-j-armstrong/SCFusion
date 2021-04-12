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

	if (waypoint.IsFinalTarget()) m_output += wxString::Format(L"\nTarget %s:\n", succeeded ? L"reached" : L"failed");
	else m_output += wxString::Format(L"\nMilestone %d %s:\n", waypointIndex + 1, succeeded ? L"reached" : L"failed");
	m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
	state.PrintDetails(m_output);
	m_output += '\n';
}

void CSC2OutputVisual::ProcessCommand(const CSC2Command* command, const CSC2Waypoint& waypoint, const CSC2State& state)
{
}

void CSC2OutputVisual::AddVisualItem(size_t buildingId, VisualItem item)
{
	for (size_t i = m_visual_items.size(); i <= buildingId; i++)
	{
		vector<VisualItem> row;
		m_visual_items.push_back(row);
	}
	m_visual_items[buildingId].push_back(item);
}

void CSC2OutputVisual::ProcessEvent(const CSC2Event& event, const CSC2Waypoint& waypoint, const CSC2State& state)
{
	const double startTime = event.m_event.m_data.m_startTime;
	const double endTime = event.m_time;
	CSC2Building* building;
	CSC2Unit* unit;
	VisualItem::VisualItemType itemType;

	switch (event.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eBuildingComplete:
		building = state.m_raceData.m_buildings[event.m_event.m_data.m_targetID];
		itemType = VisualItem::tMilitary;
		if (building->IsBase()) itemType = VisualItem::tBase;
		else if (building->IsGeyserBuilding()) itemType = VisualItem::tGas;
		else if (building->GetProvidedSupply() > 0) itemType = VisualItem::tSupply;

		AddVisualItem(state.m_allBuildings.size(), VisualItem(building->GetName(), startTime, endTime, itemType));
		break;
	case CSC2Event::eUnitComplete:
		unit = state.m_raceData.m_units[event.m_event.m_data.m_targetID];
		itemType = VisualItem::tMilitaryUnit;
		if (unit->IsWorker() || unit->GetMineralIncomeRate() > 0) itemType = VisualItem::tWorker;
		else if (unit->GetProvidedSupply() > 0) itemType = VisualItem::tSupply;

		AddVisualItem(
			event.m_event.m_data.m_sourceIsBuilding ? event.m_event.m_data.m_sourceID : 0,
			VisualItem(
				unit->GetName(),
				startTime,
				endTime,
				itemType,
				event.m_event.m_data.m_queueType
			)
		);
		break;
	case CSC2Event::eResearchComplete:
		AddVisualItem(
			event.m_event.m_data.m_sourceID,
			VisualItem(
				state.m_raceData.m_research[event.m_event.m_data.m_targetID]->GetName(),
				startTime,
				endTime,
				VisualItem::tResearch,
				event.m_event.m_data.m_queueType
			)
		);
		break;
	case CSC2Event::eBuildingMorph:
		building = state.m_raceData.m_buildings[event.m_event.m_data.m_targetID];
		itemType = VisualItem::tMilitary;
		if (building->IsBase()) itemType = VisualItem::tBase;
		else if (building->IsGeyserBuilding()) itemType = VisualItem::tGas;
		else if (building->GetProvidedSupply() > 0) itemType = VisualItem::tSupply;

		AddVisualItem(event.m_event.m_data.m_sourceID, VisualItem(building->GetName(), startTime, endTime, itemType));
		break;
	case CSC2Event::eUnitMorph:
		unit = state.m_raceData.m_units[event.m_event.m_data.m_targetID];
		itemType = VisualItem::tMilitaryUnit;
		if (unit->IsWorker()) itemType = VisualItem::tWorker;

		AddVisualItem(0, VisualItem(unit->GetName(), startTime, endTime, itemType));
		break;
	case CSC2Event::eBuildingStatusLapse:
		{
			size_t status = event.m_event.m_data.m_data;
			size_t statusIndex = 0;
			while (status > 0)
			{
				if ((status & 1) && state.m_raceData.m_buildingStatuses[statusIndex]->IsVisual())
					AddVisualItem(event.m_event.m_data.m_sourceID, VisualItem(state.m_raceData.m_buildingStatuses[statusIndex]->GetName(), startTime, endTime, VisualItem::tStatus));

				status >>= 1;
				statusIndex++;
			}
		}
		break;
	default:
		return;
	}
}

void CSC2OutputVisual::ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint& waypoint, const CSC2State& state)
{
	AddVisualItem(0, VisualItem("", 0, state.m_time, VisualItem::tMilestone));
}


void CSC2OutputGrid::ProcessCommand(const CSC2Command* command, const CSC2Waypoint& waypoint, const CSC2State& state)
{
	
	GridItem::GridItemType itemType = command->IsApplyVisualStatusCommand() ? GridItem::tStatus : GridItem::tDefault;

	if (command->IsBuildWorkerCommand()) itemType = GridItem::tWorker;
	else if (command->WillBuildUnit() && state.m_raceData.m_units[command->GetBuildUnitTypeID()]->GetMineralIncomeRate() > 0) itemType = GridItem::tWorker;
	else if (command->WillSpawnBase()) itemType = GridItem::tBase;
	else if (command->WillBuildBuilding() && state.m_raceData.m_buildings[command->GetBuildBuildingTypeID()]->IsBase()) itemType = GridItem::tBase;
	else if (command->GetProvidedSupply() > 0) itemType = GridItem::tSupply;
	else if (command->WillBuildGeyserBuilding()) itemType = GridItem::tGas;
	else if (command->WillBuildBuilding() && state.m_raceData.m_buildings[command->GetBuildBuildingTypeID()]->IsVisual()) itemType = GridItem::tMilitary;
	else if (command->WillBuildUnit()) itemType = GridItem::tMilitaryUnit;

	GridItem gridItem = GridItem(command->GetName(), state.m_time, itemType, command->IsAutoCastAbility() ? GridItem::lFull : GridItem::lSimple);
	state.FillData(gridItem);

	m_data.push_back(gridItem);
}

void CSC2OutputGrid::ProcessEvent(const CSC2Event& event, const CSC2Waypoint& waypoint, const CSC2State& state)
{
	GridItem gridItem = GridItem(wxString::Format(L"(unknown event)", event.m_event.m_data.m_data), state.m_time);
	CSC2Building* building;
	CSC2Unit* unit;

	switch (event.m_event.m_data.m_eventCategory)
	{
	case CSC2Event::eWorkerStartMiningMinerals:
		gridItem.name = wxString::Format(L"(Worker starts mining minerals)");
		break;
	case CSC2Event::eWorkerStartMiningGas:
		gridItem.name = wxString::Format(L"(Worker starts mining gas)");
		break;
	case CSC2Event::eSendScout:
		gridItem.name = wxString::Format(L"(Scouting worker sent)");
		gridItem.level = GridItem::lSimple;
		break;
	case CSC2Event::eKillScout:
		gridItem.name = wxString::Format(L"(Scouting worker killed)");
		gridItem.level = GridItem::lSimple;
		break;
	case CSC2Event::eReturnScout:
		gridItem.name = wxString::Format(L"(Scouting worker returns)");
		gridItem.level = GridItem::lSimple;
		break;
	case CSC2Event::eBuildingComplete:
	case CSC2Event::eBuildingMorph:
		building = state.m_raceData.m_buildings[event.m_event.m_data.m_targetID];
		gridItem.name = wxString::Format(L"(%s completed)", building->GetName());
		gridItem.level = GridItem::lDetailed;
		gridItem.itemType = GridItem::tMilitary;
		if (building->IsBase()) gridItem.itemType = GridItem::tBase;
		else if (building->IsGeyserBuilding()) gridItem.itemType = GridItem::tGas;
		else if (building->GetProvidedSupply() > 0) gridItem.itemType = GridItem::tSupply;
		break;
	case CSC2Event::eUnitComplete:
	case CSC2Event::eUnitMorph:
		unit = state.m_raceData.m_units[event.m_event.m_data.m_targetID];
		gridItem.name = wxString::Format(L"(%s completed)", unit->GetName());
		gridItem.level = GridItem::lDetailed;
		gridItem.itemType = GridItem::tMilitaryUnit;
		if (unit->IsWorker() || unit->GetMineralIncomeRate() > 0) gridItem.itemType = GridItem::tWorker;
		else if (unit->GetProvidedSupply() > 0) gridItem.itemType = GridItem::tSupply;
		break;
	case CSC2Event::eResearchComplete:
		gridItem.name = wxString::Format(L"(%s completed)", state.m_raceData.m_research[event.m_event.m_data.m_targetID]->GetName());
		gridItem.itemType = GridItem::tResearch;
		gridItem.level = GridItem::lDetailed;
		break;
	case CSC2Event::eBuildingConsume:
		gridItem.name = wxString::Format(L"(%s consumed)", state.m_raceData.m_buildings[event.m_event.m_data.m_targetID]->GetName());
		gridItem.level = GridItem::lDetailed;
		break;
	case CSC2Event::eUnitDies:
		gridItem.name = wxString::Format(L"(%s expired)", state.m_raceData.m_units[state.m_allUnits[event.m_event.m_data.m_sourceID]->unitTypeID]->GetName());
		break;
	case CSC2Event::eBuildingStatusApply:
	{
		size_t status = event.m_event.m_data.m_data;
		size_t statusIndex = 0;
		while (status > 0)
		{
			if (status & 1)
				gridItem.name = wxString::Format(L"(%s: %s applied)", state.m_raceData.m_buildings[state.m_allBuildings[event.m_event.m_data.m_sourceID]->buildingTypeID]->GetName(), state.m_raceData.m_buildingStatuses[statusIndex]->GetName());

			status >>= 1;
			statusIndex++;
		}
		gridItem.itemType = GridItem::tStatus;
	}
	break;
	case CSC2Event::eBuildingStatusLapse:
	{
		size_t status = event.m_event.m_data.m_data;
		size_t statusIndex = 0;
		gridItem.itemType = GridItem::tDefault;
		while (status > 0)
		{
			if (status & 1)
			{
				gridItem.name = wxString::Format(L"(%s: %s lapsed)", state.m_raceData.m_buildings[state.m_allBuildings[event.m_event.m_data.m_sourceID]->buildingTypeID]->GetName(), state.m_raceData.m_buildingStatuses[statusIndex]->GetName());
				if (state.m_raceData.m_buildingStatuses[statusIndex]->IsVisual())
					gridItem.itemType = GridItem::tStatus;
			}
			status >>= 1;
			statusIndex++;
		}
	}
	break;
	case CSC2Event::eBuildingSpawnLarvae:
		gridItem.name = wxString::Format(L"(Larva spawned)");
		gridItem.level = GridItem::lDetailed;
		break;
	case CSC2Event::eBuildingSpawnBonusLarvae:
		gridItem.name = wxString::Format(L"(%d bonus larvae spawned)", event.m_event.m_data.m_data);
		gridItem.level = GridItem::lDetailed;
		break;
	default:
		return;
	}

	state.FillData(gridItem);

	m_data.push_back(gridItem);
}

void CSC2OutputGrid::ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint& waypoint, const CSC2State& state)
{
	wxString itemName;
	if (waypoint.IsFinalTarget()) itemName = wxString::Format(L"Target %s", succeeded ? L"reached" : L"failed");
	else itemName = wxString::Format(L"Milestone %d %s", waypointIndex + 1, succeeded ? L"reached" : L"failed");

	GridItem gridItem = GridItem(itemName, state.m_time, GridItem::tMilestone, GridItem::lSimple);
	state.FillMilestoneData(gridItem);

	m_data.push_back(gridItem);
}