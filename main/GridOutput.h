#pragma once

#include <wx/grid.h>
#include "GridItem.h"

using std::vector;

class GridOutput : public wxGrid
{
public:
	GridOutput(wxWindow* parent, wxWindowID id);

	void SetData(vector<GridItem> data);

	void SetLevel(int level);

private:
	int m_level;
	vector<GridItem> m_data;

	void UpdateGrid();
	void AddRow(size_t i, GridItem item);
};