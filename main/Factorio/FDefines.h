#pragma once

#define F_BUILDING_ID_LIMIT			32
#define F_BUILDING_TRACKING_LIMIT		8
#define F_UNIT_ID_LIMIT				32
#define F_UNIT_TRACKING_LIMIT			8
#define F_RESEARCH_ID_LIMIT			64

#if F_BUILDING_ID_LIMIT <= 32
typedef unsigned long FBuildingFlags;
#else
typedef unsigned long long FBuildingFlags;
#endif

#if F_UNIT_ID_LIMIT <= 32
typedef unsigned long FUnitFlags;
#else
typedef unsigned long long FUnitFlags;
#endif

#if F_RESEARCH_ID_LIMIT <= 32
typedef unsigned long FResearchFlags;
#else
typedef unsigned long long FResearchFlags;
#endif

typedef unsigned char FBuildingStatusFlags;

typedef unsigned char FUnitStatusFlags;

enum EFSourceType
{
	eSourceTypeNone = -1
	, eSourceTypeUnit = 0
	, eSourceTypeBuilding = 1
};
#pragma once
