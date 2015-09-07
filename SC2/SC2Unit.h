#pragma once

#include <wx/xml/xml.h>

#include "../Core/Vector.h"

class CSC2Unit
{
public:
	CSC2Unit();
	~CSC2Unit() {}

	bool LoadXML(const wxXmlNode *xmlRace);

	const wxString &GetName() const { return m_name; }
	size_t GetGameStartCount() const { return m_gameStartCount; }
	bool IsWorker() const { return m_isWorker; }
	bool IsUnique() const { return m_isUnique; }
	size_t GetProvidedSupply() const { return m_providedSupply; }
	double GetSupplyCost() const { return m_supplyCost; }
	double GetStartingEnergy() const { return m_startingEnergy; }
	double GetMaxEnergy() const { return m_maxEnergy; }
	double GetEnergyRechargeRate() const { return m_energyRechargeRate; }
	double GetMineralIncomeRate() const { return m_mineralIncomeRate; }
	double GetExpirationTime() const { return m_expirationTime; }

protected:
	wxString m_name;
	size_t m_gameStartCount;
	bool m_isWorker;
	bool m_isUnique;
	double m_supplyCost;
	size_t m_providedSupply;
	double m_mineralIncomeRate;
	double m_expirationTime;
	double m_startingEnergy;
	double m_maxEnergy;
	double m_energyRechargeRate;
};
