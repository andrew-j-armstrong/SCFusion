#pragma once

#include "SC2Command.h"
#include "SC2Waypoint.h"
#include "SC2State.h"
#include "SC2Event.h"

class CSC2Output
{
public:
	virtual ~CSC2Output() {}

	virtual void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) {}
	virtual void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) {}
	virtual void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, size_t numberOfWaypoints, const CSC2Waypoint &waypoint, const CSC2State &state) {}

	virtual void Reset() = 0;

	virtual void GetOutput(wxString &output) const = 0;

protected:
	CSC2Output() {}
};

class CSC2OutputCustom : public CSC2Output
{
public:
	CSC2OutputCustom(const CSC2RaceInfo* raceinfo, bool time, bool ressources, bool supplycap, bool workers, bool chronos,
		             bool workerstransfert, bool scoutingworker, bool workerscompletion,
					 bool unitscompletion, bool buildingscompletion, bool techcompletion,
					 bool spells, bool spellscompletion, bool others, bool waypoints, bool larvae) :
	                 m_lastCommand(NULL), m_lastCommandCount(0), m_lastCommandSupplyCount(0), m_lastCommandTime(0.0), m_lastCommandWaitDuration(0.0),
		             m_lastCommandMinerals(0.0), m_lastCommandGas(0.0), m_lastCommandTotalLarvaeCount(0), m_lastCommandChronoboostsAvailable(0), m_lastCommandSupplyCap(0),
					 m_raceInfo(raceinfo), m_time(time), m_ressources(ressources), m_supplyCap(supplycap), m_workers(workers), m_chronoboostsAvailable(chronos),
					 m_workersTransfert(workerstransfert), m_scoutingWorker(scoutingworker), m_workersCompletion(workerscompletion),
					 m_unitsCompletion(unitscompletion), m_buildingsCompletion(buildingscompletion), m_techCompletion(techcompletion),
					 m_spells(spells), m_spellsCompletion(spellscompletion), m_others(others), m_waypoints(waypoints), m_larvae(larvae){}
	~CSC2OutputCustom() {}

	void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state);
	void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state);
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, size_t numberOfWaypoints, const CSC2Waypoint &waypoint, const CSC2State &state);

	void Reset() { m_lastCommandCount = 0; m_output.clear(); }

	void GetOutput(wxString &output) const { output = m_output; }

	void PrintSummary(const CSC2State &state);
	void PrintLastSummary(const CSC2State &state);

	//void SetTime(const bool val) { m_time = val; } 

protected:
	const CSC2Command *m_lastCommand;
	size_t m_lastCommandCount;
	size_t m_lastCommandSupplyCount;
	double m_lastCommandTime;
	double m_lastCommandWaitDuration;
	double m_lastCommandMinerals;
	double m_lastCommandGas;
	size_t m_lastCommandTotalLarvaeCount;
	size_t m_lastCommandChronoboostsAvailable;
	size_t m_lastCommandSupplyCap;
	wxString m_output;
	wxString m_lastSummary;
	bool m_time;
	bool m_ressources;
	bool m_supplyCap;
	bool m_workers;
	bool m_chronoboostsAvailable;
	bool m_workersTransfert;
	bool m_scoutingWorker;
	bool m_workersCompletion;
	bool m_unitsCompletion;
	bool m_buildingsCompletion;
	bool m_techCompletion;
	bool m_spells;
	bool m_spellsCompletion;
	bool m_others;
	bool m_waypoints;
	bool m_larvae;
	const CSC2RaceInfo* m_raceInfo;
};

/*class CSC2OutputMinimal : public CSC2Output
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

class CSC2OutputSimple : public CSC2Output
{
public:
	CSC2OutputSimple() : m_lastCommandWaitDuration(0.0) {}
	~CSC2OutputSimple() {}

	void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state) override;

	void Reset() { m_output.clear(); m_lastCommandWaitDuration = 0.0; }

	void GetOutput(wxString &output) const override { output = m_output; }

protected:
	wxString m_output;
	wxString m_lastSummary;
	double m_lastCommandWaitDuration;
};

class CSC2OutputDetailed : public CSC2Output
{
public:
	CSC2OutputDetailed() {}
	~CSC2OutputDetailed() {}

	void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state) override;

	void Reset() { m_output.clear(); }

	void GetOutput(wxString &output) const override { output = m_output; }

protected:
	wxString m_output;
};

class CSC2OutputFull : public CSC2Output
{
public:
	CSC2OutputFull() {}
	~CSC2OutputFull() {}

	void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state) override;

	void Reset() { m_output.clear(); }

	void GetOutput(wxString &output) const override { output = m_output; }

protected:
	wxString m_output;
};*/

/*
class CSC2OutputCustom : public CSC2Output
{
public:
	CSC2OutputCustom() {}
	~CSC2OutputCustom() {}

	void ProcessCommand(const CSC2Command *command, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessEvent(const CSC2Event &event, const CSC2Waypoint &waypoint, const CSC2State &state) override;
	void ProcessWaypointComplete(bool succeeded, size_t waypointIndex, const CSC2Waypoint &waypoint, const CSC2State &state) override;

	void GetOutput(wxString &output) const override { output = m_output; }

	class CFormat
	{
	public:
		CFormat();
		~CFormat();

	protected:
		CVector<bool> m_bulkUpCommand;
	};

protected:
	wxString m_output;
	CSC2State m_lastState;
	CSC2Command *m_lastCommand;
	size_t m_lastCommandCount;
};
*/