#include "stdafx.h"
#include "SC2BuildingStatus.h"

CSC2BuildingStatus::CSC2BuildingStatus()
	: m_name()
	, m_productionBoostFactor(1.0)
	, m_isVisual(false)
{
}

bool CSC2BuildingStatus::LoadXML(const wxXmlNode *xmlStatus)
{
	for (wxXmlNode *child = xmlStatus->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("ProductionBoostFactor"))
		{
			content.ToCDouble(&m_productionBoostFactor);
		}
		else if (child->GetName() == wxT("IsVisual"))
		{
			m_isVisual = content == "True";
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <BuildingStatus>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}
