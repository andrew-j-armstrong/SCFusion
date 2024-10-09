#pragma once

#include <wx/xml/xml.h>
#include "FItem.h"
#include "FRecipe.h"
#include "FBuilding.h"

class CFGameData
{
public:
	CFGameData();
	~CFGameData();

	bool LoadItems(const wxXmlNode* node);
	bool LoadRecipes(const wxXmlNode* node);
	bool LoadBuildings(const wxXmlNode* node);


protected:
	std::vector<CFItem*> m_items;
	std::map<wxString, size_t> m_itemIdMap;
	std::vector<size_t> m_burnableItemIds;

	std::vector<CFRecipe*> m_recipes;
	std::map<wxString, size_t> m_recipeIdMap;

	std::vector<CFBuilding*> m_buildings;
	std::map<wxString, size_t> m_buildingIdMap;
};

