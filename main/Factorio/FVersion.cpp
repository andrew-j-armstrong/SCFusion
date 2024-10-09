#include "stdafx.h"

#include <wx/xml/xml.h>

#include "FVersion.h"

CFVersion::CFVersion()
{
}

CFVersion::~CFVersion()
{
	//RemoveAllPointer(m_races);
}

bool CFVersion::Load(const wxString& fileName)
{
	wxXmlDocument doc;
	if (!doc.Load(fileName))
		return false;

	wxXmlNode* node = doc.GetRoot()->GetChildren();
	while (node)
	{
		if (node->GetName() == wxT("XMLFileVersion"))
		{
		}
		else if (node->GetName() == wxT("GameShortDescription"))
		{
			m_gameShortDescription = node->GetNodeContent();
		}
		else if (node->GetName() == wxT("GameLongDescription"))
		{
			m_gameLongDescription = node->GetNodeContent();
		}
		else if (node->GetName() == wxT("MinVersion"))
		{
			m_minVersion = node->GetNodeContent();
		}
		else if (node->GetName() == wxT("Items"))
		{
			if (!LoadItems(node))
				return false;
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

bool CFVersion::LoadItems(const wxXmlNode* node)
{
	for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Item"))
		{
			FItem* item = new FItem();
			if (!item->Load(child))
			{
				delete item;
				return false;
			}

			m_items.push_back(item);
			m_itemNames.push_back(item->GetName());

			/*if (item->GetMaxLarvaeCount() > 0)
				m_hasLarvae = true;*/
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Buildings>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}