#include "VisualItem.h"

#include <wx/wx.h>

VisualItem::VisualItem(wxString name, double startTime, double endTime, VisualItemType itemType, QueueType queueType)
{
	this->startTime = startTime;
	this->endTime = endTime;
	this->name = name;
	this->itemType = itemType;
	this->queueType = queueType;
}