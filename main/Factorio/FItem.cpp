#include "stdafx.h"
#include "FItem.h"
#include "FItemType.h"

CFItem::CFItem(const size_t& id)
	: m_id(id)
	, m_name()
	, m_type(eSolid)
	, m_stackSize(0)
	, m_burnEnergy(0)
{
}

CFItem::~CFItem()
{
}

bool CFItem::Load(const wxXmlNode* node) {
	for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("Type"))
		{
			if (content == itemTypeToString(eFluid))
			{
				m_type = eFluid;
			}
			else if(content == itemTypeToString(eElectricity))
			{
				m_type = eElectricity;
			}
			else
			{
				m_type = eSolid;
			}
		}
		else if (child->GetName() == wxT("StackSize"))
		{
			unsigned long stackSize;
			content.ToULong(&stackSize);
			m_stackSize = (size_t)stackSize;
		}
		else if (child->GetName() == wxT("BurnEnergy"))
		{
			unsigned long burnEnergy;
			content.ToULong(&burnEnergy);
			m_stackSize = (size_t)burnEnergy;
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Item>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}
