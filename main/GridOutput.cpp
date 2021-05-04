#include "GridOutput.h"
#include "GridItem.h"
#include <map>


#define ID_CONTEXT_MENU 	2000
// checkbox ID's must follow; accessed as ID_CONTEXT_MENU + i
#define ID_COL_MINERALS		2001
#define ID_COL_GAS		    2002
#define ID_COL_LARVAE		2003
#define ID_COL_MINERAL_RATE	2004
#define ID_COL_GAS_RATE	    2005
#define ID_COL_WORKERS		2006
#define ID_COL_SUPPLY		2007

#define ID_COL_ALL	    	2008
#define ID_COL_NONE		    2009

BEGIN_EVENT_TABLE(GridOutput, wxGrid)
    EVT_CONTEXT_MENU(GridOutput::OnRightClick)
END_EVENT_TABLE()

GridOutput::GridOutput(wxWindow* parent, wxWindowID id, bool hasLarvae) :
    wxGrid(parent, id, wxDefaultPosition, wxDefaultSize, wxBORDER_THEME)
    , m_level(0)
{
    m_visible_cols[3] = hasLarvae;
    m_disabled_cols[3] = !hasLarvae;

    CreateGrid(0, 9);
    EnableEditing(false);
    DisableDragRowSize();
    HideRowLabels();
    SetDefaultCellAlignment(wxALIGN_RIGHT, wxALIGN_CENTRE);
    SetDoubleBuffered(true);

    SetGridColumns();

    m_menu = new wxMenu();
    m_menu->AppendCheckItem(ID_COL_MINERALS, "Minerals");
    m_menu->Check(ID_COL_MINERALS, m_visible_cols[1]);
    m_menu->AppendCheckItem(ID_COL_GAS, "Gas");
    m_menu->Check(ID_COL_GAS, m_visible_cols[2]);
    if (!m_disabled_cols[3])
    {
        m_menu->AppendCheckItem(ID_COL_LARVAE, "Larvae");
        m_menu->Check(ID_COL_LARVAE, m_visible_cols[3]);
    }
    m_menu->AppendCheckItem(ID_COL_MINERAL_RATE, "Mineral Rate");
    m_menu->Check(ID_COL_MINERAL_RATE, m_visible_cols[4]);
    m_menu->AppendCheckItem(ID_COL_GAS_RATE, "Gas Rate");
    m_menu->Check(ID_COL_GAS_RATE, m_visible_cols[5]);
    m_menu->AppendCheckItem(ID_COL_WORKERS, "Workers");
    m_menu->Check(ID_COL_WORKERS, m_visible_cols[6]);
    m_menu->AppendCheckItem(ID_COL_SUPPLY, "Supply");
    m_menu->Check(ID_COL_SUPPLY, m_visible_cols[7]);
    m_menu->AppendSeparator();
    m_menu->Append(ID_COL_ALL, "All columns");
    m_menu->Append(ID_COL_NONE, "No optional columns");

    m_menu->Connect(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(GridOutput::OnMenuItemClick), NULL, this);

}

const std::map<GridItem::GridItemType, wxColor> COLORFUL = {
   {GridItem::tDefault, wxColor(255, 255, 255)},
   {GridItem::tMilestone, wxColor(255, 255, 255)},
   {GridItem::tBase, wxColor(204, 229, 255)},
   {GridItem::tGas, wxColor(204, 229, 204)},
   {GridItem::tSupply, wxColor(255, 255, 204)},
   {GridItem::tStatus, wxColor(255, 255, 255)},
   {GridItem::tMilitary, wxColor(255, 204, 204)},
   {GridItem::tMilitaryUnit, wxColor(255, 229, 229)},
   {GridItem::tWorker, wxColor(229, 242, 255)},
   {GridItem::tResearch, wxColor(255, 229, 255)},
};

void GridOutput::SetGridColumns()
{
    int targetColCount = 0;
    for (int i = 0; i < 9; i++)
    {
        if (m_visible_cols[i]) targetColCount++;
    }

    if (GetNumberCols() > targetColCount)
        DeleteCols(0, GetNumberCols() - targetColCount);
    else if (GetNumberCols() < targetColCount)
        AppendCols(targetColCount - GetNumberCols());


    SetColLabelValue(0, "Time");
    SetColSize(0, 55);

    int colIndex = 1;
    if (m_visible_cols[1])
    {
        SetColLabelValue(colIndex, "Minerals");
        SetColSize(colIndex, 60);
        colIndex++;
    }
    if (m_visible_cols[2])
    {
        SetColLabelValue(colIndex, "Gas");
        SetColSize(colIndex, 60);
        colIndex++;
    }
    if (m_visible_cols[3])
    {
        SetColLabelValue(colIndex, "Larvae");
        SetColSize(colIndex, 60);
        colIndex++;
    }
    if (m_visible_cols[4])
    {
        SetColLabelValue(colIndex, "Mineral Rate");
        SetColSize(colIndex, 80);
        colIndex++;
    }
    if (m_visible_cols[5])
    {
        SetColLabelValue(colIndex, "Gas Rate");
        SetColSize(colIndex, 60);
        colIndex++;
    }
    if (m_visible_cols[6])
    {
        SetColLabelValue(colIndex, "Workers");
        SetColSize(colIndex, 60);
        colIndex++;
    }
    if (m_visible_cols[7])
    {
        SetColLabelValue(colIndex, "Supply");
        SetColSize(colIndex, 60);
        colIndex++;
    }

    SetColLabelValue(colIndex, "Command / Event / Milestone");
    SetColSize(colIndex, 600);

    UpdateGrid();
}

void GridOutput::OnRightClick(wxContextMenuEvent&)
{
    PopupMenu(m_menu);
}

void GridOutput::OnMenuItemClick(wxCommandEvent& evt)
{
    switch (evt.GetId()) {
    case ID_COL_MINERALS:
        m_visible_cols[1] = m_menu->IsChecked(ID_COL_MINERALS);
        break;
    case ID_COL_GAS:
        m_visible_cols[2] = m_menu->IsChecked(ID_COL_GAS);
        break;
    case ID_COL_LARVAE:
        m_visible_cols[3] = m_menu->IsChecked(ID_COL_LARVAE);
        break;
    case ID_COL_MINERAL_RATE:
        m_visible_cols[4] = m_menu->IsChecked(ID_COL_MINERAL_RATE);
        break;
    case ID_COL_GAS_RATE:
        m_visible_cols[5] = m_menu->IsChecked(ID_COL_GAS_RATE);
        break;
    case ID_COL_WORKERS:
        m_visible_cols[6] = m_menu->IsChecked(ID_COL_WORKERS);
        break;
    case ID_COL_SUPPLY:
        m_visible_cols[7] = m_menu->IsChecked(ID_COL_SUPPLY);
        break;
    case ID_COL_ALL:
        for (size_t i = 0; i < 9; i++)
        {
            if (!m_disabled_cols[i])
            {
                m_visible_cols[i] = true;
                m_menu->Check(ID_CONTEXT_MENU + i, true);
            }
        }
        break;
    case ID_COL_NONE:
        for (size_t i = 0; i < 9; i++)
        {
            if (!m_disabled_cols[i])
            {
                m_visible_cols[i] = false;
                m_menu->Check(ID_CONTEXT_MENU + i, false);
            }
        }
        break;
    }

    SetGridColumns();
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
                DrawRow(rowIndex, item);
                rowIndex++;
        }
    }

    if ((size_t)GetNumberRows() > rowIndex)
    {
        DeleteRows(rowIndex, GetNumberRows() - rowIndex);
    }

}

void GridOutput::DrawRow(size_t rowIndex, GridItem item)
{
    if (rowIndex >= (size_t)GetNumberRows()) InsertRows(rowIndex, 1);

    SetCellValue(rowIndex, 0, wxString::Format(L"%2d:%05.2f ", (int)(item.time / 60) - 60 * (int)(item.time / 3600), item.time - 60 * (int)(item.time / 60)));
    SetCellBackgroundColour(rowIndex, 0, COLORFUL.at(item.itemType));

    int colIndex = 1;
    if (m_visible_cols[1])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%d ", item.minerals));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }
    if (m_visible_cols[2])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%d ", item.gas));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }
    if (m_visible_cols[3])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%d ", item.larvae));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }
    if (m_visible_cols[4])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%.2f ", item.mineralIncomeRate));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }
    if (m_visible_cols[5])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%.2f ", item.gasIncomeRate));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }
    if (m_visible_cols[6])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%d ", item.workers));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }
    if (m_visible_cols[7])
    {
        SetCellValue(rowIndex, colIndex, wxString::Format(L"%d / %d ", item.supply, item.supplyCap));
        SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));
        colIndex++;
    }

    wxString name = " " + item.name;
    if (item.itemType == GridItem::tMilestone)
    {
        name += "\n Buildings: " + item.buildingsCompleted;
        name += "\n Units: " + item.unitsCompleted;
        name += "\n Research: " + item.researchCompleted;
    }
    SetCellValue(rowIndex, colIndex, name);
    SetCellBackgroundColour(rowIndex, colIndex, COLORFUL.at(item.itemType));

    SetCellAlignment(rowIndex, colIndex, wxALIGN_LEFT, wxALIGN_CENTER);
    SetRowSize(rowIndex, GetDefaultRowSize() * (item.itemType == GridItem::tMilestone ? 4 : 1));
}