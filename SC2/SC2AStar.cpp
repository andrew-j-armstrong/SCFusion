#include "stdafx.h"
#include "SC2AStar.h"

template<> CSC2ASCost CSC2ASState<CProtossTarget, CProtossState, EProtossCommand, CProtossEvent>::CalculateExpectedCost(const CSC2ASConfig<CProtossTarget, CProtossState, EProtossCommand, CProtossEvent> &config)
{
	double mineralCost = 0.0;
	double gasCost = 0.0;

	const CProtossTarget &target = *config.GetFitnessCalc().GetTargets()[config.GetFitnessCalc().GetTargets().size() - 1];
	if(target.m_nexusCount.min > m_state.m_nexusCount + m_state.m_nexusUnderConstruction)
	{
		mineralCost += 400.0 * (target.m_nexusCount.min - (m_state.m_nexusCount + m_state.m_nexusUnderConstruction));
	}
	if(target.m_assimilatorCount.min > m_state.m_assimilatorCount + m_state.m_assimilatorUnderConstruction)
	{
		mineralCost += 75.0 * (target.m_assimilatorCount.min - (m_state.m_assimilatorCount + m_state.m_assimilatorUnderConstruction));
	}
	if(target.m_pylonCount.min > m_state.m_pylonCount + m_state.m_pylonUnderConstruction)
	{
		mineralCost += 100.0 * (target.m_pylonCount.min - (m_state.m_pylonCount + m_state.m_pylonUnderConstruction));
	}
	if(target.m_gatewayCount.min > m_state.m_gatewayCount + m_state.m_gatewayUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_gatewayCount.min - (m_state.m_gatewayCount + m_state.m_gatewayUnderConstruction));
	}
	if(target.m_warpGateCount.min > m_state.m_warpGateCount + m_state.m_warpGateUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_warpGateCount.min - (m_state.m_warpGateCount + m_state.m_warpGateUnderConstruction));
	}
	if(target.m_forgeCount.min > m_state.m_forgeCount + m_state.m_forgeUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_forgeCount.min - (m_state.m_forgeCount + m_state.m_forgeUnderConstruction));
	}
	if(target.m_photonCannonCount.min > m_state.m_photonCannonCount + m_state.m_photonCannonUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_photonCannonCount.min - (m_state.m_photonCannonCount + m_state.m_photonCannonUnderConstruction));
	}
	if(target.m_cyberneticsCoreCount.min > m_state.m_cyberneticsCoreCount + m_state.m_cyberneticsCoreUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_cyberneticsCoreCount.min - (m_state.m_cyberneticsCoreCount + m_state.m_cyberneticsCoreUnderConstruction));
	}
	if(target.m_twilightCouncilCount.min > m_state.m_twilightCouncilCount + m_state.m_twilightCouncilUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_twilightCouncilCount.min - (m_state.m_twilightCouncilCount + m_state.m_twilightCouncilUnderConstruction));
		gasCost += 100.0 * (target.m_twilightCouncilCount.min - (m_state.m_twilightCouncilCount + m_state.m_twilightCouncilUnderConstruction));
	}
	if(target.m_templarArchivesCount.min > m_state.m_templarArchivesCount + m_state.m_templarArchivesUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_templarArchivesCount.min - (m_state.m_templarArchivesCount + m_state.m_templarArchivesUnderConstruction));
		gasCost += 200.0 * (target.m_templarArchivesCount.min - (m_state.m_templarArchivesCount + m_state.m_templarArchivesUnderConstruction));
	}
	if(target.m_darkShrineCount.min > m_state.m_darkShrineCount + m_state.m_darkShrineUnderConstruction)
	{
		mineralCost += 100.0 * (target.m_darkShrineCount.min - (m_state.m_darkShrineCount + m_state.m_darkShrineUnderConstruction));
		gasCost += 250.0 * (target.m_darkShrineCount.min - (m_state.m_darkShrineCount + m_state.m_darkShrineUnderConstruction));
	}
	if(target.m_roboticsFacilityCount.min > m_state.m_roboticsFacilityCount + m_state.m_roboticsFacilityUnderConstruction)
	{
		mineralCost += 200.0 * (target.m_roboticsFacilityCount.min - (m_state.m_roboticsFacilityCount + m_state.m_roboticsFacilityUnderConstruction));
		gasCost += 100.0 * (target.m_roboticsFacilityCount.min - (m_state.m_roboticsFacilityCount + m_state.m_roboticsFacilityUnderConstruction));
	}
	if(target.m_roboticsBayCount.min > m_state.m_roboticsBayCount + m_state.m_roboticsBayUnderConstruction)
	{
		mineralCost += 200.0 * (target.m_roboticsBayCount.min - (m_state.m_roboticsBayCount + m_state.m_roboticsBayUnderConstruction));
		gasCost += 200.0 * (target.m_roboticsBayCount.min - (m_state.m_roboticsBayCount + m_state.m_roboticsBayUnderConstruction));
	}
	if(target.m_stargateCount.min > m_state.m_stargateCount + m_state.m_stargateUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_stargateCount.min - (m_state.m_stargateCount + m_state.m_stargateUnderConstruction));
		gasCost += 150.0 * (target.m_stargateCount.min - (m_state.m_stargateCount + m_state.m_stargateUnderConstruction));
	}
	if(target.m_fleetBeaconCount.min > m_state.m_fleetBeaconCount + m_state.m_fleetBeaconUnderConstruction)
	{
		mineralCost += 300.0 * (target.m_fleetBeaconCount.min - (m_state.m_fleetBeaconCount + m_state.m_fleetBeaconUnderConstruction));
		gasCost += 200.0 * (target.m_fleetBeaconCount.min - (m_state.m_fleetBeaconCount + m_state.m_fleetBeaconUnderConstruction));
	}

	if(target.m_probeCount.min > m_state.m_probeCount + m_state.m_probeUnderConstruction)
	{
		mineralCost += 50.0 * (target.m_probeCount.min - (m_state.m_probeCount + m_state.m_probeUnderConstruction));
	}
	if(target.m_zealotCount.min > m_state.m_zealotCount + m_state.m_zealotUnderConstruction)
	{
		mineralCost += 100.0 * (target.m_zealotCount.min - (m_state.m_zealotCount + m_state.m_zealotUnderConstruction));
	}
	if(target.m_stalkerCount.min > m_state.m_stalkerCount + m_state.m_stalkerUnderConstruction)
	{
		mineralCost += 125.0 * (target.m_stalkerCount.min - (m_state.m_stalkerCount + m_state.m_stalkerUnderConstruction));
		gasCost += 50.0 * (target.m_stalkerCount.min - (m_state.m_stalkerCount + m_state.m_stalkerUnderConstruction));
	}
	if(target.m_sentryCount.min > m_state.m_sentryCount + m_state.m_sentryUnderConstruction)
	{
		mineralCost += 50.0 * (target.m_sentryCount.min - (m_state.m_sentryCount + m_state.m_sentryUnderConstruction));
		gasCost += 100.0 * (target.m_sentryCount.min - (m_state.m_sentryCount + m_state.m_sentryUnderConstruction));
	}
	if(target.m_highTemplarCount.min > m_state.m_highTemplarCount + m_state.m_highTemplarUnderConstruction)
	{
		mineralCost += 50.0 * (target.m_highTemplarCount.min - (m_state.m_highTemplarCount + m_state.m_highTemplarUnderConstruction));
		gasCost += 150.0 * (target.m_highTemplarCount.min - (m_state.m_highTemplarCount + m_state.m_highTemplarUnderConstruction));
	}
	if(target.m_darkTemplarCount.min > m_state.m_darkTemplarCount + m_state.m_darkTemplarUnderConstruction)
	{
		mineralCost += 125.0 * (target.m_darkTemplarCount.min - (m_state.m_darkTemplarCount + m_state.m_darkTemplarUnderConstruction));
		gasCost += 125.0 * (target.m_darkTemplarCount.min - (m_state.m_darkTemplarCount + m_state.m_darkTemplarUnderConstruction));
	}
	if(target.m_archonCount.min > m_state.m_archonCount + m_state.m_archonUnderConstruction)
	{
		// Minimum of each
		mineralCost += 100.0 * (target.m_archonCount.min - (m_state.m_archonCount + m_state.m_archonUnderConstruction));
		gasCost += 250.0 * (target.m_archonCount.min - (m_state.m_archonCount + m_state.m_archonUnderConstruction));
	}
	if(target.m_warpPrismCount.min > m_state.m_warpPrismCount + m_state.m_warpPrismUnderConstruction)
	{
		mineralCost += 200.0 * (target.m_warpPrismCount.min - (m_state.m_warpPrismCount + m_state.m_warpPrismUnderConstruction));
	}
	if(target.m_observerCount.min > m_state.m_observerCount + m_state.m_observerUnderConstruction)
	{
		mineralCost += 25.0 * (target.m_observerCount.min - (m_state.m_observerCount + m_state.m_observerUnderConstruction));
		gasCost += 75.0 * (target.m_observerCount.min - (m_state.m_observerCount + m_state.m_observerUnderConstruction));
	}
	if(target.m_immortalCount.min > m_state.m_immortalCount + m_state.m_immortalUnderConstruction)
	{
		mineralCost += 250.0 * (target.m_immortalCount.min - (m_state.m_immortalCount + m_state.m_immortalUnderConstruction));
		gasCost += 100.0 * (target.m_immortalCount.min - (m_state.m_immortalCount + m_state.m_immortalUnderConstruction));
	}
	if(target.m_colossusCount.min > m_state.m_colossusCount + m_state.m_colossusUnderConstruction)
	{
		mineralCost += 300.0 * (target.m_colossusCount.min - (m_state.m_colossusCount + m_state.m_colossusUnderConstruction));
		gasCost += 200.0 * (target.m_colossusCount.min - (m_state.m_colossusCount + m_state.m_colossusUnderConstruction));
	}
	if(target.m_phoenixCount.min > m_state.m_phoenixCount + m_state.m_phoenixUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_phoenixCount.min - (m_state.m_phoenixCount + m_state.m_phoenixUnderConstruction));
		gasCost += 100.0 * (target.m_phoenixCount.min - (m_state.m_phoenixCount + m_state.m_phoenixUnderConstruction));
	}
	if(target.m_voidRayCount.min > m_state.m_voidRayCount + m_state.m_voidRayUnderConstruction)
	{
		mineralCost += 250.0 * (target.m_voidRayCount.min - (m_state.m_voidRayCount + m_state.m_voidRayUnderConstruction));
		gasCost += 150.0 * (target.m_voidRayCount.min - (m_state.m_voidRayCount + m_state.m_voidRayUnderConstruction));
	}
	if(target.m_carrierCount.min > m_state.m_carrierCount + m_state.m_carrierUnderConstruction)
	{
		mineralCost += 300.0 * (target.m_carrierCount.min - (m_state.m_carrierCount + m_state.m_carrierUnderConstruction));
		gasCost += 200.0 * (target.m_carrierCount.min - (m_state.m_carrierCount + m_state.m_carrierUnderConstruction));
	}
	if(target.m_mothershipCount.min > m_state.m_mothershipCount + m_state.m_mothershipUnderConstruction)
	{
		mineralCost += 400.0 * (target.m_mothershipCount.min - (m_state.m_mothershipCount + m_state.m_mothershipUnderConstruction));
		gasCost += 400.0 * (target.m_mothershipCount.min - (m_state.m_mothershipCount + m_state.m_mothershipUnderConstruction));
	}
	/*
	// Research
	, m_researchGroundWeapons1Completed(false, false)
	, m_researchGroundWeapons2Completed(false, false)
	, m_researchGroundWeapons3Completed(false, false)
	, m_researchGroundArmor1Completed(false, false)
	, m_researchGroundArmor2Completed(false, false)
	, m_researchGroundArmor3Completed(false, false)
	, m_researchShields1Completed(false, false)
	, m_researchShields2Completed(false, false)
	, m_researchShields3Completed(false, false)
	, m_researchAirWeapons1Completed(false, false)
	, m_researchAirWeapons2Completed(false, false)
	, m_researchAirWeapons3Completed(false, false)
	, m_researchAirArmor1Completed(false, false)
	, m_researchAirArmor2Completed(false, false)
	, m_researchAirArmor3Completed(false, false)
	, m_researchWarpGateTransformationCompleted(false, true)
	, m_researchHallucinationCompleted(false, false)
	, m_researchChargeCompleted(false, false)
	, m_researchBlinkCompleted(false, false)
	, m_researchPsionicStormCompleted(false, false)
	, m_researchKhaydarinAmuletCompleted(false, false)
	, m_researchGraviticDriveCompleted(false, false)
	, m_researchGraviticBoostersCompleted(false, false)
	, m_researchExtendedThermalLanceCompleted(false, false)
	, m_researchFluxVanesCompleted(false, false)
	, m_researchGravitonCatapultCompleted(false, false)
	, m_constantProbeProduction(false)
	*/
	double requiredMinerals = mineralCost - m_state.m_minerals;
	double requiredGas = gasCost - m_state.m_gas;

	if(requiredMinerals < 0.0)
		requiredMinerals = 0.0;
	if(requiredGas < 0.0)
		requiredGas = 0.0;

	double probeMiningTime = requiredMinerals / (45.0 / 60.0) + requiredGas / (38.0 / 60.0);

	double expectedTime = (-(17*(2.0/3.0)*(m_state.m_probeCount + m_state.m_probeUnderConstruction)) + sqrt((17*(2.0/3.0)*(m_state.m_probeCount + m_state.m_probeUnderConstruction)) * (17*(2.0/3.0)*(m_state.m_probeCount + m_state.m_probeUnderConstruction)) + 4 * (17 * (2.0/3.0)) * probeMiningTime)) / 2.0;

	return CSC2ASCost(expectedTime);
}


template<> CSC2ASCost CSC2ASState<CTerranTarget, CTerranState, ETerranCommand, CTerranEvent>::CalculateExpectedCost(const CSC2ASConfig<CTerranTarget, CTerranState, ETerranCommand, CTerranEvent> &config)
{
	double mineralCost = 0.0;
	double gasCost = 0.0;
	double barracksBuildTime = 0.0;

	const CTerranTarget &target = *config.GetFitnessCalc().GetTargets()[config.GetFitnessCalc().GetTargets().size() - 1];
	if(target.m_commandCenterCount.min > m_state.m_commandCenterCount + m_state.m_commandCenterUnderConstruction + m_state.m_orbitalCommandCount + m_state.m_orbitalCommandUnderConstruction)
	{
		mineralCost += 400.0 * (target.m_commandCenterCount.min - (m_state.m_commandCenterCount + m_state.m_commandCenterUnderConstruction + m_state.m_orbitalCommandCount + m_state.m_orbitalCommandUnderConstruction));
	}
	if(target.m_supplyDepotCount.min > m_state.m_supplyDepotCount + m_state.m_supplyDepotUnderConstruction)
	{
		mineralCost += 100.0 * (target.m_supplyDepotCount.min - (m_state.m_supplyDepotCount + m_state.m_supplyDepotUnderConstruction));
	}
	if(target.m_barracksCount.min > m_state.m_barracksCount + m_state.m_barracksUnderConstruction)
	{
		mineralCost += 150.0 * (target.m_barracksCount.min - (m_state.m_barracksCount + m_state.m_barracksUnderConstruction));
	}

	if(target.m_scvCount.min > m_state.m_scvCount + m_state.m_scvUnderConstruction)
	{
		mineralCost += 50.0 * (target.m_scvCount.min - (m_state.m_scvCount + m_state.m_scvUnderConstruction));
	}
	if(target.m_marineCount.min > m_state.m_marineCount + m_state.m_marineUnderConstruction)
	{
		mineralCost += 50.0 * (target.m_marineCount.min - (m_state.m_marineCount + m_state.m_marineUnderConstruction));
		barracksBuildTime += 25.0 * (target.m_marineCount.min - (m_state.m_marineCount + m_state.m_marineUnderConstruction));
	}

	double requiredMinerals = mineralCost - m_state.m_minerals;
	double requiredGas = gasCost - m_state.m_gas;

	if(requiredMinerals < 0.0)
		requiredMinerals = 0.0;
	if(requiredGas < 0.0)
		requiredGas = 0.0;

	double scvMiningTime = requiredMinerals / (45.0 / 60.0) + requiredGas / (38.0 / 60.0);

	double expectedMiningTime;
	if(scvMiningTime == 0.0)
		expectedMiningTime = 0.0;
	else
		expectedMiningTime = (-17.0*(m_state.m_scvCount + m_state.m_scvUnderConstruction) + sqrt((17.0*(m_state.m_scvCount + m_state.m_scvUnderConstruction)) * (17.0*(m_state.m_scvCount + m_state.m_scvUnderConstruction)) + 4 * 17.0 * scvMiningTime)) / 2.0;

	if(barracksBuildTime > expectedMiningTime)
		return CSC2ASCost(barracksBuildTime);
	else
		return CSC2ASCost(expectedMiningTime);
}
