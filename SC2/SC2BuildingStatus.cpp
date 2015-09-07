#include "stdafx.h"
#include "SC2BuildingStatus.h"

CSC2BuildingStatus::CSC2BuildingStatus()
	: m_name()
	, m_productionBoostFactor(1.0)
{
}

bool CSC2BuildingStatus::LoadXML(const wxXmlNode *xmlResearch)
{
	for (wxXmlNode *child = xmlResearch->GetChildren(); child; child = child->GetNext())
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
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Research>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}
