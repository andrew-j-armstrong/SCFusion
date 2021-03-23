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

    GridItem(wxString name, double time, GridItemType itemType)
    {
        this->name = name;
        this->time = time;
        this->itemType = itemType;
    }

    wxString name;
    double time;
    GridItemType itemType;

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