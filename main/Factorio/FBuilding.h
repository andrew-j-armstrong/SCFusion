#pragma once

#include <wx/xml/xml.h>

#include "FRecipe.h"

class CFBuilding
{
public:
	CFBuilding(const size_t& id);
	~CFBuilding();

	bool Load(const wxXmlNode* node, const std::map<wxString, size_t>& itemIdMap, const std::map<wxString, size_t>& recipeIdMap);

	const wxString& GetName() const { return m_name; }
	const double& GetSpeed() const { return m_speed; }
	const double& GeBurnerConsumption() const { return m_burnerConsumption; }

protected:
	size_t m_id;
	wxString m_name; // Name of building
	std::vector<size_t> m_requiredItemIds; // Items required to build
	std::vector<size_t> m_requiredItemCounts; // Count of requried items
	std::vector<size_t> m_recipeIds; // Recipes available for this building
	double m_speed; // final recipe time = recipe time / building speed
	size_t m_outputStacks;
	double m_burnerConsumption; // in kW
};

