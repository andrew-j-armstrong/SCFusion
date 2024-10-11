#include "FBuildingRecipe.h"

CFBuildingRecipe::CFBuildingRecipe(const size_t& id, const std::vector<CFItem*>& burnableItems, CFRecipe*& recipe, CFBuilding*& building)
	: m_id(id)
	, m_recipe(recipe)
	, m_building(building)
	, m_ingredientIds()
	, m_ingredientRates()
	, m_itemIds()
	, m_itemRates()
{
	for (size_t i = 0; i < recipe->GetIngredienIds().size(); ++i) {
		m_ingredientIds.push_back(recipe->GetIngredienIds()[i]);
		m_ingredientRates.push_back(building->GetSpeed() * recipe->GetIngredientCounts()[i]);
	}

	for (size_t i = 0; i < recipe->GetItemIds().size(); ++i) {
		m_itemIds.push_back(recipe->GetItemIds()[i]);
		m_itemRates.push_back(building->GetSpeed() * recipe->GetItemCounts()[i]);
	}

	// add burnable fuel as ingredient
	for (size_t i = 0; i < burnableItems.size(); ++i) {

		auto it = std::find(m_ingredientIds.begin(), m_ingredientIds.end(), burnableItems[i]->GetId());

		if (it != m_ingredientIds.end()) {
			size_t index = std::distance(m_ingredientIds.begin(), it);
			m_ingredientRates[index] += building->GeBurnerConsumption() / burnableItems[i]->GetBurnEnergy();
		}
		else {
			m_ingredientIds.push_back(burnableItems[i]->GetId());
			m_ingredientRates.push_back(building->GeBurnerConsumption() / burnableItems[i]->GetBurnEnergy());
		}
	}
}

CFBuildingRecipe::~CFBuildingRecipe()
{
}