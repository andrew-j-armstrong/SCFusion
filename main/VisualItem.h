#pragma once

#include <wx/wx.h>

class VisualItem
{

public:
    VisualItem(wxString name, double startTime, double endTime, bool isStatus = false, bool isDoubleQueue = false);

    wxString name;
    double startTime;
    double endTime;
    bool isStatus;
    bool isDoubleQueue;
};