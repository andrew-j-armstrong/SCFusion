#pragma once

#define SC2_BUILDING_ID_LIMIT			32
#define SC2_BUILDING_TRACKING_LIMIT		8
#define SC2_UNIT_ID_LIMIT				32
#define SC2_UNIT_TRACKING_LIMIT			8
#define SC2_RESEARCH_ID_LIMIT			64

#if SC2_BUILDING_ID_LIMIT <= 32
typedef unsigned long SC2BuildingFlags;
#else
typedef unsigned long long SC2BuildingFlags;
#endif

#if SC2_UNIT_ID_LIMIT <= 32
typedef unsigned long SC2UnitFlags;
#else
typedef unsigned long long SC2UnitFlags;
#endif

#if SC2_RESEARCH_ID_LIMIT <= 32
typedef unsigned long SC2ResearchFlags;
#else
typedef unsigned long long SC2ResearchFlags;
#endif

typedef unsigned char SC2BuildingStatusFlags;

typedef unsigned char SC2UnitStatusFlags;

enum ESC2SourceType
{
	eSourceTypeNone = -1
	, eSourceTypeUnit = 0
	, eSourceTypeBuilding = 1
};
