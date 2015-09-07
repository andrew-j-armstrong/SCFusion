#pragma once

#include <wx/xml/xml.h>

#include "SC2Defines.h"
#include "SC2State.h"
#include "SC2Command.h"
#include "../Core/Vector.h"

class CSC2Target
{
public:
	CSC2Target() : m_displayInResults(true) {}
	virtual ~CSC2Target() {}

	virtual bool LoadXML(const wxXmlNode *xmlTarget) = 0;
	virtual bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands) = 0;

	const wxString &GetName() const { return m_name; }
	virtual wxString GetCategory() const = 0;

	enum ERestrictionType
	{
		eSizeTMinMax
		, eSizeTMinMaxAndUnderConstruction
		, eBooleanMinMaxAndUnderConstruction
		, eBoolean
	};
	virtual ERestrictionType GetRestrictionType() const = 0;
	virtual bool DisplayInResults() const { return m_displayInResults; }

	virtual size_t GetStateCount(const CSC2State &state) const { return 0; }
	virtual size_t GetStateCountUnderConstruction(const CSC2State &state) const { return 0; }
	virtual double GetStateDurationPenalty(const CSC2State &state, double time) const { return 0.0; }

	virtual SC2BuildingFlags GetBuildingRequirementFlags() const = 0;
	virtual SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const = 0;
	virtual SC2UnitFlags GetUnitRequirementFlags() const = 0;
	virtual SC2ResearchFlags GetResearchRequirementFlags() const = 0;

protected:
	wxString m_name;
	bool m_displayInResults;
};

class CSC2TargetBuildingCount : public CSC2Target
{
public:
	CSC2TargetBuildingCount();
	~CSC2TargetBuildingCount() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Buildings"); }
	ERestrictionType GetRestrictionType() const { return eSizeTMinMaxAndUnderConstruction; }

	size_t GetStateCount(const CSC2State &state) const;
	size_t GetStateCountUnderConstruction(const CSC2State &state) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return ((SC2BuildingFlags)1 << m_buildingIDs[0]); }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const { return 0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return 0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return 0; }

protected:
	std::vector<wxString> m_buildingNames;
	CVector<size_t> m_buildingIDs;
};

class CSC2TargetBuildingAbility : public CSC2Target
{
public:
	CSC2TargetBuildingAbility();
	~CSC2TargetBuildingAbility() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Building Abilities"); }
	ERestrictionType GetRestrictionType() const { return eSizeTMinMaxAndUnderConstruction; }

	size_t GetStateCount(const CSC2State &state) const;
	size_t GetStateCountUnderConstruction(const CSC2State &state) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return ((SC2BuildingFlags)1 << m_sourceBuildingID); }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const { return 0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return 0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return 0; }

protected:
	wxString m_sourceBuildingName;
	size_t m_sourceBuildingID;
	double m_requiredEnergy;
};

class CSC2TargetBuildingStatus : public CSC2Target
{
public:
	CSC2TargetBuildingStatus();
	~CSC2TargetBuildingStatus() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Building Status"); }
	ERestrictionType GetRestrictionType() const { return eSizeTMinMax; }

	size_t GetStateCount(const CSC2State &state) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return ((SC2BuildingFlags)1 << m_buildingIDs[0]); }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const;
	SC2UnitFlags GetUnitRequirementFlags() const { return 0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return 0; }

protected:
	std::vector<wxString> m_buildingNames;
	CVector<size_t> m_buildingIDs;
	std::vector<wxString> m_buildingStatusNames;
	CVector<SC2BuildingStatusFlags> m_buildingStatusFlags;
};

class CSC2TargetBuildingStatusDuration : public CSC2Target
{
public:
	CSC2TargetBuildingStatusDuration();
	~CSC2TargetBuildingStatusDuration() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Building Status"); }
	ERestrictionType GetRestrictionType() const { return eBoolean; }

	double GetStateDurationPenalty(const CSC2State &state, double time) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return ((SC2BuildingFlags)1 << m_buildingIDs[0]); }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const { return 0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return 0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return 0; }

protected:
	std::vector<wxString> m_buildingNames;
	CVector<size_t> m_buildingIDs;
	std::vector<wxString> m_buildingStatusNames;
	CVector<SC2BuildingStatusFlags> m_buildingStatusFlags;
};

class CSC2TargetUnitCount : public CSC2Target
{
public:
	CSC2TargetUnitCount();
	~CSC2TargetUnitCount() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Units"); }
	ERestrictionType GetRestrictionType() const { return eSizeTMinMaxAndUnderConstruction; }

	size_t GetStateCount(const CSC2State &state) const;
	size_t GetStateCountUnderConstruction(const CSC2State &state) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return 0; }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const { return 0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return m_unitIDs.size() > 0 ? ((SC2UnitFlags)1 << m_unitIDs[0]) : 0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return 0; }

protected:
	std::vector<wxString> m_unitNames;
	CVector<size_t> m_unitIDs;
	bool m_addTotalLarvae;
};

class CSC2TargetUnitAbility : public CSC2Target
{
public:
	CSC2TargetUnitAbility();
	~CSC2TargetUnitAbility() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Unit Abilities"); }
	ERestrictionType GetRestrictionType() const { return eSizeTMinMaxAndUnderConstruction; }

	size_t GetStateCount(const CSC2State &state) const;
	size_t GetStateCountUnderConstruction(const CSC2State &state) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return 0; }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const { return 0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return m_requiredUnitFlags; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return 0; }

protected:
	std::vector<wxString> m_sourceUnitNames;
	CVector<size_t> m_sourceUnitIDs;
	SC2UnitFlags m_requiredUnitFlags;
	CVector<double> m_requiredEnergy;
};

class CSC2TargetResearch : public CSC2Target
{
public:
	CSC2TargetResearch();
	~CSC2TargetResearch() {}

	bool LoadXML(const wxXmlNode *xmlTarget);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	wxString GetCategory() const { return wxT("Research"); }
	ERestrictionType GetRestrictionType() const { return eBooleanMinMaxAndUnderConstruction; }

	size_t GetStateCount(const CSC2State &state) const;
	size_t GetStateCountUnderConstruction(const CSC2State &state) const;

	SC2BuildingFlags GetBuildingRequirementFlags() const { return 0; }
	SC2BuildingStatusFlags GetBuildingStatusRequirementFlags(size_t buildingID) const { return 0; }
	SC2UnitFlags GetUnitRequirementFlags() const { return 0; }
	SC2ResearchFlags GetResearchRequirementFlags() const { return ((SC2ResearchFlags)1 << m_researchIDs[0]); }

protected:
	std::vector<wxString> m_researchNames;
	CVector<size_t> m_researchIDs;
	SC2ResearchFlags m_flags;
};
