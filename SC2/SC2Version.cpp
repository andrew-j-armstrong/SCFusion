#include "stdafx.h"

#include <wx/xml/xml.h>

#include "SC2Version.h"

CSC2Version::CSC2Version()
{
}

CSC2Version::~CSC2Version()
{
	RemoveAllPointer(m_races);
}

bool CSC2Version::Load(const wxString &fileName)
{
	wxXmlDocument doc;
	if(!doc.Load(fileName))
		return false;

	wxXmlNode *node = doc.GetRoot()->GetChildren();
	while(node)
	{
		if(node->GetName() == wxT("XMLFileVersion"))
		{
		}
		else if(node->GetName() == wxT("GameShortDescription"))
		{
			m_gameShortDescription = node->GetNodeContent();
		}
		else if(node->GetName() == wxT("GameLongDescription"))
		{
			m_gameLongDescription = node->GetNodeContent();
		}
		else if(node->GetName() == wxT("MinVersion"))
		{
			m_minVersion = node->GetNodeContent();
		}
		else if(node->GetName() == wxT("Races"))
		{
			wxXmlNode *raceChild = node->GetChildren();
			while(raceChild)
			{
				CSC2RaceInfo *raceData = new CSC2RaceInfo();
				if(!raceData->LoadXML(raceChild))
				{
					delete raceData;
					return false;
				}

				m_races.push_back(raceData);

				raceChild = raceChild->GetNext();
			}
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Data>: '%s'", node->GetName()));
			return false;
		}

		node = node->GetNext();
	}

	return true;
}

const CSC2RaceInfo *CSC2Version::GetRace(const wxString &name) const
{
	for(size_t i=0; i < m_races.size(); i++)
	{
		if(m_races[i]->GetName() == name)
			return m_races[i];
	}

	return NULL;
}