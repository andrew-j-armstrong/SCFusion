#pragma once

#include "../Core/Vector.h"

class CFEvent
{
public:
	enum EEventCategory
	{
		eCraftDone = 0 // Crafting a recipe is done
		, eTransferred // Inventory has been transferred
		, ePlacingDone // Building has been placed
		, eBuildingStopped // Micro-managed building stops because of ingredient shortage
	};

	struct SEvent
	{
		SEvent(const SEvent& event)
			: m_data(event.m_data)
		{
		}

		SEvent(double startTime, size_t eventCategory, size_t craftedItemID, size_t data)
		{
			m_data.m_startTime = startTime;
			m_data.m_eventCategory = eventCategory;
			m_data.m_craftedItemID = craftedItemID;
			m_data.m_data = data;
		}

		void operator=(const SEvent& event)
		{
			m_data = event.m_data;
		}

		struct SData
		{
			double m_startTime;
			size_t m_eventCategory : 2;
			size_t m_craftedItemID : 10;
			size_t m_data : 6;
		} m_data;
	};

	CFEvent(double time, size_t eventCategory, size_t craftedItemID = 0, size_t data = 0, double startTime = 0) : m_time(time), m_event(startTime, eventCategory, craftedItemID, data) {}

	CFEvent(double time, const SEvent& event) : m_time(time), m_event(event) {}

	double m_time;
	SEvent m_event;

	bool operator <(const CFEvent& event) const { return m_time < event.m_time; }
	bool operator >(const CFEvent& event) const { return m_time > event.m_time; }
};

template<>
void initArray(CFEvent* data, size_t from, size_t to);

