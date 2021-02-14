#include "stdafx.h"
#include "GameCalcs.h"

CGameCalcs CGameCalcs::m_init;

double CGameCalcs::m_mineralIncomeRates[SC2_BASE_LIMIT + 1][300];

void CGameCalcs::InitData()
{
	for(int baseCount=0; baseCount <= SC2_BASE_LIMIT; baseCount++)
		for(int workerCount=0; workerCount < 300; workerCount++)
			m_mineralIncomeRates[baseCount][workerCount] = CalculateMineralIncomeRate(baseCount, workerCount);
}

size_t CGameCalcs::MineralWorkerLimit(size_t baseCount)
{
	if(baseCount > SC2_BASE_LIMIT)
		baseCount = SC2_BASE_LIMIT;
	return 3 * 8 * baseCount;
}

size_t CGameCalcs::GasWorkerLimit2Near(size_t baseCount, size_t geyserBuildingCount)
{
	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;
	return 3 * geyserBuildingCount;
}

double CGameCalcs::CalculateMineralIncomeRate(size_t baseCount, size_t workerCount)
{
	static double sumIncome[] = {0.0, 236/60.0, 470/60.0, 704/60.0, 936/60.0, 1070/60.0, 1115/60.0}; // Sum of income rates
	static double incomeRate[] = {59/60.0, 58.5/60.0, 58.5/60.0, 58/60.0, 33.5/60.0, 11.25/60.0, 0.0}; // Income rates

	size_t patchCount = 4 * baseCount; // Yes, I know it's 8 per base, this is per type of patch (near/far) & how many workers on that patch (1,2,3), in the order that they'd get used
	if(workerCount > 24*baseCount)
		workerCount = 24*baseCount;

	if(patchCount == 0 || workerCount == 0)
		return 0;

	size_t incomeRateIndex = workerCount/patchCount;
	return baseCount*sumIncome[incomeRateIndex] + (workerCount % patchCount)*incomeRate[incomeRateIndex];
}

double CGameCalcs::CalculateGasIncomeRate2Near(size_t baseCount, size_t geyserBuildingCount, size_t workerCount)
{
	static double closeGeyserSumIncome[] = {0.0, 62.5/60.0, 125/60.0, 163/60.0}; // Sum of income rates
	static double incomeRate[] = {62.5/60.0, 62.5/60.0, 38/60.0, 0.0}; // Income rates

	if (geyserBuildingCount == 0)
		return 0;

	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;

	if(workerCount > 3*geyserBuildingCount)
		workerCount = 3*geyserBuildingCount;

	size_t incomeRateIndex = workerCount / geyserBuildingCount;
	return geyserBuildingCount* closeGeyserSumIncome[incomeRateIndex] + (workerCount % geyserBuildingCount)*incomeRate[incomeRateIndex];
}
