#include "stdafx.h"
#include "FItemType.h"

wxString itemTypeToString(const EFItemType& itemType)
{
	switch (itemType)
	{
	case eSolid:
		return wxT("Solid");
	case eFluid:
		return wxT("Fluid");
	case eElectricity:
		return wxT("Electricity");
	}

	return wxT("");
}
