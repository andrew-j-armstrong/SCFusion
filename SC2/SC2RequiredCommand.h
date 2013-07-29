#pragma once

#include "SC2Command.h"
#include "SC2State.h"
#include "TargetMinMax.h"

class CSC2RequiredCommand
{
public:
	CSC2RequiredCommand();
	~CSC2RequiredCommand();

	bool LoadXML(const wxXmlNode *xmlCommand);
	bool ResolveIDs(const CSC2RaceData &raceData, const CVector<const CSC2Command *> &commands);

	bool MeetsRequirements(const CSC2State &state) const;
	const CSC2Command *GetCommand() const { return m_command; }

protected:
	std::vector<wxString> m_buildingRequirementNames;
	SC2BuildingFlags m_buildingRequirements;
	std::vector<wxString> m_unitRequirementNames;
	SC2UnitFlags m_unitRequirements;
	std::vector<wxString> m_researchRequirementNames;
	SC2ResearchFlags m_researchRequirements;

	std::vector<wxString> m_buildingNames;
	CVector<size_t> m_buildingIDs;
	CVector<CTargetOptionalMinMax<size_t>> m_buildingRange;
	std::vector<wxString> m_unitNames;
	CVector<size_t> m_unitIDs;
	CVector<CTargetOptionalMinMax<size_t>> m_unitRange;

	wxString m_commandName;
	const CSC2Command *m_command;
};
