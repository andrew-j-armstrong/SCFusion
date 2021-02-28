#pragma once

#include <wx/wx.h>

class VisualItem
{

public:
    VisualItem(wxString name, int startTime, int endTime, bool isStatus = false, bool isDoubleQueue = false);

    wxString name;
    int startTime;
    int endTime;
    bool isStatus;
    bool isDoubleQueue;
};