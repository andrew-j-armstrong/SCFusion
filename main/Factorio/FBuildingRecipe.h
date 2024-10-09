#pragma once

#include "FRecipe.h"
#include "FBuilding.h"


class CFBuildingRecipe
{
public:
	CFBuildingRecipe(const size_t &id, const CFRecipe* &recipe, const CFBuilding* &building);
	~CFBuildingRecipe();

	const wxString& GetName() const { return m_building->GetName() + "(" + m_recipe->GetName() + ")"; }

protected:
	size_t m_id;
	CFRecipe* m_recipe;
	CFBuilding* m_building;
	
	std::vector<size_t> m_ingredientIds;
	std::vector<size_t> m_ingredientRates; // per second
	std::vector<size_t> m_itemIds; // Items created by recipe
	std::vector<size_t> m_itemRates; // Number of items created by recipe

};
