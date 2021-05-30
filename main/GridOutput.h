#pragma once

#include <wx/grid.h>
#include "GridItem.h"

using std::vector;

class GridOutput : public wxGrid
{
public:
	GridOutput(wxWindow* parent, wxWindowID id, bool hasLarvae);

	void SetData(vector<GridItem> data);

	void SetLevel(int level);

private:
	int m_level;
	vector<GridItem> m_data;
	wxMenu* m_menu;
	vector<bool> m_visible_cols { true, true, true, true, true, true, true, true, true };
	vector<bool> m_disabled_cols { true, false, false, false, false, false, false, false, true };

	void UpdateGrid();
	void DrawRow(size_t i, GridItem item);
	void OnMenuItemClick(wxCommandEvent& evt);
	void OnRightClick(wxContextMenuEvent& evt);
	void SetGridColumns();

DECLARE_EVENT_TABLE()
};