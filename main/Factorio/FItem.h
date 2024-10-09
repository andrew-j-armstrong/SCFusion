#pragma once

#include <wx/xml/xml.h>

#include "../Core/Vector.h"

#include "FItemType.h"


class CFItem
{
public:
	CFItem(const size_t& id);
	~CFItem();

	bool Load(const wxXmlNode* node);

	const size_t& GetId() const { return m_id; }
	const wxString& GetName() const { return m_name; }
	const EFItemType& GetType() const { return m_type; }
	const size_t& GetStackSize() const { return m_stackSize; }
	const size_t& GetBurnEnergy() const { return m_burnEnergy; }

protected:
	size_t m_id;
	wxString m_name;
	EFItemType m_type;
	size_t m_stackSize;
	size_t m_burnEnergy;
};

