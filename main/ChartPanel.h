#pragma once

#include <wx/wx.h>
#include <Wx/sizer.h>
#include "ChartItem.h"

using std::vector;

class ChartPanel : public wxScrolledWindow
{

public:
    ChartPanel(wxFrame* parent, wxWindowID id);

    void OnDraw(wxDC& dc);
    bool ExportSVG(wxString filename);

    void SetChartItems(vector<vector<ChartItem>> chartItems);
    void SetColorfulOutput();
    void SetPlainOutput();

    wxColor ChartPanel::GetBrushColorByType(ChartItem::ChartItemType itemType);

    DECLARE_EVENT_TABLE()

protected:
    vector<vector<ChartItem>> m_chart_items;
    vector<vector<ChartItem>> m_stray_chart_items;
    vector<ChartItem> m_milestones;
    int m_width;
    int m_height;
    bool m_colorful = false;
};
