#pragma once

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/hashset.h>

#include "bitmaps/Protoss.xpm"
#include "bitmaps/Terran.xpm"
#include "bitmaps/Zerg.xpm"

#include "Core/Tokeniser.h"

#include "SC2/SC2Version.h"
#include "SC2/SC2RaceInfo.h"
#include "SC2/SC2Command.h"
#include "SC2/SC2State.h"
#include "SC2/SC2Target.h"
#include "SC2/SC2Waypoint.h"
#include "SC2/SC2Race.h"
#include "SC2/SC2FitnessCalc.h"
#include "SC2/GasMicro.h"
#include "SC2/TargetMinMax.h"

class CSC2Engine
{
public:
	CSC2Engine(const CSC2Version *version, ESC2Race race, const CSC2RaceInfo *raceInfo);
	~CSC2Engine() { delete m_fitnessCalc; delete m_engine; }

	//WX_DECLARE_HASH_SET_PTR(wxPGProperty*, wxPointerHash, wxPointerEqual, CPropertiesSet);
	WX_DECLARE_HASH_SET(wxPGProperty*, wxPointerHash, wxPointerEqual, CPropertiesSet);
	WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, wxHashSetString);

	wxString GetRaceString() const { return tostring(GetRace()); }

	enum EScoutingWorker { eNoScout, eScoutStays, eScoutReturns, eScoutDies };

	void SetGasMicro(EGasMicro gasMicro) { m_gasMicro = gasMicro; }
	EGasMicro GetGasMicro() const { return m_gasMicro; }
	void SetAllowWaitCommands(bool enabled = true) { m_allowWaitCommands = enabled; }
	bool GetAllowWaitCommands() const { return m_allowWaitCommands; }
	void SetMinimumCommandDuration(double duration) { m_minimumCommandDuration = duration; }
	double GetMinimumCommandDuration() const { return m_minimumCommandDuration; }

	static void BuildMinMaxString(wxString &str, const CDoubleMinMax *minmax);
	static void BuildTimeMinMaxString(wxString &str, const CDoubleMinMax *minmax);
	static void BuildMinMaxString(wxString &str, const CSizeTMinMax *minmax);

	static void BuildMinMaxString(wxString &str, const CSizeTOptionalMinMax *minmax);
	static void BuildMinMaxString(wxString &str, const CDoubleOptionalMinMax *minmax);
	
	static wxString BuildMinMaxString(const CSizeTMinMax *minmax);

	static bool InterpretMinMaxValue(CDoubleMinMax &minmax, const wxString &value);
	static bool InterpretTimeMinMaxValue(CDoubleMinMax &minmax, const wxString &value);
	static bool InterpretMinMaxValue(CSizeTMinMax &minmax, const wxString &value);
	static bool InterpretMinMaxValue(CBoolMinMax &minmax, int value);

	static bool InterpretMinMaxValue(CSizeTOptionalMinMax &minmax, const wxString &value);
	static bool InterpretMinMaxValue(CDoubleOptionalMinMax &minmax, const wxString &value);
	static bool InterpretMinMaxValue(CBoolOptionalMinMax &minmax, const wxString &value);

	static bool InterpretTimeValue(wxString value, double &time);
	static wxString TimeToString(double time, int nDecimalPlaces = 2);

	void GetInitialBuildOrders(wxArrayString &arrInitialBuildOrders) const;
	bool GetInitialBuildOrder(const wxString &buildOrderName, wxString &buildOrder) const;
	bool SetInitialBuildOrder(const wxString &buildOrder);

	int AddWaypoint(wxPropertyGrid *pgWaypoint, int waypointIndex, CPropertiesSet &setDoubleProperties, CPropertiesSet &setTimeProperties, CPropertiesSet &setSizeTMinMaxProperties, CPropertiesSet &setBoolMinMaxProperties, CPropertiesSet &setBoolProperties);

	void AddTargetProperties(wxPropertyGrid *pgTarget, CPropertiesSet &setDoubleProperties, CPropertiesSet &setTimeProperties, CPropertiesSet &setSizeTMinMaxProperties, CPropertiesSet &setBoolMinMaxProperties, CPropertiesSet &setBoolProperties)
	{
		AddProperties(pgTarget, 0.0, setDoubleProperties, setTimeProperties, setSizeTMinMaxProperties, setBoolMinMaxProperties, setBoolProperties, false);
	}
	void InitialiseResults(wxPropertyGrid *pgResults) const { CSC2State state(m_race, m_raceInfo->GetData()); CPriorityQueue<CSC2Event> events; state.SetInitialState(events); AddResults(state, pgResults); }

	bool InitialiseFitnessCalc(const CVector<wxPropertyGrid *> &pgWaypoints, const wxPropertyGrid *pgTarget, EScoutingWorker scout /* = eNoScout */, int scoutTime /* = 0 */, int scoutEndTime /* = 0 */);
	bool Start(const CVector<wxPropertyGrid *> &pgWaypoints, const wxPropertyGrid *pgTarget, EScoutingWorker scout = eNoScout, int scoutTime = 0, int scoutEndTime = 0);

	void Stop();

	void GetCityStats(size_t &population, size_t &evolution, size_t &stagnationCount, unsigned long long &gameCount, double &bestFitness) const;
	void GetVillageStats(size_t villageIndex, size_t &population, size_t &evolution, size_t &stagnationCount, unsigned long long &gameCount, double &bestFitness) const;

	size_t GetStagnationLimit() const { return m_engine->StagnationLimit(); }

	void UpdateBestGame();
	void PrintBestGame(wxString &output, wxPropertyGrid *pgResults) const;

	static int GetPropertyIndex(const wxString &name);
	static int PropertySortFunction(wxPropertyGrid* propGrid, wxPGProperty* p1, wxPGProperty* p2);

	ESC2Race GetRace() const { return m_race; }
	const CSC2RaceInfo *GetRaceInfo() const { return m_raceInfo; }
	wxIcon GetIcon() const { return wxIcon(Protoss_xpm); }
	double &GetMaxTargetTime() { return m_maxTime; }

	void SetSeed(const CGASequenceChromosome<const CSC2Command *> &seed) { m_bestGame = seed; }
	void SetSeed(const std::vector<wxString> &seed);

	void SetOutput(CSC2Output *output) { if(m_output) delete m_output; m_output = output; }

	const wxString &GetGameLongDescription() const { return m_version->GetGameLongDescription(); }
	const wxString &GetMinGameVersion() const { return m_version->GetMinGameVersion(); }

	const CGASequenceChromosome<const CSC2Command *> &GetBestGame() const { return m_bestGame; }

protected:
	void AddTimeResult(wxPropertyGrid *pgResults, wxString name, double time, bool alwaysAdd = false) const;
	void AddIntResult(wxPropertyGrid *pgResults, wxString name, double value, bool alwaysAdd = false) const;
	void AddBoolResult(wxPropertyGrid *pgResults, wxString name, bool value, bool alwaysAdd = false) const;

	EGasMicro m_gasMicro;
	bool m_allowWaitCommands;
	double m_minimumCommandDuration;

	WX_DECLARE_STRING_HASH_MAP(wxString, wxStringHashMap);
	wxStringHashMap m_initialBuildOrders;

	void AddProperties(wxPropertyGrid *pgTarget, double timeLimit, CPropertiesSet &setDoubleProperties, CPropertiesSet &setTimeProperties, CPropertiesSet &setSizeTMinMaxProperties, CPropertiesSet &setBoolMinMaxProperties, CPropertiesSet &setBoolProperties, bool isWaypoint);
	void AddResults(const CSC2State &state, wxPropertyGrid *pgResults) const;
	bool BuildWaypoint(const wxPropertyGrid *propertyGrid, CSC2Waypoint &waypoint);

	const CSC2Version *m_version;
	CSC2Output *m_output;
	const CSC2RaceInfo *m_raceInfo;
	ESC2Race m_race;
	CSC2FitnessCalc *m_fitnessCalc;
	CGAEngine<CSC2NewChromosome, CSC2NewMutator, CSC2FitnessCalc, CFitnessValue, CGAPopulationPartialSortAndRemoveDuplicates> *m_engine;
	CVector<const CSC2Command *> m_alphabet;
	CVector<const CSC2Command *> m_initialBuildOrder;
	CSC2WaitCommand m_commandWaitPoint1Second;
	CSC2WaitCommand m_commandWait1Second;
	CSC2WaitCommand m_commandWait5Seconds;
	CVector<CSC2Event> m_events;
	CSC2NewMutator m_mutator;
	double m_maxTime;
	CGASequenceChromosome<const CSC2Command *> m_bestGame;
};
