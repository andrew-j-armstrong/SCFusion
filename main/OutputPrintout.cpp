#include <OutputPrintout.h>
#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/print.h>

OutputPrintout::OutputPrintout(wxGrid* g, const wxString& title)
    : wxPrintout(title) {
    grid = g;
}
    
bool OutputPrintout::OnPrintPage(int page)
{
    wxDC* dc = GetDC();
    dc->SetFont(grid->GetFont());

    int pageWidth, pageHeight;
    GetPageSizePixels(&pageWidth, &pageHeight);

    int gridWidth = 0;
    wxGridSizesInfo colSizes = grid->GetColSizes();
    int gridColCount = grid->GetNumberCols();
    for (int i = 0; i < gridColCount; i++)
    {
        gridWidth += colSizes.GetSize(i);
    }

    int gridHeight = grid->GetColLabelSize();
    wxGridSizesInfo rowSizes = grid->GetRowSizes();
    int gridRowCount = grid->GetNumberRows();
    for (int i = 0; i < gridRowCount; i++)
    {
        gridHeight += rowSizes.GetSize(i);
    }

    float scale = (float)pageWidth / (float)gridWidth;
    int gridViewHeight = (int)((float)pageHeight / scale);

    int startRow = 0;
    int endRow = 0;
    
    int pages = 0;
    int row = 0;
    while ((row < gridRowCount) && (pages <= page))
    {
        pages++;
        if (pages == page) startRow = row;
        int viewHeight = grid->GetColLabelSize();
        while ((row < gridRowCount) && (viewHeight + rowSizes.GetSize(row) < gridViewHeight))
        {
            viewHeight += rowSizes.GetSize(row);
            row++;
        }
        if (pages == page) endRow = row - 1;
    }

    grid->Render(*dc, wxPoint(0, 0), wxDefaultSize,
        wxGridCellCoords(startRow, 0), wxGridCellCoords(endRow, gridColCount - 1));

    return true;
}

bool OutputPrintout::HasPage(int page)
{
    return (page <= GetPageCount());
}

bool OutputPrintout::OnBeginDocument(int startPage, int endPage)
{
    if (!wxPrintout::OnBeginDocument(startPage, endPage))
        return false;
    return true;
}

int OutputPrintout::GetPageCount()
{
    int pageWidth, pageHeight;
    GetPageSizePixels(&pageWidth, &pageHeight);

    int gridWidth = 0;
    wxGridSizesInfo colSizes = grid->GetColSizes();
    int gridColCount = grid->GetNumberCols();
    for (int i = 0; i < gridColCount; i++)
    {
        gridWidth += colSizes.GetSize(i);
    }

    int gridHeight = grid->GetColLabelSize();
    wxGridSizesInfo rowSizes = grid->GetRowSizes();
    int gridRowCount = grid->GetNumberRows();
    for (int i = 0; i < gridRowCount; i++)
    {
        gridHeight += rowSizes.GetSize(i);
    }

    float scale = (float)pageWidth / (float)gridWidth;
    int gridViewHeight = (int)((float)pageHeight / scale);

    int pages = 0;
    int row = 0;
    while (row < gridRowCount)
    {
        pages++;
        int viewHeight = grid->GetColLabelSize();
        while (viewHeight + rowSizes.GetSize(row) < gridViewHeight)
        {
            viewHeight += rowSizes.GetSize(row);
            row++;
        }
    }

    return pages;
}

void OutputPrintout::GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo)
{
    int pages = GetPageCount();
    
    *minPage = 1;
    *maxPage = pages;
    *selPageFrom = 1;
    *selPageTo = pages;
}
