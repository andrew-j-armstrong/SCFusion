#pragma once

#define SC2EVENT_BITFIELDS

#include "../Core/Vector.h"

class CSC2Event
{
public:
	enum EEventCategory
	{
		eWorkerStartMiningMinerals = 0
		, eWorkerStartMiningGas
		, eSendScout
		, eKillScout
		, eReturnScout
		, eBuildingComplete
		, eUnitComplete
		, eUnitMorph
		, eUnitDies // MULEs and other temporary units
		, eResearchComplete
		, eBuildingConsume
		, eBuildingMorph
		, eBuildingStatusApply
		, eBuildingStatusLapse
		, eBuildingSpawnLarvae
		, eBuildingSpawnBonusLarvae
		, eSpawnBase
		, eEventCount
	};

#ifdef SC2EVENT_BITFIELDS
	struct SEvent
	{
		SEvent(const SEvent &event)
			: m_data(event.m_data)
		{
		}

		SEvent(size_t eventCategory, bool sourceIsBuilding, size_t sourceID, size_t targetID, size_t data)
		{
			m_data.m_eventCategory = eventCategory;
			m_data.m_sourceIsBuilding = sourceIsBuilding;
			m_data.m_sourceID = sourceID;
			m_data.m_targetID = targetID;
			m_data.m_data = data;
		}

		void operator=(const SEvent &event)
		{
			m_data = event.m_data;
		}

		struct SData
		{
		size_t m_eventCategory : 5;
		bool m_sourceIsBuilding : 1;
		size_t m_sourceID : 10;
		size_t m_targetID : 10;
		size_t m_data : 6;
		} m_data;
	};

	CSC2Event(double time, size_t eventCategory, bool sourceIsBuilding = false, size_t sourceID = 0, size_t targetID = 0, size_t data = 0) : m_time(time), m_event(eventCategory, sourceIsBuilding, sourceID, targetID, data) {}

#elif SC2EVENT_UNIONBITFIELD
	union SEvent
	{
		SEvent(const SEvent &event)
			: m_copyMember(event.m_copyMember)
		{
		}

		SEvent(size_t eventCategory, bool sourceIsBuilding, size_t sourceID, size_t targetID, size_t data)
			: m_data.m_eventCategory(eventCategory)
			, m_data.m_sourceID(sourceID)
			, m_data.m_targetID(targetID)
			, m_data.m_data(data)
		{
		}

		void operator=(const SEvent &event)
		{
			m_copyMember = event.m_copyMember;
		}

		unsigned long m_copyMember;

		struct SData
		{
		size_t m_eventCategory : 5;
		bool m_sourceIsBuilding : 1;
		size_t m_sourceID : 10;
		size_t m_targetID : 10;
		size_t m_data : 6
		} m_data;
	}

	CSC2Event(double time, size_t eventCategory, bool sourceIsBuilding = false, size_t sourceID = 0, size_t targetID = 0, size_t data = 0) : m_time(time), m_event(eventCategory, sourceIsBuilding, sourceID, targetID, data) {}

#else
	struct SEvent
	{
		SEvent(const SEvent &event)
			: m_data(event.m_data)
		{
		}

		SEvent(unsigned char eventCategory, bool sourceIsBuilding, unsigned short sourceID, unsigned short targetID, unsigned char data)
			: m_data.m_eventCategory(eventCategory)
			, m_data.m_sourceID(sourceID)
			, m_data.m_targetID(targetID)
			, m_data.m_data(data)
		{
		}

		void operator=(const SEvent &event)
		{
			m_data = event.m_data;
		}

		struct SData
		{
			unsigned char m_eventCategory;
			bool m_sourceIsBuilding;
			unsigned short m_sourceID;
			unsigned short m_targetID;
			unsigned char m_data;
		} m_data;
	};

	CSC2Event(double time, unsigned char eventCategory, bool sourceIsBuilding = false, unsigned short sourceID = 0, unsigned short targetID = 0, unsigned char data = 0) : m_time(time), m_event(eventCategory, sourceIsBuilding, sourceID, targetID, data) {}

#endif

	CSC2Event(double time, const SEvent &event) : m_time(time), m_event(event) {}

	double m_time;
	SEvent m_event;

	bool operator <(const CSC2Event &event) const { return m_time < event.m_time; }
};

//const WCHAR *tostring(EOutputFormat format, CSC2Event::EEvent event);

//bool DisplayEvent(EOutputFormat format, CSC2Event::EEvent event);

template<>
void initArray(CSC2Event *data, size_t from, size_t to);
