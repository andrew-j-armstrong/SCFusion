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
    SetColSize(8, 700);
}


void GridOutput::SetData(vector<GridItem> data)
{
    if (GetNumberRows() > (int)data.size())
    {
        DeleteRows(0, GetNumberRows() - data.size());
    }
    else if ((int)data.size() > GetNumberRows())
    {
        InsertRows(0, data.size() - GetNumberRows());
    }

    size_t i = 0;
    for (GridItem item : data)
    {
        SetCellValue(i, 0, wxString::Format(L"%2d:%05.2f ", (int)(item.time / 60) - 60 * (int)(item.time / 3600), item.time - 60 * (int)(item.time / 60)));
        SetCellValue(i, 1, wxString::Format(L"%d ", item.minerals));
        SetCellValue(i, 2, wxString::Format(L"%d ", item.gas));
        SetCellValue(i, 3, wxString::Format(L"%d ", item.larvae));
        SetCellValue(i, 4, wxString::Format(L"%.2f ", item.mineralIncomeRate));
        SetCellValue(i, 5, wxString::Format(L"%.2f ", item.gasIncomeRate));
        SetCellValue(i, 6, wxString::Format(L"%d ", item.workers));
        SetCellValue(i, 7, wxString::Format(L"%d / %d ", item.supply, item.supplyCap));

        wxString name = " " + item.name;
        if (item.itemType == GridItem::tMilestone)
        {
            name += "\n Buildings: " + item.buildingsCompleted;
            name += "\n Units: " + item.unitsCompleted;
            name += "\n Research: " + item.researchCompleted;
        }
        SetCellValue(i, 8, name);

        SetCellAlignment(i, 8, wxALIGN_LEFT, wxALIGN_CENTER);
        SetRowSize(i, GetDefaultRowSize() * (item.itemType == GridItem::tMilestone ? 4 : 1));

        i++;
    }

}