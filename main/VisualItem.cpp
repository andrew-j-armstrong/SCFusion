#include "VisualItem.h"

#include <wx/wx.h>

VisualItem::VisualItem(wxString name, double startTime, double endTime, bool isStatus, bool isDoubleQueue)
{
	this->startTime = startTime;
	this->endTime = endTime;
	this->name = name;
	this->isStatus = isStatus;
	this->isDoubleQueue = isDoubleQueue;
}