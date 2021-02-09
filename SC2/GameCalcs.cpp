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

size_t CGameCalcs::GasWorkerLimit1Near1Far(size_t baseCount, size_t geyserBuildingCount)
{
	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;
	return 3 * ((geyserBuildingCount + 1)/2) + 4 * (geyserBuildingCount/2);
}

size_t CGameCalcs::GasWorkerLimit2Far(size_t baseCount, size_t geyserBuildingCount)
{
	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;
	return 4 * geyserBuildingCount;
}

double CGameCalcs::CalculateMineralIncomeRate(size_t baseCount, size_t workerCount)
{
	static double sumIncome[] = {0.0, 225/60.0, 455/60.0, 680/60.0, 910/60.0, 1030/60.0, 1075/60.0}; // Sum of income rates
	static double incomeRate[] = {56/60.0, 57/60.0, 56/60.0, 57/60.0, 30/60.0, 11/60.0, 0.0}; // Income rates

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
	static double closeGeyserSumIncome[] = {0.0, 56/60.0, 112/60.0, 158/60.0}; // Sum of income rates
	static double incomeRate[] = {56/60.0, 56/60.0, 46/60.0, 0.0}; // Income rates

	if (geyserBuildingCount == 0)
		return 0;

	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;

	if(workerCount > 3*geyserBuildingCount)
		workerCount = 3*geyserBuildingCount;

	size_t incomeRateIndex = workerCount / geyserBuildingCount;
	return geyserBuildingCount* closeGeyserSumIncome[incomeRateIndex] + (workerCount % geyserBuildingCount)*incomeRate[incomeRateIndex];
}

double CGameCalcs::CalculateGasIncomeRate1Near1Far(size_t baseCount, size_t geyserBuildingCount, size_t workerCount)
{
	static double closeGeyserSumIncome[] = {0.0, 56/60.0, 112/60.0, 158/60.0}; // Sum of income rates
	static double farGeyserSumIncome[] = {0.0, 12/60.0, 23/60.0, 34/60.0, 45/60.0}; // Sum of income rates

	if (geyserBuildingCount == 0)
		return 0;

	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;

	size_t closeGeyserCount = (geyserBuildingCount + 1) / 2;
	size_t closeGeyserWorkerCount = workerCount;
	if(closeGeyserWorkerCount > 3*closeGeyserCount)
		closeGeyserWorkerCount = 3*closeGeyserCount;

	size_t farGeyserCount = geyserBuildingCount / 2;
	size_t farGeyserWorkerCount = workerCount-closeGeyserWorkerCount;
	if(farGeyserWorkerCount > 4*farGeyserCount)
		farGeyserWorkerCount = 4*farGeyserCount;

	return (114/60.0) * (closeGeyserWorkerCount/3) + closeGeyserSumIncome[closeGeyserWorkerCount%3]
		 + (114/60.0) * (farGeyserWorkerCount/4) + farGeyserSumIncome[farGeyserWorkerCount%4];
}

double CGameCalcs::CalculateGasIncomeRate2Far(size_t baseCount, size_t geyserBuildingCount, size_t workerCount)
{
	static double farGeyserSumIncome[] = {0.0, 12/60.0, 23/60.0, 34/60.0, 45/60.0}; // Sum of income rates

	if (geyserBuildingCount == 0)
		return 0;

	if(geyserBuildingCount > 2 * baseCount)
		geyserBuildingCount = 2 * baseCount;

	if(workerCount > 4*geyserBuildingCount)
		workerCount = 4*geyserBuildingCount;

	return (114/60.0) * (workerCount/4) + farGeyserSumIncome[workerCount%4];
}
