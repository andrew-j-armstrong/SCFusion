#pragma once

enum EFItemType
{
	eSolid
	, eFluid
	, eElectricity
};

wxString itemTypeToString(const EFItemType& itemType);
