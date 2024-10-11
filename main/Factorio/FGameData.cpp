#include "FGameData.h"

CFGameData::CFGameData()
	: m_items()
	, m_itemIdMap()
	, m_burnableItems()
	, m_recipes()
	, m_recipeIdMap()
	, m_buildings()
	, m_buildingIdMap()
{
}

CFGameData::~CFGameData()
{
}

bool CFGameData::LoadItems(const wxXmlNode* node)
{
	for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Item"))
		{
			size_t id = m_items.size();
			CFItem* item = new CFItem(id);
			if (!item->Load(child))
			{
				delete item;
				return false;
			}

			m_items.push_back(item);
			m_itemIdMap.emplace(item->GetName(), id);

			if (item->GetBurnEnergy() > 0)
			{
				m_burnableItems.push_back(item);
			}
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Items>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CFGameData::LoadRecipes(const wxXmlNode* node)
{
	for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Recipe"))
		{
			size_t id = m_recipes.size();
			CFRecipe* recipe = new CFRecipe(id);
			if (!recipe->Load(child, m_itemIdMap))
			{
				delete recipe;
				return false;
			}

			m_recipes.push_back(recipe);
			m_recipeIdMap.emplace(recipe->GetName(), id);
		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Recipes>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}

bool CFGameData::LoadBuildings(const wxXmlNode* node)
{
	for (wxXmlNode* child = node->GetChildren(); child; child = child->GetNext())
	{
		if (wxXML_COMMENT_NODE == child->GetType())
			continue;

		if (child->GetName() == wxT("Building"))
		{
			size_t id = m_buildings.size();
			CFBuilding* building = new CFBuilding(id);
			if (!building->Load(child, m_itemIdMap, m_recipeIdMap))
			{
				delete building;
				return false;
			}

			m_buildings.push_back(building);
			m_buildingIdMap.emplace(building->GetName(), id);

		}
		else
		{
			wxFAIL_MSG(wxString::Format("Unexpected XML tag in <Buildings>: '%s'", child->GetName()));
			return false;
		}
	}

	return true;
}