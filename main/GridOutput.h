#pragma once

#include <wx/grid.h>
#include "GridItem.h"

using std::vector;

class GridOutput : public wxGrid
{
public:
	GridOutput(wxWindow* parent, wxWindowID id);

	void SetData(vector<GridItem> data);
};