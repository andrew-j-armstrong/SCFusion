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
        tResearch,
        tMilestone
    };

    enum QueueType
    {
        qSingle,
        qDoublePrimary,
        qDoubleSecondary
    };

    VisualItem(wxString name, double startTime, double endTime, VisualItemType itemType = tDefault, QueueType queueType = qSingle)
    {
        this->startTime = startTime;
        this->endTime = endTime;
        this->name = name;
        this->itemType = itemType;
        this->queueType = queueType;
    }

    wxString name;
    double startTime;
    double endTime;
    VisualItemType itemType;
    QueueType queueType;
};