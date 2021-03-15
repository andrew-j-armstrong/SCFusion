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

    enum QueueType
    {
        qSingle,
        qDoublePrimary,
        qDoubleSecondary
    };

    VisualItem(wxString name, double startTime, double endTime, VisualItemType itemType = tDefault, QueueType queueType = qSingle);

    wxString name;
    double startTime;
    double endTime;
    VisualItemType itemType;
    QueueType queueType;
};