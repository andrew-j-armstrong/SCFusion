#include "stdafx.h"
#include "SC2Research.h"

CSC2Research::CSC2Research()
	: m_name()
	, m_deprecateResearchIDs()
{
}

bool CSC2Research::LoadXML(const wxXmlNode *xmlResearch)
{
	for(wxXmlNode *child = xmlResearch->GetChildren(); child; child = child->GetNext())
	{
		if(wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("DeprecateResearch"))
		{
			m_deprecateResearch.push_back(content);
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Research>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CSC2Research::ResolveIDs(const std::vector<wxString> &researchNames)
{
	for(size_t i=0; i < m_deprecateResearch.size(); i++)
	{
		size_t researchID = UINT_MAX;
		for(size_t j=0; j < researchNames.size(); j++)
		{
			if(m_deprecateResearch[i] == researchNames[j])
			{
				researchID = j;
				break;
			}
		}

		wxCHECK_MSG(researchID < researchNames.size(), false, wxString::Format("Unknown research to be deprecated: '%s'", m_deprecateResearch[i]));

		m_deprecateResearchIDs.push_back(researchID);
	}

	return true;
}
