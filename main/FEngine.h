#pragma once

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>
#include <wx/hashset.h>

#include "bitmaps/Protoss.xpm"

#include "Core/Tokeniser.h"

#include "Factorio/FVersion.h"
#include "Factorio/FState.h"


class FEngine
{
public:
	FEngine(const CFVersion* version);
	~FEngine() { delete m_fitnessCalc; delete m_engine; }

	//WX_DECLARE_HASH_SET_PTR(wxPGProperty*, wxPointerHash, wxPointerEqual, CPropertiesSet);
	WX_DECLARE_HASH_SET(wxPGProperty*, wxPointerHash, wxPointerEqual, CPropertiesSet);
	WX_DECLARE_HASH_SET(wxString, wxStringHash, wxStringEqual, wxHashSetString);

	
	/*void SetAllowWaitCommands(bool enabled = true) { m_allowWaitCommands = enabled; }
	bool GetAllowWaitCommands() const { return m_allowWaitCommands; }
	void SetMinimumCommandDuration(double duration) { m_minimumCommandDuration = duration; }
	double GetMinimumCommandDuration() const { return m_minimumCommandDuration; }*/

	static void BuildMinMaxString(wxString& str, const CDoubleMinMax* minmax);
	static void BuildTimeMinMaxString(wxString& str, const CDoubleMinMax* minmax);
	static void BuildMinMaxString(wxString& str, const CSizeTMinMax* minmax);

	static void BuildMinMaxString(wxString& str, const CSizeTOptionalMinMax* minmax);
	static void BuildMinMaxString(wxString& str, const CDoubleOptionalMinMax* minmax);

	static wxString BuildMinMaxString(const CSizeTMinMax* minmax);

	static bool InterpretMinMaxValue(CDoubleMinMax& minmax, const wxString& value);
	static bool InterpretTimeMinMaxValue(CDoubleMinMax& minmax, const wxString& value);
	static bool InterpretMinMaxValue(CSizeTMinMax& minmax, const wxString& value);
	static bool InterpretMinMaxValue(CBoolMinMax& minmax, int value);

	static bool InterpretMinMaxValue(CSizeTOptionalMinMax& minmax, const wxString& value);
	static bool InterpretMinMaxValue(CDoubleOptionalMinMax& minmax, const wxString& value);
	static bool InterpretMinMaxValue(CBoolOptionalMinMax& minmax, const wxString& value);

	static bool InterpretTimeValue(wxString value, double& time);
	static wxString TimeToString(double time, int nDecimalPlaces = 2);

	void GetInitialBuildOrders(wxArrayString& arrInitialBuildOrders) const;
	bool GetInitialBuildOrder(const wxString& buildOrderName, wxString& buildOrder) const;
	bool SetInitialBuildOrder(const wxString& buildOrder);

	int AddWaypoint(wxPropertyGrid* pgWaypoint, int waypointIndex, CPropertiesSet& setDoubleProperties, CPropertiesSet& setTimeProperties, CPropertiesSet& setSizeTMinMaxProperties, CPropertiesSet& setBoolMinMaxProperties, CPropertiesSet& setBoolProperties);

	void AddTargetProperties(wxPropertyGrid* pgTarget, CPropertiesSet& setDoubleProperties, CPropertiesSet& setTimeProperties, CPropertiesSet& setSizeTMinMaxProperties, CPropertiesSet& setBoolMinMaxProperties, CPropertiesSet& setBoolProperties)
	{
		AddProperties(pgTarget, 0.0, setDoubleProperties, setTimeProperties, setSizeTMinMaxProperties, setBoolMinMaxProperties, setBoolProperties, false);
	}
	void InitialiseResults(wxPropertyGrid* pgResults) const { CFState state(m_version); CPriorityQueue<FEvent> events; state.SetInitialState(events); AddResults(state, pgResults); }

	bool InitialiseFitnessCalc(const CVector<wxPropertyGrid*>& pgWaypoints, const wxPropertyGrid* pgTarget);
	bool Start(const CVector<wxPropertyGrid*>& pgWaypoints, const wxPropertyGrid* pgTarget);

	void Stop();

	void GetCityStats(size_t& population, size_t& evolution, size_t& stagnationCount, unsigned long long& gameCount, double& bestFitness) const;
	void GetVillageStats(size_t villageIndex, size_t& population, size_t& evolution, size_t& stagnationCount, unsigned long long& gameCount, double& bestFitness) const;

	size_t GetStagnationLimit() const { return m_engine->StagnationLimit(); }

	void UpdateBestGame();
	void PrintBestGame(wxString& output, wxPropertyGrid* pgResults) const;
	void DrawBestGame(vector<vector<ChartItem>>& chartItems, wxPropertyGrid* pgResults) const;
	void GetBestGameGridData(vector<GridItem>& data, wxPropertyGrid* pgResults) const;

	static int GetPropertyIndex(const wxString& name);
	static int PropertySortFunction(wxPropertyGrid* propGrid, wxPGProperty* p1, wxPGProperty* p2);

	wxIcon GetIcon() const { return wxIcon(Protoss_xpm); }
	double& GetMaxTargetTime() { return m_maxTime; }

	void SetSeed(const CGASequenceChromosome<const FAction*>& seed) { m_bestGame = seed; }
	void SetSeed(const std::vector<wxString>& seed);

	void SetOutput(FOutput* output) { if (m_output) delete m_output; m_output = output; }

	const wxString& GetGameLongDescription() const { return m_version->GetGameLongDescription(); }
	const wxString& GetMinGameVersion() const { return m_version->GetMinGameVersion(); }

	const CGASequenceChromosome<const FAction*>& GetBestGame() const { return m_bestGame; }

protected:
	void AddTimeResult(wxPropertyGrid* pgResults, wxString name, double time, bool alwaysAdd = false) const;
	void AddIntResult(wxPropertyGrid* pgResults, wxString name, double value, bool alwaysAdd = false) const;
	void AddBoolResult(wxPropertyGrid* pgResults, wxString name, bool value, bool alwaysAdd = false) const;

	bool m_allowWaitCommands;
	double m_minimumCommandDuration;

	WX_DECLARE_STRING_HASH_MAP(wxString, wxStringHashMap);
	wxStringHashMap m_initialBuildOrders;

	void AddProperties(wxPropertyGrid* pgTarget, double timeLimit, CPropertiesSet& setDoubleProperties, CPropertiesSet& setTimeProperties, CPropertiesSet& setSizeTMinMaxProperties, CPropertiesSet& setBoolMinMaxProperties, CPropertiesSet& setBoolProperties, bool isWaypoint);
	void AddResults(const FState& state, wxPropertyGrid* pgResults) const;
	bool BuildWaypoint(const wxPropertyGrid* propertyGrid, FWaypoint& waypoint);

	const FVersion* m_version;
	FOutput* m_output;
	FOutputChart* m_chart_output;
	FOutputGrid* m_grid_output;
	FFitnessCalc* m_fitnessCalc;
	CGAEngine<FNewChromosome, FNewMutator, FFitnessCalc, CFitnessValue, CGAPopulationPartialSortAndRemoveDuplicates>* m_engine;
	CVector<const FAction*> m_alphabet;
	CVector<const FAction*> m_initialBuildOrder;
	FWaitCommand m_commandWaitPoint1Second;
	FWaitCommand m_commandWait1Second;
	FWaitCommand m_commandWait5Seconds;
	CVector<FEvent> m_events;
	FNewMutator m_mutator;
	double m_maxTime;
	CGASequenceChromosome<const FAction*> m_bestGame;
};
