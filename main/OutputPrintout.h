#pragma once

#include <wx/wx.h>
#include <wx/grid.h>
#include <wx/print.h>

class OutputPrintout : public wxPrintout
{
public:
    OutputPrintout(wxGrid* g, const wxString& title = "My printout");

    bool OnPrintPage(int page);
    bool HasPage(int page);
    bool OnBeginDocument(int startPage, int endPage);
    void GetPageInfo(int* minPage, int* maxPage, int* selPageFrom, int* selPageTo);

private:
    wxGrid* grid;

    int GetPageCount();
};
