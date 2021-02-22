#pragma once

#include "SC2Race.h"

enum EOutputFormat
{
	eOutputFormatMinimal
	, eOutputFormatSimple
	, eOutputFormatDetailed
	, eOutputFormatFull
	, eOutputFormatHaploid
	, eOutputFormatYABOT
	, eOutputFormatSC2Gears
};

void GetInitialYABOTOutput(ESC2Race race, wxString &output);

template <typename TTarget, typename TState, typename TCommand, typename TEvent>
class COutputFormat
{
public:
	virtual ~COutputFormat() {}

	virtual void ProcessCommand(TCommand &command, const TTarget &target, const TState &state) {}
	virtual void ProcessEvent(const TEvent &event, const TTarget &target, const TState &state) {}
	virtual void ProcessTargetComplete(bool succeeded, size_t targetIndex, const TTarget &target, const TState &state) {}

	virtual void GetOutput(wxString &output) = 0;

protected:
	COutputFormat() {}
};

template <typename TTarget, typename TState, typename TCommand, typename TEvent>
class COutputFormatSimple : public COutputFormat<TTarget, TState, TCommand, TEvent>
{
public:
	COutputFormatSimple() : m_lastCommand((TCommand)0), m_lastCommandCount(0) {}
	~COutputFormatSimple() {}

	void ProcessCommand(TCommand &command, const TTarget &target, const TState &state)
	{
		if(command != m_lastCommand && m_lastCommandCount > 0)
			AddLastCommands();

		if(DisplayCommand(eOutputFormatSimple, command))
		{
			m_lastState = state;
			m_lastCommand = command;
			m_lastCommandCount++;
		}
	}
	
	void ProcessEvent(const TEvent &event, const TTarget &target, const TState &state)
	{
/*		if(DisplayEvent(format, events->GetData().event()))
		{
			switch(format)
			{
			case eOutputFormatFull:
				m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
				state.PrintSummary(output);
				m_output.Append(L" - ");
				break;
			}
			m_output += wxString::Format(L"(%s)\n", tostring(format, events->GetData().event()));
		}
*/	}

	void ProcessTargetComplete(bool succeeded, size_t targetIndex, const TTarget &target, const TState &state)
	{
		if(m_lastCommandCount > 0)
			AddLastCommands();

		m_output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, succeeded ? L"satisfied" : L"failed");
		m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
		state.PrintDetails(m_output);
		m_output += '\n';
	}

	void GetOutput(wxString &output) { output = m_output; }

protected:
	wxString m_output;
	TState m_lastState;
	TCommand m_lastCommand;
	size_t m_lastCommandCount;

	void AddLastCommands()
	{
		if(!m_lastCommand)
			return;

		m_output += wxString::Format(L"%d ", (int)(m_lastState.m_supply));
		m_output += tostring(eOutputFormatSimple, m_lastCommand, m_lastCommandCount);
		m_output.Append('\n');

		m_lastCommand = (TCommand)0;
		m_lastCommandCount = 0;
	}
};

template <typename TTarget, typename TState, typename TCommand, typename TEvent>
class COutputFormatDetailed : public COutputFormat<TTarget, TState, TCommand, TEvent>
{
public:
	COutputFormatDetailed() : {}
	~COutputFormatDetailed() {}

	void ProcessCommand(TCommand &command, const TTarget &target, const TState &state)
	{
		if(DisplayCommand(eOutputFormatDetailed, command))
		{
			m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
			state.PrintSummary(m_output);
			m_output.Append(L" - ");
			m_output += tostring(eOutputFormatDetailed, command);
			m_output.Append('\n');
		}
	}
	
	void ProcessEvent(const TEvent &event, const TTarget &target, const TState &state)
	{
/*		if(DisplayEvent(format, events->GetData().event()))
		{
			switch(format)
			{
			case eOutputFormatFull:
				m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
				state.PrintSummary(output);
				m_output.Append(L" - ");
				break;
			}
			m_output += wxString::Format(L"(%s)\n", tostring(format, events->GetData().event()));
		}
*/	}

	void ProcessTargetComplete(bool succeeded, size_t targetIndex, const TTarget &target, const TState &state)
	{
		m_output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, succeeded ? L"satisfied" : L"failed");
		m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
		state.PrintDetails(m_output);
		m_output += '\n';
	}

	void GetOutput(wxString &output) { output = m_output; }

protected:
	wxString m_output;
};

template <typename TTarget, typename TState, typename TCommand, typename TEvent>
class COutputFormatFull : public COutputFormat<TTarget, TState, TCommand, TEvent>
{
public:
	COutputFormatFull() : {}
	~COutputFormatFull() {}

	void ProcessCommand(TCommand &command, const TTarget &target, const TState &state)
	{
		if(DisplayCommand(eOutputFormatFull, command))
		{
			m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
			state.PrintSummary(m_output);
			m_output.Append(L" - ");
			m_output += tostring(eOutputFormatFull, command);
			m_output.Append('\n');
		}
	}
	
	void ProcessEvent(const TEvent &event, const TTarget &target, const TState &state)
	{
		if(DisplayEvent(format, events->GetData().event()))
		{
			switch(format)
			{
			case eOutputFormatFull:
				m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
				state.PrintSummary(output);
				m_output.Append(L" - ");
				break;
			}
			m_output += wxString::Format(L"(%s)\n", tostring(format, events->GetData().event()));
		}
	}

	void ProcessTargetComplete(bool succeeded, size_t targetIndex, const TTarget &target, const TState &state)
	{
		m_output += wxString::Format(L"\nWaypoint %d %s:\n", targetIndex + 1, succeeded ? L"satisfied" : L"failed");
		m_output += wxString::Format(L"%2d:%05.2f: ", (int)(state.m_time/60) - 60*(int)(state.m_time/3600), state.m_time - 60*(int)(state.m_time/60));
		state.PrintDetails(m_output);
		m_output += '\n';
	}

	void GetOutput(wxString &output) { output = m_output; }

protected:
	wxString m_output;
};
