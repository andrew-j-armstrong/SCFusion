#pragma once

#include <wx/wx.h>

class ChartItem
{

public:
    enum ChartItemType
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

    ChartItem(wxString name, double startTime, double endTime, ChartItemType itemType = tDefault, QueueType queueType = qSingle)
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
    ChartItemType itemType;
    QueueType queueType;
};