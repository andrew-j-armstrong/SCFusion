#pragma once

#include <wx/wx.h>

class GridItem
{

public:
    enum GridItemType
    {
        tDefault,
        tBase,
        tSupply,
        tGas,
        tStatus,
        tMilitary,
        tMilitaryUnit,
        tWorker,
        tResearch,
        tMilestone
    };

    enum GridItemLevel
    {
        lSimple,
        lDetailed,
        lFull
    };

    GridItem(wxString name, double time, GridItemType itemType = tDefault, GridItemLevel level = lFull)
    {
        this->name = name;
        this->time = time;
        this->itemType = itemType;
        this->level = level;
    }

    wxString name;
    double time;
    GridItemType itemType;
    GridItemLevel level;

    int minerals;
    int gas;
    int larvae;
    double mineralIncomeRate;
    double gasIncomeRate;
    int supply;
    int workers;
    int supplyCap;

    wxString buildingsCompleted;
    wxString unitsCompleted;
    wxString researchCompleted;
};