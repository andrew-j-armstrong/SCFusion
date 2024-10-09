#pragma once

#include "../Core/Vector.h"

#include "FItem.h"

class CFVersion
{
public:
	CFVersion();
	~CFVersion();

	bool Load(const wxString& fileName);

	const wxString& GetGameShortDescription() const { return m_gameShortDescription; }
	const wxString& GetGameLongDescription() const { return m_gameLongDescription; }
	const wxString& GetMinGameVersion() const { return m_minVersion; }
	const FItem* GetItems(const wxString& name) const;

protected:
	wxString m_gameShortDescription;
	wxString m_gameLongDescription;
	wxString m_minVersion;

	CVector<FItem*> m_items;
	CVector<wxString> m_itemNames;
	bool LoadItems(const wxXmlNode* node);
};
