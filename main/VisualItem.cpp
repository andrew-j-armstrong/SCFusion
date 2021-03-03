#include "VisualItem.h"

#include <wx/wx.h>

VisualItem::VisualItem(wxString name, double startTime, double endTime, VisualItemType itemType, bool isDoubleQueue)
{
	this->startTime = startTime;
	this->endTime = endTime;
	this->name = name;
	this->itemType = itemType;
	this->isDoubleQueue = isDoubleQueue;
}