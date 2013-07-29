#pragma once

#include <wx/xml/xml.h>

#include "../Core/Vector.h"

#include "SC2Defines.h"
#include "SC2Race.h"
#include "SC2RaceData.h"
#include "SC2Command.h"
#include "SC2Target.h"
#include "SC2RequiredCommand.h"

class CSC2RaceInfo
{
public:
	CSC2RaceInfo();
	~CSC2RaceInfo();

	bool LoadXML(const wxXmlNode *xmlRace);

	const wxString &GetName() const { return m_name; }
	const CSC2RaceData &GetData() const { return m_data; }
	const CVector<const CSC2Command *> &GetCommands() const { return m_allCommands; }

	const CVector<CVector<const CSC2Command *> *> &GetBuildingPrerequisitCommands() const { return m_buildingPrerequisitCommands; }
	const CVector<CVector<CVector<const CSC2Command *> *> *> &GetBuildingStatusPrerequisitCommands() const { return m_buildingStatusPrerequisitCommands; }
	const CVector<CVector<const CSC2Command *> *> &GetUnitPrerequisitCommands() const { return m_unitPrerequisitCommands; }
	const CVector<CVector<const CSC2Command *> *> &GetResearchPrerequisitCommands() const { return m_researchPrerequisitCommands; }
	const CVector<const CSC2Command *> &GetBuildGeyserBuildingCommands() const { return m_buildGeyserBuildingCommands; }
	const CVector<const CSC2Command *> &GetMoveWorkersToMineralsCommands() const { return m_moveWorkersToMineralsCommands; }
	const CVector<const CSC2Command *> &GetMoveWorkersToGasCommands() const { return m_moveWorkersToGasCommands; }
	const CVector<const CSC2Command *> &GetProvideSupplyCommands() const { return m_provideSupplyCommands; }

	const CVector<const CSC2Command *> &GetMacroAbilityCommands() const { return m_macroAbilityCommands; }

	const CVector<CSC2Target *> &GetTargets() const { return m_targets; }

	const CSC2Command *GetRequiredCommand(EGasMicro gasMicro, const CSC2State &state) const;

protected:
	wxString m_name;
	CSC2RaceData m_data;
	CVector<const CSC2Command *> m_allCommands;
	CVector<CSC2XMLCommand *> m_xmlCommands;
	CVector<CSC2RequiredCommand *> m_requiredCommands;
	CVector<CSC2Target *> m_targets;

	CVector<CVector<const CSC2Command *> *> m_buildingPrerequisitCommands;
	CVector<CVector<CVector<const CSC2Command *> *> *> m_buildingStatusPrerequisitCommands;
	CVector<CVector<const CSC2Command *> *> m_unitPrerequisitCommands;
	CVector<CVector<const CSC2Command *> *> m_researchPrerequisitCommands;
	CVector<const CSC2Command *> m_buildGeyserBuildingCommands;
	CVector<const CSC2Command *> m_macroAbilityCommands;
	CVector<const CSC2Command *> m_moveWorkersToMineralsCommands;
	CVector<const CSC2Command *> m_moveWorkersToGasCommands;
	CVector<const CSC2Command *> m_provideSupplyCommands;

	bool LoadCommandXML(const wxXmlNode *xmlRace);
	bool LoadRequiredCommandXML(const wxXmlNode *xmlRace);
	bool LoadTargets(const wxXmlNode *xmlTargets);
};
