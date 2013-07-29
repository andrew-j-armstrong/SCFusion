#pragma once

#define SC2_BASE_LIMIT	10

class CGameCalcs
{
public:
	CGameCalcs() { InitData(); }

	static void InitData();

	static size_t MineralWorkerLimit(size_t baseCount);
	static size_t GasWorkerLimit2Near(size_t baseCount, size_t geyserBuildingCount);
	static size_t GasWorkerLimit1Near1Far(size_t baseCount, size_t geyserBuildingCount);
	static size_t GasWorkerLimit2Far(size_t baseCount, size_t geyserBuildingCount);

	static double GetMineralIncomeRate(size_t baseCount, size_t workerCount) { return m_mineralIncomeRates[mymin(baseCount, (size_t)SC2_BASE_LIMIT)][workerCount]; }
	static double GetGasIncomeRate2Near(size_t baseCount, size_t geyserBuildingCount, size_t workerCount) { return CalculateGasIncomeRate2Near(baseCount, geyserBuildingCount, workerCount); }
	static double GetGasIncomeRate1Near1Far(size_t baseCount, size_t geyserBuildingCount, size_t workerCount) { return CalculateGasIncomeRate1Near1Far(baseCount, geyserBuildingCount, workerCount); }
	static double GetGasIncomeRate2Far(size_t baseCount, size_t geyserBuildingCount, size_t workerCount) { return CalculateGasIncomeRate2Far(baseCount, geyserBuildingCount, workerCount); }

protected:
	static CGameCalcs m_init;

	static double CalculateMineralIncomeRate(size_t baseCount, size_t workerCount);
	static double CalculateGasIncomeRate2Near(size_t baseCount, size_t geyserBuildingCount, size_t workerCount);
	static double CalculateGasIncomeRate1Near1Far(size_t baseCount, size_t geyserBuildingCount, size_t workerCount);
	static double CalculateGasIncomeRate2Far(size_t baseCount, size_t geyserBuildingCount, size_t workerCount);

	static double m_mineralIncomeRates[SC2_BASE_LIMIT + 1][300];
};
