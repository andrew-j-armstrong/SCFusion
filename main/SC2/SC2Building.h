#pragma once

#include <wx/xml/xml.h>

#include "../Core/Vector.h"
#include "SC2Defines.h"
#include "SC2BuildingStatus.h"

class CSC2Building
{
public:
	CSC2Building();
	~CSC2Building() {}

	bool LoadXML(const wxXmlNode *xmlBuilding);

	const wxString &GetName() const { return m_name; }
	size_t GetGameStartCount() const { return m_gameStartCount; }
	bool IsBase() const { return m_isBase; }
	bool IsGeyserBuilding() const { return m_isGeyserBuilding; }
	size_t GetProvidedSupply() const { return m_providedSupply; }
	double GetStartingEnergy() const { return m_startingEnergy; }
	double GetMaxEnergy() const { return m_maxEnergy; }
	double GetEnergyRechargeRate() const { return m_energyRechargeRate; }
	size_t GetGameStartLarvaeCount() const { return m_gameStartLarvaeCount; }
	size_t GetInitialLarvaeCount() const { return m_initialLarvaeCount; }
	size_t GetMaxLarvaeSpawningCount() const { return m_maxLarvaeSpawningCount; }
	size_t GetMaxLarvaeCount() const { return m_maxLarvaeCount; }
	double GetLarvaeSpawnTime() const { return m_larvaeSpawnTime; }
	SC2BuildingStatusFlags GetInitialStatus() const { return m_initialStatus; }
	const CVector<size_t> &GetInitialStatusList() const { return m_initialStatusList; }
	const CVector<double> &GetInitialStatusDurations() const { return m_initialStatusDurations; }
	double GetInitialProductionBoost() const { return m_initialProductionBoost; }
	SC2BuildingStatusFlags GetGameStartStatus() const { return m_gameStartStatus; }
	const CVector<size_t> &GetGameStartStatusList() const { return m_gameStartStatusList; }
	const CVector<double> &GetGameStartStatusDurations() const { return m_gameStartStatusDurations; }
	double GetGameStartProductionBoost() const { return m_gameStartProductionBoost; }

	bool ResolveIDs(const CVector<CSC2BuildingStatus *> &buildingStatuses);

protected:
	wxString m_name;
	size_t m_gameStartCount;
	bool m_isBase;
	bool m_isGeyserBuilding;
	size_t m_providedSupply;
	double m_startingEnergy;
	double m_maxEnergy;
	double m_energyRechargeRate;
	size_t m_gameStartLarvaeCount;
	size_t m_initialLarvaeCount; // Upon building completion
	size_t m_maxLarvaeSpawningCount;
	size_t m_maxLarvaeCount;
	double m_larvaeSpawnTime;
	std::vector<wxString> m_initialStatusNames;
	SC2BuildingStatusFlags m_initialStatus;
	CVector<size_t> m_initialStatusList;
	CVector<double> m_initialStatusDurations;
	double m_initialProductionBoost;
	std::vector<wxString> m_gameStartStatusNames;
	SC2BuildingStatusFlags m_gameStartStatus;
	CVector<size_t> m_gameStartStatusList;
	CVector<double> m_gameStartStatusDurations;
	double m_gameStartProductionBoost;
};
