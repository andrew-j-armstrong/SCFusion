#pragma once

#include <wx/wx.h>

class VisualItem
{

public:
    enum VisualItemType
    {
        tDefault,
        tBase,
        tSupply,
        tGas,
        tStatus,
        tMilitary,
        tMilitaryUnit,
        tWorker,
        tResearch
    };

    VisualItem(wxString name, double startTime, double endTime, VisualItemType itemType = tDefault, bool isDoubleQueue = false);

    wxString name;
    double startTime;
    double endTime;
    VisualItemType itemType;
    bool isDoubleQueue;
};