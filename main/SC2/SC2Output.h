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
};

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