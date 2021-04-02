#include "GridOutput.h"

GridOutput::GridOutput(wxWindow* parent, wxWindowID id) :
    wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , m_level(0)
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
    m_data = data;
    UpdateGrid();
}

void GridOutput::SetLevel(int level)
{
    m_level = level;
    UpdateGrid();
}

void GridOutput::UpdateGrid()
{
    
    size_t rowIndex = 0;
    for (GridItem item : m_data)
    {
        switch (item.level)
        {
            case GridItem::lFull:
                if (m_level < 2) break;
            case GridItem::lDetailed:
                if (m_level < 1) break;
            default:
                if (rowIndex >= GetNumberRows()) InsertRows(rowIndex, 1);
                AddRow(rowIndex, item);
                rowIndex++;
        }
    }

    if (GetNumberRows() > rowIndex)
    {
        DeleteRows(rowIndex, GetNumberRows() - rowIndex);
    }

}

void GridOutput::AddRow(size_t rowIndex, GridItem item)
{
    SetCellValue(rowIndex, 0, wxString::Format(L"%2d:%05.2f ", (int)(item.time / 60) - 60 * (int)(item.time / 3600), item.time - 60 * (int)(item.time / 60)));
    SetCellValue(rowIndex, 1, wxString::Format(L"%d ", item.minerals));
    SetCellValue(rowIndex, 2, wxString::Format(L"%d ", item.gas));
    SetCellValue(rowIndex, 3, wxString::Format(L"%d ", item.larvae));
    SetCellValue(rowIndex, 4, wxString::Format(L"%.2f ", item.mineralIncomeRate));
    SetCellValue(rowIndex, 5, wxString::Format(L"%.2f ", item.gasIncomeRate));
    SetCellValue(rowIndex, 6, wxString::Format(L"%d ", item.workers));
    SetCellValue(rowIndex, 7, wxString::Format(L"%d / %d ", item.supply, item.supplyCap));

    wxString name = " " + item.name;
    if (item.itemType == GridItem::tMilestone)
    {
        name += "\n Buildings: " + item.buildingsCompleted;
        name += "\n Units: " + item.unitsCompleted;
        name += "\n Research: " + item.researchCompleted;
    }
    SetCellValue(rowIndex, 8, name);

    SetCellAlignment(rowIndex, 8, wxALIGN_LEFT, wxALIGN_CENTER);
    SetRowSize(rowIndex, GetDefaultRowSize() * (item.itemType == GridItem::tMilestone ? 4 : 1));
}