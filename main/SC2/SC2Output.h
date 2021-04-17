#pragma once

#include "SC2Command.h"
#include "SC2Waypoint.h"
#include "SC2State.h"
#include "SC2Event.h"
#include "ChartItem.h"
#include "GridItem.h"

class CSC2Output
{
public:
	virtual ~CSC2Output() {}

	virtual void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) {}
	virtual void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) {}
	virtual void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state) {}

	virtual void Reset() = 0;

	virtual void GetOutput(wxString &output) const = 0;

protected:
	CSC2Output() {}
};

class CSC2OutputMinimal : public CSC2Output
{
public:
	CSC2OutputMinimal() : m_lastCommand(NULL), m_lastCommandCount(0), m_lastCommandSupplyCount(0), m_lastCommandTime(0.0), m_lastCommandWaitDuration(0.0) {}
	~CSC2OutputMinimal() {}

	void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state) override;

	void Reset() { m_lastCommandCount = 0; m_output.clear(); }

	void GetOutput(wxString &output) const override { output = m_output; }

protected:
	const CSC2Command *m_lastCommand;
	size_t m_lastCommandCount;
	size_t m_lastCommandSupplyCount;
	double m_lastCommandTime;
	double m_lastCommandWaitDuration;
	wxString m_output;
};

class CSC2OutputChart : public CSC2Output
{
public:
	CSC2OutputChart() {}
	~CSC2OutputChart() {}

	void ProcessCommand(const CSC2Command* command, const CSC2Waypoint& waypoint, const CSC2State& state) override;
	void ProcessEvent(const CSC2Event& event, const CSC2Waypoint& waypoint, const CSC2State& state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint& waypoint, const CSC2State& state) override;

	void Reset()
	{
		m_output.clear();
		m_chart_items.clear();
	}

	void GetOutput(wxString& output) const override { output = m_output; }
	void GetChartItems(vector<vector<ChartItem>>& chartItems) const { chartItems = m_chart_items; }

	void AddChartItem(size_t buildingId, ChartItem item);

protected:
	wxString m_output;
	vector<vector<ChartItem>> m_chart_items;
};

class CSC2OutputGrid : public CSC2Output
{
public:
	CSC2OutputGrid() {}
	~CSC2OutputGrid() {}

	void ProcessCommand(const CSC2Command* command, const CSC2Waypoint& waypoint, const CSC2State& state) override;
	void ProcessEvent(const CSC2Event& event, const CSC2Waypoint& waypoint, const CSC2State& state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint& waypoint, const CSC2State& state) override;

	void Reset()
	{
		m_data.clear();
	};

	void GetOutput(wxString& output) const override { output = m_output; }

	void GetData(vector<GridItem>& data) const { data = m_data; }

protected:
	vector<GridItem> m_data;
	wxString m_output;

};