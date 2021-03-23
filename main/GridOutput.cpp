#include "GridOutput.h"

GridOutput::GridOutput(wxWindow* parent, wxWindowID id) :
    wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
{
    CreateGrid(0, 9);
    EnableEditing(false);
    DisableDragRowSize();
    HideRowLabels();
    SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
    SetDoubleBuffered(true);

    SetColLabelValue(0, "Time");
    SetColSize(0, 55);
    SetColLabelValue(1, "Minerals");
    SetColSize(1, 60);
    SetColLabelValue(2, "Gas");
    SetColSize(2, 60);
    SetColLabelValue(3, "Larvae");
    SetColSize(3, 60);
    SetColLabelValue(4, "Mineral Rate");
    SetColSize(4, 80);
    SetColLabelValue(5, "Gas Rate");
    SetColSize(5, 60);
    SetColLabelValue(6, "Workers");
    SetColSize(6, 60);
    SetColLabelValue(7, "Supply");
    SetColSize(7, 60);
    SetColLabelValue(8, "Event");
    SetColSize(8, 500);
}


void GridOutput::SetData(vector<GridItem> data)
{
    if (GetNumberRows() > 0)
        DeleteRows(0, GetNumberRows());
    InsertRows(0, data.size());

    for (size_t i = 0; i < data.size(); i++)
    {
        SetCellValue(i, 0, wxString::Format(L"%2d:%05.2f ", (int)(data[i].time / 60) - 60 * (int)(data[i].time / 3600), data[i].time - 60 * (int)(data[i].time / 60)));
        SetCellValue(i, 1, wxString::Format(L"%d ", data[i].minerals));
        SetCellValue(i, 2, wxString::Format(L"%d ", data[i].gas));
        SetCellValue(i, 3, wxString::Format(L"%d ", data[i].larvae));
        SetCellValue(i, 4, wxString::Format(L"%.2f ", data[i].mineralIncomeRate));
        SetCellValue(i, 5, wxString::Format(L"%.2f ", data[i].gasIncomeRate));
        SetCellValue(i, 6, wxString::Format(L"%d ", data[i].workers));
        SetCellValue(i, 7, wxString::Format(L"%d / %d ", data[i].supply, data[i].supplyCap));
        SetCellValue(i, 8, data[i].name);
        SetCellAlignment(i, 8, wxALIGN_LEFT, wxALIGN_CENTER);
    }

}