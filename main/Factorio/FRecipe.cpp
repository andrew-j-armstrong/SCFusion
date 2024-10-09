#include "FRecipe.h"

CFRecipe::CFRecipe(const size_t& id)
	: m_id(id)
	, m_name("")
	, m_ingredientIds()
	, m_ingredientCounts()
	, m_itemIds()
	, m_itemCounts()
	, m_time()
	, m_crafting()
	, m_mining()
{
}

CFRecipe::~CFRecipe()
{
}


bool CFRecipe::Load(const wxXmlNode* node, const std::map<wxString, size_t>& itemIdMap) {
	for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		wxString content = child->GetNodeContent();
		if (child->GetName() == wxT("Name"))
		{
			m_name = content;
		}
		else if (child->GetName() == wxT("Item"))
		{
			
			if (m_name == "")
			{
				m_name = content;
			}

			auto it = itemIdMap.find(content);
			if (it != itemIdMap.end()) {
				m_itemIds.push_back(it->second);

				unsigned long itemCount = 1;
				child->GetAttribute(wxT("count"), wxT("1")).ToULong(&itemCount);
				m_itemCounts.push_back((size_t)itemCount);
			}
			else {
				wxFAIL_MSG(wxString::Format("Unexpected Item Name : '%s'", content));
				return false;
			}
		}
		else if (child->GetName() == wxT("Time"))
		{
			double time;
			content.ToCDouble(&time);
			m_time = (size_t)time;
		}
		else if (child->GetName() == wxT("Ingredient"))
		{
			auto it = itemIdMap.find(content);
			if (it != itemIdMap.end()) {
				m_ingredientIds.push_back(it->second);

				unsigned long itemCount = 1;
				child->GetAttribute(wxT("count"), wxT("1")).ToULong(&itemCount);
				m_ingredientCounts.push_back((size_t)itemCount);
			}
			else {
				wxFAIL_MSG(wxString::Format("Unexpected Ingredient Name : '%s'", content));
				return false;
			}
		}
		else if (child->GetName() == wxT("Crafting"))
		{
			if (content == "True")
			{
				m_crafting = true;
			}
			else if (content == "False")
			{
				m_crafting = false;
			}
			else
			{
				wxFAIL_MSG(wxString::Format("Unexpected value in <Craft>: '%s'", child->GetName()));
				return false;
			}
		}
		else if (child->GetName() == wxT("Mining"))
		{
			if (content == "True")
			{
				m_mining = true;
			}
			else if (content == "False")
			{
				m_mining = false;
			}
			else
			{
				wxFAIL_MSG(wxString::Format("Unexpected value in <Craft>: '%s'", child->GetName()));
				return false;
			}
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Recipe>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}
