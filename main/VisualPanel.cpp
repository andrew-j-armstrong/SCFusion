#include "VisualPanel.h"

BEGIN_EVENT_TABLE(VisualPanel, wxScrolledWindow)
END_EVENT_TABLE()

VisualPanel::VisualPanel(wxFrame* parent, wxWindowID id) :
    wxScrolledWindow(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE)
{
    SetBackgroundColour(wxColour(255, 255, 255));
    SetScrollbars(10, 10, 200, 200, 0, 0);
}

bool compareStartTime(VisualItem a, VisualItem b)
{
    return a.startTime < b.startTime;
}

void VisualPanel::SetVisualItems(vector<vector<VisualItem>> visualItems)
{
    int maxEndTime = 0;

    // split stray items (not occupying a building) on multiple rows to avoid overlapping
    m_stray_visual_items.clear();
    if (visualItems.size() > 0)
    {
        vector<VisualItem> strayRow = visualItems[0];
        sort(strayRow.begin(), strayRow.end(), compareStartTime);
        for (size_t i = 0; i < strayRow.size(); i++)
        {
            // track max endTime
            if (strayRow[i].endTime > maxEndTime) maxEndTime = strayRow[i].endTime;

            bool fitted = false;
            size_t n = 0;
            while (!fitted && n < m_stray_visual_items.size())
            {
                if (m_stray_visual_items[n].back().endTime > strayRow[i].startTime)
                {
                    n++;
                }
                else
                {
                    m_stray_visual_items[n].push_back(strayRow[i]);
                    fitted = true;
                }
            }
            
            if (!fitted)
            {
                // add new row for stray item
                vector<VisualItem> row;
                row.push_back(strayRow[i]);
                m_stray_visual_items.push_back(row);
            }
        }

        visualItems[0].clear();
    }

    m_visual_items = visualItems;
    m_height = (m_visual_items.size() + m_stray_visual_items.size()) * 30 + 50;
    
    for (size_t i = 0; i < m_visual_items.size(); i++)
    {
        if (m_visual_items[i].size() && m_visual_items[i].back().endTime > maxEndTime) maxEndTime = m_visual_items[i].back().endTime;
    }
    m_width = maxEndTime * 5 + 50;

    wxPoint scrolled = GetViewStart();
    SetScrollbars(10, 10, (int)m_width/10, (int)m_height/10, scrolled.x, scrolled.y);
}

void VisualPanel::OnDraw(wxDC& dc)
{
    // Draw grid
    dc.SetPen(wxPen(wxColor(230, 230, 230), 1));
    dc.SetTextForeground(wxColor(127, 127, 127));
    for (int i = 1; i <= m_width / 75; i++)
    {
        dc.DrawLine(wxPoint(i * 75, 20), wxPoint(i * 75, m_height - 20));
        if (i % 4 == 0) dc.DrawLine(wxPoint(i * 75 - 1, 20), wxPoint(i * 75 - 1, m_height - 20));
        dc.DrawText(wxString::Format(L"%02d:%02d", i / 4, i % 4 * 15), i * 75 - 13, 3);
        dc.DrawText(wxString::Format(L"%02d:%02d", i / 4, i % 4 * 15), i * 75 - 13, m_height - 17);
    }

    // Draw items
    dc.SetBrush(wxColor(127, 191, 255));
    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetTextForeground(wxColor(0, 0, 0));

    const int pixelsPerSecond = 5;
    const int labelIndent = 3;
    const int labelTopMargin = 2;
    const int topMargin = 30;
    const int rowHeight = 30;
    const int defaultItemHeight = 20;
    const int statusHeight = 5;
    const int doubleQueueHeight = rowHeight / 2;
    const int doubleQueueMarginCorrection = -5;

    for (size_t i = 0; i < m_stray_visual_items.size(); i++)
    {
        for (auto item : m_stray_visual_items[i])
        {
            dc.DrawRectangle(
                item.startTime * pixelsPerSecond,
                i * rowHeight + topMargin,
                (item.endTime - item.startTime) * pixelsPerSecond - 1,
                defaultItemHeight - 1
            );
            dc.DrawText(item.name, item.startTime * pixelsPerSecond + labelIndent, i * rowHeight + topMargin + labelTopMargin);
        }
    }

    for (size_t i = 0; i < m_visual_items.size(); i++)
    {
        bool doubleQueueUpper = false;
        for (auto item : m_visual_items[i])
        {
            if (item.isStatus)
            {
                dc.SetBrush(wxColor(127, 255, 191));
                dc.DrawRectangle(
                    item.startTime * pixelsPerSecond,
                    (i + m_stray_visual_items.size()) * rowHeight + topMargin + defaultItemHeight,
                    (item.endTime - item.startTime) * pixelsPerSecond - 1,
                    statusHeight - 1
                );
                dc.SetBrush(wxColor(127, 191, 255));
            }
            else
            {
                if (item.isDoubleQueue)
                {
                    doubleQueueUpper = !doubleQueueUpper;
                    dc.DrawRectangle(
                        item.startTime * pixelsPerSecond,
                        (i + m_stray_visual_items.size()) * rowHeight + topMargin + (doubleQueueUpper ? 0 : doubleQueueHeight) + doubleQueueMarginCorrection,
                        (item.endTime - item.startTime) * pixelsPerSecond - 1,
                        doubleQueueHeight - 1
                    );
                    dc.DrawText(
                        item.name,
                        item.startTime * pixelsPerSecond + labelIndent,
                        (i + m_stray_visual_items.size()) * rowHeight + topMargin + (doubleQueueUpper ? 0 : doubleQueueHeight) + doubleQueueMarginCorrection - 1);
                }
                else
                {
                    dc.DrawRectangle(
                        item.startTime * pixelsPerSecond,
                        (i + m_stray_visual_items.size()) * rowHeight + topMargin,
                        (item.endTime - item.startTime) * pixelsPerSecond - 1,
                        defaultItemHeight - 1
                    );
                    dc.DrawText(item.name, item.startTime * pixelsPerSecond + labelIndent, (i + m_stray_visual_items.size()) * rowHeight + topMargin + labelTopMargin);
                }
            }
        }
    }
}