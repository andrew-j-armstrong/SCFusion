#include "ChartPanel.h"
#include <map>
#include <wx/dcsvg.h>
#include <wx/settings.h>

// Follow the OS: light theme, dark theme, we're not here to argue.
// (Twin of the one in GridOutput.cpp — promote to a header if it multiplies.)
static bool IsDarkTheme()
{
#if wxCHECK_VERSION(3, 1, 3)
    return wxSystemSettings::GetAppearance().IsDark();
#else
    return false;	// wx too old to ask; it was always light back then anyway
#endif
}

BEGIN_EVENT_TABLE(ChartPanel, wxScrolledWindow)
END_EVENT_TABLE()

ChartPanel::ChartPanel(wxFrame* parent, wxWindowID id) :
    wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
{
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
    SetScrollbars(10, 10, 200, 200, 0, 0);
    SetDoubleBuffered(true);
}

const int PIXELS_PER_SECOND = 5;
const int LABEL_OFFSET_LEFT = 3;
const int LABEL_OFFSET_TOP = 2;
const int OFFSET_TOP = 30;
const int ROW_HEIGHT = 30;
const int ITEM_HEIGHT = 20;
const int STATUS_HEIGHT = 5;
const int DOUBLE_QUEUE_ITEM_HEIGHT = ROW_HEIGHT / 2;
const int doubleQueueMarginCorrection = (ITEM_HEIGHT - ROW_HEIGHT) / 2;

// Gantt-bar palettes: pastels for light themes, embers for dark ones.
// (Function-local statics so wxSystemSettings isn't poked before wxApp.)
static const std::map<ChartItem::ChartItemType, wxColor> &ChartColours(bool colorful)
{
    static const std::map<ChartItem::ChartItemType, wxColor> colorfulLight = {
       {ChartItem::tDefault, wxColor(204, 204, 204)},
       {ChartItem::tBase, wxColor(153, 204, 255)},
       {ChartItem::tGas, wxColor(153, 204, 153)},
       {ChartItem::tSupply, wxColor(255, 204, 153)},
       {ChartItem::tStatus, wxColor(102, 229, 204)},
       {ChartItem::tMilitary, wxColor(255, 153, 153)},
       {ChartItem::tMilitaryUnit, wxColor(255, 204, 204)},
       {ChartItem::tWorker, wxColor(204, 229, 255)},
       {ChartItem::tResearch, wxColor(229, 204, 255)},
    };
    static const std::map<ChartItem::ChartItemType, wxColor> colorfulDark = {
       {ChartItem::tDefault, wxColor(72, 72, 72)},
       {ChartItem::tBase, wxColor(42, 84, 126)},
       {ChartItem::tGas, wxColor(54, 96, 54)},
       {ChartItem::tSupply, wxColor(122, 82, 42)},
       {ChartItem::tStatus, wxColor(28, 104, 88)},
       {ChartItem::tMilitary, wxColor(128, 52, 52)},
       {ChartItem::tMilitaryUnit, wxColor(100, 58, 58)},
       {ChartItem::tWorker, wxColor(52, 78, 108)},
       {ChartItem::tResearch, wxColor(92, 68, 120)},
    };
    static const std::map<ChartItem::ChartItemType, wxColor> plainLight = {
       {ChartItem::tDefault, wxColor(153, 204, 255)},
       {ChartItem::tBase, wxColor(153, 204, 255)},
       {ChartItem::tGas, wxColor(153, 204, 255)},
       {ChartItem::tSupply, wxColor(153, 204, 255)},
       {ChartItem::tStatus, wxColor(153, 204, 255)},
       {ChartItem::tMilitary, wxColor(153, 204, 255)},
       {ChartItem::tMilitaryUnit, wxColor(153, 204, 255)},
       {ChartItem::tWorker, wxColor(153, 204, 255)},
       {ChartItem::tResearch, wxColor(153, 204, 255)},
    };
    static const std::map<ChartItem::ChartItemType, wxColor> plainDark = {
       {ChartItem::tDefault, wxColor(42, 84, 126)},
       {ChartItem::tBase, wxColor(42, 84, 126)},
       {ChartItem::tGas, wxColor(42, 84, 126)},
       {ChartItem::tSupply, wxColor(42, 84, 126)},
       {ChartItem::tStatus, wxColor(42, 84, 126)},
       {ChartItem::tMilitary, wxColor(42, 84, 126)},
       {ChartItem::tMilitaryUnit, wxColor(42, 84, 126)},
       {ChartItem::tWorker, wxColor(42, 84, 126)},
       {ChartItem::tResearch, wxColor(42, 84, 126)},
    };
    if (IsDarkTheme())
        return colorful ? colorfulDark : plainDark;
    return colorful ? colorfulLight : plainLight;
}

bool compareStartTime(ChartItem a, ChartItem b)
{
    return a.startTime < b.startTime;
}

// store data needed for redraws
void ChartPanel::SetChartItems(vector<vector<ChartItem>> chartItems)
{
    int maxEndTime = 0;

    // split stray items (not occupying a building) on multiple rows to avoid overlapping
    m_stray_chart_items.clear();
    m_milestones.clear();
    if (chartItems.size() > 0)
    {
        vector<ChartItem> strayRow = chartItems[0];
        sort(strayRow.begin(), strayRow.end(), compareStartTime);
        for (size_t i = 0; i < strayRow.size(); i++)
        {
            // track max endTime
            if (strayRow[i].endTime > maxEndTime) maxEndTime = strayRow[i].endTime;

            if (strayRow[i].itemType == ChartItem::tMilestone)
            {
                m_milestones.push_back(strayRow[i]);
            }
            else
            {
                bool fitted = false;
                size_t n = 0;
                while (!fitted && n < m_stray_chart_items.size())
                {
                    if (m_stray_chart_items[n].back().endTime > strayRow[i].startTime || m_stray_chart_items[n].back().startTime + 10 > strayRow[i].startTime)
                    {
                        n++;
                    }
                    else
                    {
                        m_stray_chart_items[n].push_back(strayRow[i]);
                        fitted = true;
                    }
                }

                if (!fitted)
                {
                    // add new row for stray item
                    vector<ChartItem> row;
                    row.push_back(strayRow[i]);
                    m_stray_chart_items.push_back(row);
                }
            }
        }

        chartItems[0].clear();
    }

    m_chart_items = chartItems;
    m_height = (m_chart_items.size() + m_stray_chart_items.size()) * ROW_HEIGHT + 30;
    
    for (size_t i = 0; i < m_chart_items.size(); i++)
    {
        if (m_chart_items[i].size() && m_chart_items[i].back().endTime > maxEndTime) maxEndTime = m_chart_items[i].back().endTime;
    }
    m_width = (maxEndTime + 3) * PIXELS_PER_SECOND;

    wxPoint scrolled = GetViewStart();
    SetScrollbars(10, 10, (int)m_width/10, (int)m_height/10, scrolled.x, scrolled.y);
    Refresh();
}

wxColor ChartPanel::GetBrushColorByType(ChartItem::ChartItemType itemType)
{
    return ChartColours(m_colorful).at(itemType);
}

bool ChartPanel::ExportSVG(wxString filename)
{
    wxSVGFileDC svgDC(filename, m_width, m_height);
    OnDraw(svgDC);
    return svgDC.IsOk();
}

void ChartPanel::OnDraw(wxDC& dc)
{
    const bool dark = IsDarkTheme();
    const wxColor gridlineColour = dark ? wxColor(60, 60, 60) : wxColor(230, 230, 230);
    const wxColor milestoneColour = dark ? wxColor(90, 90, 90) : wxColor(204, 204, 204);

    // Draw grid
    dc.SetPen(wxPen(gridlineColour, 1));
    dc.SetTextForeground(wxColor(153, 153, 153));	// mid-grey: legible on both
    for (int i = 1; i <= m_width / 15 / PIXELS_PER_SECOND; i++)
    {
        dc.DrawLine(wxPoint(i * 15 * PIXELS_PER_SECOND, 20), wxPoint(i * 15 * PIXELS_PER_SECOND, m_height - 20));
        if (i % 4 == 0) dc.DrawLine(wxPoint(i * 15 * PIXELS_PER_SECOND - 1, 20), wxPoint(i * 15 * PIXELS_PER_SECOND - 1, m_height - 20));
        dc.DrawText(wxString::Format(L"%02d:%02d", i / 4, i % 4 * 15), i * 15 * PIXELS_PER_SECOND - 13, 3);
        dc.DrawText(wxString::Format(L"%02d:%02d", i / 4, i % 4 * 15), i * 15 * PIXELS_PER_SECOND - 13, m_height - 17);
    }

    // Draw milestones
    dc.SetTextForeground(wxColor(153, 153, 153));
    dc.SetBrush(gridlineColour);
    for (size_t i = 0; i < m_milestones.size(); i++)
    {
        dc.SetPen(wxPen(milestoneColour, 2));
        dc.DrawLine(wxPoint(m_milestones[i].endTime * PIXELS_PER_SECOND - 1, 20), wxPoint(m_milestones[i].endTime * PIXELS_PER_SECOND - 1, m_height - 20));
        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.DrawRectangle((m_milestones[i].endTime * PIXELS_PER_SECOND) - 18, 3, 36, 15);
        dc.DrawRectangle((m_milestones[i].endTime * PIXELS_PER_SECOND) - 18, m_height - 17, 36, 15);
        dc.DrawText(wxString::Format(L"%02d:%02d", (int)(m_milestones[i].endTime / 60), (int)(m_milestones[i].endTime) % 60), m_milestones[i].endTime * PIXELS_PER_SECOND - 13, 3);
        dc.DrawText(wxString::Format(L"%02d:%02d", (int)(m_milestones[i].endTime / 60), (int)(m_milestones[i].endTime) % 60), m_milestones[i].endTime * PIXELS_PER_SECOND - 13, m_height - 17);
    }

    // Draw items — bar labels need to contrast the bars, and the bars follow
    // the theme, so the text does too.
    dc.SetBrush(wxColor(127, 191, 255));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetTextForeground(dark ? wxColor(230, 230, 230) : wxColor(0, 0, 0));

    // Draw stray items first
    for (size_t i = 0; i < m_stray_chart_items.size(); i++)
    {
        for (auto item : m_stray_chart_items[i])
        {
            dc.SetBrush(GetBrushColorByType(item.itemType));
            dc.DrawRectangle(
                item.startTime * PIXELS_PER_SECOND,
                i * ROW_HEIGHT + OFFSET_TOP,
                (item.endTime - item.startTime) * PIXELS_PER_SECOND - 1,
                ITEM_HEIGHT - 1
            );
            dc.DrawText(item.name, item.startTime * PIXELS_PER_SECOND + LABEL_OFFSET_LEFT, i * ROW_HEIGHT + OFFSET_TOP + LABEL_OFFSET_TOP);
        }
    }

    // Draw items from buildings
    for (size_t i = 0; i < m_chart_items.size(); i++)
    {
        size_t offset = i + m_stray_chart_items.size() - 1;
        for (auto item : m_chart_items[i])
        {
            dc.SetBrush(GetBrushColorByType(item.itemType));
            if (item.itemType == ChartItem::tStatus)
            {
                dc.DrawRectangle(
                    item.startTime * PIXELS_PER_SECOND,
                    offset * ROW_HEIGHT + OFFSET_TOP + ITEM_HEIGHT,
                    (item.endTime - item.startTime) * PIXELS_PER_SECOND - 1,
                    STATUS_HEIGHT - 1
                );
            }
            else
            {
                if (item.queueType == ChartItem::qSingle)
                {
                    dc.DrawRectangle(
                        item.startTime * PIXELS_PER_SECOND,
                        offset * ROW_HEIGHT + OFFSET_TOP,
                        (item.endTime - item.startTime) * PIXELS_PER_SECOND - 1,
                        ITEM_HEIGHT - 1
                    );
                    dc.DrawText(item.name, item.startTime * PIXELS_PER_SECOND + LABEL_OFFSET_LEFT, offset * ROW_HEIGHT + OFFSET_TOP + LABEL_OFFSET_TOP);
                }
                else
                {
                    dc.DrawRectangle(
                        item.startTime * PIXELS_PER_SECOND,
                        offset * ROW_HEIGHT + OFFSET_TOP + (item.queueType == ChartItem::qDoublePrimary ? 0 : DOUBLE_QUEUE_ITEM_HEIGHT) + doubleQueueMarginCorrection,
                        (item.endTime - item.startTime) * PIXELS_PER_SECOND - 1,
                        DOUBLE_QUEUE_ITEM_HEIGHT - 1
                    );
                    dc.DrawText(
                        item.name,
                        item.startTime * PIXELS_PER_SECOND + LABEL_OFFSET_LEFT,
                        offset * ROW_HEIGHT + OFFSET_TOP + (item.queueType == ChartItem::qDoublePrimary ? 0 : DOUBLE_QUEUE_ITEM_HEIGHT) + doubleQueueMarginCorrection - 1);
                }
            }
        }
    }
}

void ChartPanel::SetColorfulOutput()
{
    m_colorful = true;
}

void ChartPanel::SetPlainOutput()
{
    m_colorful = false;
}