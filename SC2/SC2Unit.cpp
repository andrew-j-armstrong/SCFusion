#include "stdafx.h"
#include "SC2Unit.h"

CSC2Unit::CSC2Unit()
	: m_name()
	, m_gameStartCount(0)
	, m_isWorker(false)
	, m_isUnique(false)
	, m_supplyCost(0.0)
	, m_providedSupply(0)
	, m_mineralIncomeRate(0.0)
	, m_expirationTime(0.0)
	, m_startingEnergy(0.0)
	, m_maxEnergy(0.0)
	, m_energyRechargeRate(0.0)
{
}

bool CSC2Unit::LoadXML(const wxXmlNode *xmlUnit)
{
	for(wxXmlNode *child = xmlUnit->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("ProvidedSupply"))
		{
			unsigned long providedSupply;
			content.ToULong(&providedSupply);
			m_providedSupply = (size_t)providedSupply;
		}
		else if (child->GetName() == wxT("SupplyCost"))
		{
			content.ToCDouble(&m_supplyCost);
		}
		else if (child->GetName() == wxT("IsWorker"))
		{
			if(content == "True")
				m_isWorker = true;
			else
				m_isWorker = false;
		}
		else if (child->GetName() == wxT("IsUnique"))
		{
			if(content == "True")
				m_isUnique = true;
			else
				m_isUnique = false;
		}
		else if (child->GetName() == wxT("StartingEnergy"))
		{
			content.ToCDouble(&m_startingEnergy);
		}
		else if (child->GetName() == wxT("MaxEnergy"))
		{
			content.ToCDouble(&m_maxEnergy);
		}
		else if (child->GetName() == wxT("EnergyRechargeRate"))
		{
			content.ToCDouble(&m_energyRechargeRate);
		}
		else if (child->GetName() == wxT("GameStartCount"))
		{
			unsigned long gameStartCount;
			content.ToULong(&gameStartCount);
			m_gameStartCount = (size_t)gameStartCount;
		}
		else if (child->GetName() == wxT("MineralIncomeRate"))
		{
			content.ToCDouble(&m_mineralIncomeRate);
		}
		else if (child->GetName() == wxT("ExpirationTime"))
		{
			content.ToCDouble(&m_expirationTime);
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Unit>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

