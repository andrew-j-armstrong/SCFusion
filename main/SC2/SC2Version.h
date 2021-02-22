#pragma once

#include "../Core/Vector.h"

#include "SC2RaceInfo.h"

class CSC2Version
{
public:
	CSC2Version();
	~CSC2Version();

	bool Load(const wxString &fileName);

	const wxString &GetGameShortDescription() const { return m_gameShortDescription; }
	const wxString &GetGameLongDescription() const { return m_gameLongDescription; }
	const wxString &GetMinGameVersion() const { return m_minVersion; }
	const CSC2RaceInfo *GetRace(const wxString &name) const;

protected:
	wxString m_gameShortDescription;
	wxString m_gameLongDescription;
	wxString m_minVersion;

	CVector<CSC2RaceInfo *> m_races;
};
