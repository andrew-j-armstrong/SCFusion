#ifndef _VISUALPANEL_H_
#define _VISUALPANEL_H_

#include <wx/wx.h>
#include <Wx/sizer.h>
#include "VisualItem.h"

using std::vector;

class VisualPanel : public wxScrolledWindow
{

public:
    VisualPanel(wxFrame* parent, wxWindowID id);

    void OnDraw(wxDC& dc);
    bool ExportSVG(wxString filename);

    void SetVisualItems(vector<vector<VisualItem>> visualItems);
    void SetColorfulOutput();
    void SetPlainOutput();

    wxColor VisualPanel::GetBrushColorByType(VisualItem::VisualItemType itemType);

    DECLARE_EVENT_TABLE()

protected:
    vector<vector<VisualItem>> m_visual_items;
    vector<vector<VisualItem>> m_stray_visual_items;
    int m_width;
    int m_height;
    bool m_colorful = false;
};

#endif // _VISUALPANEL_H_