#pragma once

#include <wx/xml/xml.h>

#include "FItem.h"

class CFRecipe
{
public:
	CFRecipe(const size_t &id);
	~CFRecipe();

	bool Load(const wxXmlNode* node, const std::map<wxString, size_t> &itemIdMap);

	const wxString& GetName() const { return m_name; }
	const std::vector<size_t>& GetItemIds() const { return m_itemIds; }
	const std::vector<size_t>& GetItemCounts() const { return m_itemCounts; }
	const double& GetTime() const { return m_time; }
	const std::vector<size_t>& GetIngredienIds() const { return m_ingredientIds; }
	const std::vector<size_t>& GetIngredientCounts() const { return m_ingredientCounts; }
	const bool& CanBeCrafted() const { return m_crafting; }
	const bool& CanBeMined() const { return m_crafting; }
	
protected:
	size_t m_id;
	wxString m_name; // Name of recipe
	std::vector<size_t> m_ingredientIds;
	std::vector<size_t> m_ingredientCounts;
	std::vector<size_t> m_itemIds; // Items created by recipe
	std::vector<size_t> m_itemCounts; // Number of items created by recipe
	double m_time; // Time (in seconds) to create the items
	bool m_crafting; // Can player craft this recipe?
	bool m_mining; // Can player mine this recipe?

};

