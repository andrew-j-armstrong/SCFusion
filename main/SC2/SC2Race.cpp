#include "stdafx.h"
#include "SC2Race.h"

wxString tostring(const ESC2Race &race)
{
	switch(race)
	{
	case eProtoss:
		return wxT("Protoss");
	case eTerran:
		return wxT("Terran");
	case eZerg:
		return wxT("Zerg");
	}

	return wxT("");
}
