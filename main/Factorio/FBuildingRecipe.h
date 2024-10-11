#pragma once

#include "FRecipe.h"
#include "FBuilding.h"


class CFBuildingRecipe
{
public:
	CFBuildingRecipe(const size_t &id, const std::vector<CFItem*> &burnableItems, CFRecipe* &recipe, CFBuilding* &building);
	~CFBuildingRecipe();

	const wxString& GetName() const { return m_building->GetName() + "(" + m_recipe->GetName() + ")"; }

protected:
	size_t m_id;
	CFRecipe* m_recipe;
	CFBuilding* m_building;
	
	std::vector<size_t> m_ingredientIds;
	std::vector<double> m_ingredientRates; // per second
	std::vector<size_t> m_itemIds; 
	std::vector<double> m_itemRates; // per second
};
