#include "stdafx.h"
#include "OutputFormat.h"

void GetInitialYABOTOutput(ESC2Race race, wxString &output)
{
	switch(race)
	{
	case eProtoss:
		output.Append(L"100 [i] scbuildorderresult | 7 | SCBuildOrder | Build order developed by SCBuildOrder [/i] [s] ");
		break;
	case eTerran:
		output.Append(L"100 [i] scbuildorderresult | 3 | SCBuildOrder | Build order developed by SCBuildOrder [/i] [s] ");
		break;
	case eZerg:
		output.Append(L"100 [i] scbuildorderresult | 11 | SCBuildOrder | Build order developed by SCBuildOrder [/i] [s] ");
		break;
	}
}

wxString tostring(EOutputFormat format, ESC2Command command, size_t count /* = 1 */)
{
	wxString output;

	switch(format)
	{
	case eOutputFormatMinimal:
	case eOutputFormatSimple:
	case eOutputFormatHaploid:
	case eOutputFormatSC2Gears:
		switch(command)
		{
		case eProtossCommandBuildNexus:
			output = wxT("Nexus");
			break;
		case eProtossCommandBuildPylon:
			output = wxT("Pylon");
			break;
		case eProtossCommandBuildAssimilator:
			output = wxT("Assimilator");
			break;
		case eProtossCommandBuildGateway:
			output = wxT("Gateway");
			break;
		case eProtossCommandConvertGatewayToWarpGate:
			output = wxT("Convert Gateway To Warp Gate");
			break;
		case eProtossCommandBuildForge:
			output = wxT("Forge");
			break;
		case eProtossCommandBuildPhotonCannon:
			output = wxT("Photon Cannon");
			break;
		case eProtossCommandBuildCyberneticsCore:
			output = wxT("Cybernetics Core");
			break;
		case eProtossCommandBuildTwilightCouncil:
			output = wxT("Twilight Council");
			break;
		case eProtossCommandBuildTemplarArchives:
			output = wxT("Templar Archives");
			break;
		case eProtossCommandBuildDarkShrine:
			output = wxT("Dark Shrine");
			break;
		case eProtossCommandBuildRoboticsFacility:
			output = wxT("Robotics Facility");
			break;
		case eProtossCommandBuildRoboticsBay:
			output = wxT("Robotics Bay");
			break;
		case eProtossCommandBuildStargate:
			output = wxT("Stargate");
			break;
		case eProtossCommandBuildFleetBeacon:
			output = wxT("Fleet Beacon");
			break;

		case eProtossCommandBuildProbe:
			output = wxT("Probe");
			break;
		case eProtossCommandBuildZealot:
			output = wxT("Zealot");
			break;
		case eProtossCommandBuildStalker:
			output = wxT("Stalker");
			break;
		case eProtossCommandBuildSentry:
			output = wxT("Sentry");
			break;
		case eProtossCommandBuildHighTemplar:
			output = wxT("High Templar");
			break;
		case eProtossCommandBuildDarkTemplar:
			output = wxT("Dark Templar");
			break;
		case eProtossCommandBuildArchon2HT:
		case eProtossCommandBuildArchon1HT1DT:
		case eProtossCommandBuildArchon2DT:
			output = wxT("Archon");
			break;
		case eProtossCommandBuildWarpPrism:
			output = wxT("Warp Prism");
			break;
		case eProtossCommandBuildObserver:
			output = wxT("Observer");
			break;
		case eProtossCommandBuildImmortal:
			output = wxT("Immortal");
			break;
		case eProtossCommandBuildColossus:
			output = wxT("Colossus");
			break;
		case eProtossCommandBuildPhoenix:
			output = wxT("Phoenix");
			break;
		case eProtossCommandBuildVoidRay:
			output = wxT("Void Ray");
			break;
		case eProtossCommandBuildCarrier:
			output = wxT("Carrier");
			break;
		case eProtossCommandBuildMothership:
			output = wxT("Mothership");
			break;

		case eProtossCommandChronoNexus:
			output = wxT("Chrono Nexus");
			break;
		case eProtossCommandChronoGateway:
			output = wxT("Chrono Gateway");
			break;
		case eProtossCommandChronoWarpGate:
			output = wxT("Chrono Warp Gate");
			break;
		case eProtossCommandChronoForge:
			output = wxT("Chrono Forge");
			break;
		case eProtossCommandChronoCyberneticsCore:
			output = wxT("Chrono Cybernetics Core");
			break;
		case eProtossCommandChronoTwilightCouncil:
			output = wxT("Chrono Twilight Council");
			break;
		case eProtossCommandChronoTemplarArchives:
			output = wxT("Chrono Templar Archives");
			break;
		case eProtossCommandChronoRoboticsFacility:
			output = wxT("Chrono Robotics Facility");
			break;
		case eProtossCommandChronoRoboticsBay:
			output = wxT("Chrono Robotics Bay");
			break;
		case eProtossCommandChronoStargate:
			output = wxT("Chrono Stargate");
			break;
		case eProtossCommandChronoFleetBeacon:
			output = wxT("Chrono Fleet Beacon");
			break;

		case eProtossCommandResearchGroundWeapons1:
			output = wxT("Ground Weapons 1");
			break;
		case eProtossCommandResearchGroundWeapons2:
			output = wxT("Ground Weapons 2");
			break;
		case eProtossCommandResearchGroundWeapons3:
			output = wxT("Ground Weapons 3");
			break;
		case eProtossCommandResearchGroundArmor1:
			output = wxT("Ground Armor 1");
			break;
		case eProtossCommandResearchGroundArmor2:
			output = wxT("Ground Armor 2");
			break;
		case eProtossCommandResearchGroundArmor3:
			output = wxT("Ground Armor 3");
			break;
		case eProtossCommandResearchShields1:
			output = wxT("Shields 1");
			break;
		case eProtossCommandResearchShields2:
			output = wxT("Shields 2");
			break;
		case eProtossCommandResearchShields3:
			output = wxT("Shields 3");
			break;
		case eProtossCommandResearchAirWeapons1:
			output = wxT("Air Weapons 1");
			break;
		case eProtossCommandResearchAirWeapons2:
			output = wxT("Air Weapons 2");
			break;
		case eProtossCommandResearchAirWeapons3:
			output = wxT("Air Weapons 3");
			break;
		case eProtossCommandResearchAirArmor1:
			output = wxT("Air Armor 1");
			break;
		case eProtossCommandResearchAirArmor2:
			output = wxT("Air Armor 2");
			break;
		case eProtossCommandResearchAirArmor3:
			output = wxT("Air Armor 3");
			break;
		case eProtossCommandResearchWarpGateTransformation:
			output = wxT("Warp Gate Transformation");
			break;
		case eProtossCommandResearchHallucination:
			output = wxT("Hallucination");
			break;
		case eProtossCommandResearchCharge:
			output = wxT("Charge");
			break;
		case eProtossCommandResearchBlink:
			output = wxT("Blink");
			break;
		case eProtossCommandResearchPsionicStorm:
			output = wxT("Psionic Storm");
			break;
		case eProtossCommandResearchKhaydarinAmulet:
			output = wxT("Khaydarin Amulet");
			break;
		case eProtossCommandResearchGraviticDrive:
			output = wxT("Gravitic Drive");
			break;
		case eProtossCommandResearchGraviticBoosters:
			output = wxT("Gravitic Boosters");
			break;
		case eProtossCommandResearchExtendedThermalLance:
			output = wxT("Extended Thermal Lance");
			break;
		case eProtossCommandResearchFluxVanes:
			output = wxT("Flux Vanes");
			break;
		case eProtossCommandResearchAnionPulseCrystals:
			output = wxT("Anion Pulse Crystals");
			break;
		case eProtossCommandResearchGravitonCatapult:
			output = wxT("Graviton Catapult");
			break;

		case eProtossCommandMoveProbeToGas:
			if(eOutputFormatHaploid == format)
				output = wxT("+1 on gas");
			else
				output = wxT("Move Probe To Gas");
			break;
		case eProtossCommandMoveProbeToMinerals:
			if(eOutputFormatHaploid == format)
				output = wxT("+1 off gas");
			else
				output = wxT("Move Probe To Minerals");
			break;
		case eProtossCommandMoveThreeProbesToGas:
			if(eOutputFormatHaploid == format)
				output = wxT("+3 on gas");
			else
				output = wxT("Move Three Probes To Gas");
			break;
		case eProtossCommandMoveThreeProbesToMinerals:
			if(eOutputFormatHaploid == format)
				output = wxT("+3 off gas");
			else
				output = wxT("Move Three Probes To Minerals");
			break;
		case eProtossCommandWaitOneSecond:
			output = wxString::Format("Wait %us", count);
			break;


		case eTerranCommandBuildCommandCenter:
			output = wxT("Command Center");
			break;
		case eTerranCommandBuildRefinery:
			output = wxT("Refinery");
			break;
		case eTerranCommandBuildSupplyDepot:
			output = wxT("Supply Depot");
			break;
		case eTerranCommandBuildBarracksNaked:
			output = wxT("Barracks (Naked)");
			break;
		case eTerranCommandBuildBarracksOnTechLab:
			output = wxT("Barracks (on Tech Lab)");
			break;
		case eTerranCommandBuildBarracksOnReactor:
			output = wxT("Barracks (on Reactor)");
			break;
		case eTerranCommandBuildOrbitalCommand:
			output = wxT("Orbital Command");
			break;
		case eTerranCommandBuildEngineeringBay:
			output = wxT("Engineering Bay");
			break;
		case eTerranCommandBuildBunker:
			output = wxT("Bunker");
			break;
		case eTerranCommandBuildMissileTurret:
			output = wxT("Missile Turret");
			break;
		case eTerranCommandBuildSensorTower:
			output = wxT("Sensor Tower");
			break;
		case eTerranCommandBuildPlanetaryFortress:
			output = wxT("Planetary Fortress");
			break;
		case eTerranCommandBuildGhostAcademy:
			output = wxT("Ghost Academy");
			break;
		case eTerranCommandBuildFactoryNaked:
			output = wxT("Factory (Naked)");
			break;
		case eTerranCommandBuildFactoryOnTechLab:
			output = wxT("Factory (on Tech Lab)");
			break;
		case eTerranCommandBuildFactoryOnReactor:
			output = wxT("Factory (on Reactor)");
			break;
		case eTerranCommandBuildArmory:
			output = wxT("Armory");
			break;
		case eTerranCommandBuildStarportNaked:
			output = wxT("Starport (Naked)");
			break;
		case eTerranCommandBuildStarportOnTechLab:
			output = wxT("Starport (on Tech Lab)");
			break;
		case eTerranCommandBuildStarportOnReactor:
			output = wxT("Starport (on Reactor)");
			break;
		case eTerranCommandBuildFusionCore:
			output = wxT("Fusion Core");
			break;

		case eTerranCommandBuildBarracksTechLab:
			output = wxT("Barracks Tech Lab");
			break;
		case eTerranCommandBuildBarracksReactor:
			output = wxT("Barracks Reactor");
			break;
		case eTerranCommandBuildFactoryTechLab:
			output = wxT("Factory Tech Lab");
			break;
		case eTerranCommandBuildFactoryReactor:
			output = wxT("Factory Reactor");
			break;
		case eTerranCommandBuildStarportTechLab:
			output = wxT("Starport Tech Lab");
			break;
		case eTerranCommandBuildStarportReactor:
			output = wxT("Starport Reactor");
			break;

		case eTerranCommandLiftBarracksTechLab:
			output = wxT("Lift Barracks (Tech Lab)");
			break;
		case eTerranCommandLiftBarracksReactor:
			output = wxT("Lift Barracks (Reactor)");
			break;
		case eTerranCommandLiftBarracksNaked:
			output = wxT("Lift Barracks (Naked)");
			break;
		case eTerranCommandLiftFactoryTechLab:
			output = wxT("Lift Factory (Tech Lab)");
			break;
		case eTerranCommandLiftFactoryReactor:
			output = wxT("Lift Factory (Reactor)");
			break;
		case eTerranCommandLiftFactoryNaked:
			output = wxT("Lift Factory (Naked)");
			break;
		case eTerranCommandLiftStarportTechLab:
			output = wxT("Lift Starport (Tech Lab)");
			break;
		case eTerranCommandLiftStarportReactor:
			output = wxT("Lift Starport (Reactor)");
			break;
		case eTerranCommandLiftStarportNaked:
			output = wxT("Lift Starport (Naked)");
			break;

		case eTerranCommandLandBarracksTechLab:
			output = wxT("Land Barracks (Tech Lab)");
			break;
		case eTerranCommandLandBarracksReactor:
			output = wxT("Land Barracks (Reactor)");
			break;
		case eTerranCommandLandBarracksNaked:
			output = wxT("Land Barracks (Naked)");
			break;
		case eTerranCommandLandFactoryTechLab:
			output = wxT("Land Factory (Tech Lab)");
			break;
		case eTerranCommandLandFactoryReactor:
			output = wxT("Land Factory (Reactor)");
			break;
		case eTerranCommandLandFactoryNaked:
			output = wxT("Land Factory (Naked)");
			break;
		case eTerranCommandLandStarportTechLab:
			output = wxT("Land Starport (Tech Lab)");
			break;
		case eTerranCommandLandStarportReactor:
			output = wxT("Land Starport (Reactor)");
			break;
		case eTerranCommandLandStarportNaked:
			output = wxT("Land Starport (Naked)");
			break;

		case eTerranCommandSalvageBunker:
			output = wxT("Salvage Bunker");
			break;

		case eTerranCommandBuildSCV:
			output = wxT("SCV");
			break;
		case eTerranCommandBuildMarine:
			output = wxT("Marine");
			break;
		case eTerranCommandBuildMarineNaked:
			output = wxT("Marine (@ Naked Barracks)");
			break;
		case eTerranCommandBuildMarineReactor:
			output = wxT("Marine (@ Reactor Barracks)");
			break;
		case eTerranCommandBuildMarineTechLab:
			output = wxT("Marine (@ Tech Lab Barracks)");
			break;
		case eTerranCommandBuildMarauder:
			output = wxT("Marauder");
			break;
		case eTerranCommandBuildReaper:
			output = wxT("Reaper");
			break;
		case eTerranCommandBuildGhost:
			output = wxT("Ghost");
			break;
		case eTerranCommandBuildHellion:
			output = wxT("Hellion");
			break;
		case eTerranCommandBuildHellionNaked:
			output = wxT("Hellion (@ Naked Factory)");
			break;
		case eTerranCommandBuildHellionReactor:
			output = wxT("Hellion (@ Reactor Factory)");
			break;
		case eTerranCommandBuildHellionTechLab:
			output = wxT("Hellion (@ Tech Lab Factory)");
			break;
		case eTerranCommandBuildSiegeTank:
			output = wxT("Siege Tank");
			break;
		case eTerranCommandBuildThor:
			output = wxT("Thor");
			break;
		case eTerranCommandBuildViking:
			output = wxT("Viking");
			break;
		case eTerranCommandBuildVikingNaked:
			output = wxT("Viking (@ Naked Starport)");
			break;
		case eTerranCommandBuildVikingReactor:
			output = wxT("Viking (@ Reactor Starport)");
			break;
		case eTerranCommandBuildVikingTechLab:
			output = wxT("Viking (@ Tech Lab Starport)");
			break;
		case eTerranCommandBuildMedivac:
			output = wxT("Medivac");
			break;
		case eTerranCommandBuildMedivacNaked:
			output = wxT("Medivac (@ Naked Starport)");
			break;
		case eTerranCommandBuildMedivacReactor:
			output = wxT("Medivac (@ Reactor Starport)");
			break;
		case eTerranCommandBuildMedivacTechLab:
			output = wxT("Medivac (@ Tech Lab Starport)");
			break;
		case eTerranCommandBuildRaven:
			output = wxT("Raven");
			break;
		case eTerranCommandBuildBanshee:
			output = wxT("Banshee");
			break;
		case eTerranCommandBuildBattleCruiser:
			output = wxT("Battle Cruiser");
			break;

		case eTerranCommandCalldownMULE:
			output = wxT("Calldown MULE");
			break;
		case eTerranCommandCalldownExtraSupplies:
			output = wxT("Calldown Extra Supplies");
			break;
		case eTerranCommandScannerSweep:
			output = wxT("Scanner Sweep");
			break;

		case eTerranCommandResearchStimpack:
			output = wxT("Stimpack");
			break;
		case eTerranCommandResearchCombatShield:
			output = wxT("Combat Shield");
			break;
		case eTerranCommandResearchNitroPacks:
			output = wxT("Nitro Packs");
			break;
			break;
		case eTerranCommandResearchConcussiveShells:
			output = wxT("Concussive Shells");
			break;
		case eTerranCommandResearchInfantryWeapons1:
			output = wxT("Infantry Weapons 1");
			break;
		case eTerranCommandResearchInfantryWeapons2:
			output = wxT("Infantry Weapons 2");
			break;
		case eTerranCommandResearchInfantryWeapons3:
			output = wxT("Infantry Weapons 3");
			break;
		case eTerranCommandResearchInfantryArmor1:
			output = wxT("Infantry Armor 1");
			break;
		case eTerranCommandResearchInfantryArmor2:
			output = wxT("Infantry Armor 2");
			break;
		case eTerranCommandResearchInfantryArmor3:
			output = wxT("Infantry Armor 3");
			break;
		case eTerranCommandResearchBuildingArmor:
			output = wxT("Building Armor");
			break;
		case eTerranCommandResearchHiSecAutoTracking:
			output = wxT("Hi-Sec Auto Tracking");
			break;
		case eTerranCommandResearchNeoSteelFrame:
			output = wxT("Neo-steel Frame");
			break;
		case eTerranCommandResearchMoebiusReactor:
			output = wxT("Moebius Reactor");
			break;
		case eTerranCommandResearchPersonalCloaking:
			output = wxT("Personal Cloaking");
			break;
		case eTerranCommandArmNuke:
			output = wxT("Arm Nuke");
			break;
		case eTerranCommandResearchInfernalPreIgniter:
			output = wxT("Infernal Pre-igniter");
			break;
		case eTerranCommandResearchSiegeTech:
			output = wxT("Siege Tech");
			break;
		case eTerranCommandResearch250mmStrikeCannons:
			output = wxT("250mm Strike Cannons");
			break;
		case eTerranCommandResearchVehicleWeapons1:
			output = wxT("Vehicle Weapons 1");
			break;
		case eTerranCommandResearchVehicleWeapons2:
			output = wxT("Vehicle Weapons 2");
			break;
		case eTerranCommandResearchVehicleWeapons3:
			output = wxT("Vehicle Weapons 3");
			break;
		case eTerranCommandResearchVehiclePlating1:
			output = wxT("Vehicle Plating 1");
			break;
		case eTerranCommandResearchVehiclePlating2:
			output = wxT("Vehicle Plating 2");
			break;
		case eTerranCommandResearchVehiclePlating3:
			output = wxT("Vehicle Plating 3");
			break;
		case eTerranCommandResearchShipWeapons1:
			output = wxT("Ship Weapons 1");
			break;
		case eTerranCommandResearchShipWeapons2:
			output = wxT("Ship Weapons 2");
			break;
		case eTerranCommandResearchShipWeapons3:
			output = wxT("Ship Weapons 3");
			break;
		case eTerranCommandResearchShipPlating1:
			output = wxT("Ship Plating 1");
			break;
		case eTerranCommandResearchShipPlating2:
			output = wxT("Ship Plating 2");
			break;
		case eTerranCommandResearchShipPlating3:
			output = wxT("Ship Plating 3");
			break;
		case eTerranCommandResearchDurableMaterials:
			output = wxT("Durable Materials");
			break;
		case eTerranCommandResearchCorvidReactor:
			output = wxT("Corvid Reactor");
			break;
		case eTerranCommandResearchCaduceusReactor:
			output = wxT("Caduceus Reactor");
			break;
		case eTerranCommandResearchSeekerMissile:
			output = wxT("Seeker Missile");
			break;
		case eTerranCommandResearchCloakingField:
			output = wxT("Cloaking Field");
			break;
		case eTerranCommandResearchBehemothReactor:
			output = wxT("Behemoth Reactor");
			break;
		case eTerranCommandResearchWeaponRefit:
			output = wxT("Weapon Refit");
			break;

		case eTerranCommandMoveSCVToGas:
			if(eOutputFormatHaploid == format)
				output = wxT("+1 on gas");
			else
				output = wxT("Move SCV To Gas");
			break;
		case eTerranCommandMoveSCVToMinerals:
			if(eOutputFormatHaploid == format)
				output = wxT("+1 off gas");
			else
				output = wxT("Move SCV To Minerals");
			break;
		case eTerranCommandMoveThreeSCVsToGas:
			if(eOutputFormatHaploid == format)
				output = wxT("+3 on gas");
			else
				output = wxT("Move Three SCVs To Gas");
			break;
		case eTerranCommandMoveThreeSCVsToMinerals:
			if(eOutputFormatHaploid == format)
				output = wxT("+3 off gas");
			else
				output = wxT("Move Three SCVs To Minerals");
			break;
		case eTerranCommandWaitOneSecond:
			output = wxString::Format("Wait %us", count);
			break;


		case eZergCommandBuildHatchery:
			output = wxT("Hatchery");
			break;
		case eZergCommandBuildExtractor:
			output = wxT("Extractor");
			break;
		case eZergCommandBuildSpawningPool:
			output = wxT("Spawning Pool");
			break;
		case eZergCommandExpandCreepTumor:
			output = wxT("Expand Creep Tumor");
			break;
		case eZergCommandBuildEvolutionChamber:
			output = wxT("Evolution Chamber");
			break;
		case eZergCommandBuildSpineCrawler:
			output = wxT("Spine Crawler");
			break;
		case eZergCommandBuildSporeCrawler:
			output = wxT("Spore Crawler");
			break;
		case eZergCommandBuildRoachWarren:
			output = wxT("Roach Warren");
			break;
		case eZergCommandBuildLair:
			output = wxT("Lair");
			break;
		case eZergCommandBuildHydraliskDen:
			output = wxT("Hydralisk Den");
			break;
		case eZergCommandBuildBanelingNest:
			output = wxT("Baneling Nest");
			break;
		case eZergCommandBuildSpire:
			output = wxT("Spire");
			break;
		case eZergCommandBuildInfestationPit:
			output = wxT("Infestation Pit");
			break;
		case eZergCommandBuildNydusNetwork:
			output = wxT("Nydus Network");
			break;
		case eZergCommandBuildHive:
			output = wxT("Hive");
			break;
		case eZergCommandBuildUltraliskCavern:
			output = wxT("Ultralisk Cavern");
			break;
		case eZergCommandBuildGreaterSpire:
			output = wxT("Greater Spire");
			break;

		case eZergCommandCancelExtractor:
			output = wxT("Cancel Extractor");
			break;

		case eZergCommandBuildDrone:
			output = wxT("Drone");
			break;
		case eZergCommandBuildOverlord:
			output = wxT("Overlord");
			break;
		case eZergCommandBuildQueen:
			output = wxT("Queen");
			break;
		case eZergCommandBuildZergling:
			output = wxT("Zergling");
			break;
		case eZergCommandBuildRoach:
			output = wxT("Roach");
			break;
		case eZergCommandBuildBaneling:
			output = wxT("Baneling");
			break;
		case eZergCommandBuildHydralisk:
			output = wxT("Hydralisk");
			break;
		case eZergCommandBuildOverseer:
			output = wxT("Overseer");
			break;
		case eZergCommandBuildInfestor:
			output = wxT("Infestor");
			break;
		case eZergCommandBuildMutalisk:
			output = wxT("Mutalisk");
			break;
		case eZergCommandBuildCorruptor:
			output = wxT("Corruptor");
			break;
		case eZergCommandBuildUltralisk:
			output = wxT("Ultralisk");
			break;
		case eZergCommandBuildBroodlord:
			output = wxT("Broodlord");
			break;

		case eZergCommandExtractorTrickDrone:
			output = wxT("Extractor Trick");
			break;

		case eZergCommandQueenSpawnCreepTumor:
			output = wxT("Creep Tumor");
			break;
		case eZergCommandQueenSpawnLarvae:
			output = wxT("Spawn Larvae");
			break;

		case eZergCommandResearchAdrenalGlands:
			output = wxT("Adrenal Glands");
			break;
		case eZergCommandResearchMetabolicBoost:
			output = wxT("Metabolic Boost");
			break;
		case eZergCommandResearchMeleeAttacks1:
			output = wxT("Melee Attacks 1");
			break;
		case eZergCommandResearchMeleeAttacks2:
			output = wxT("Melee Attacks 2");
			break;
		case eZergCommandResearchMeleeAttacks3:
			output = wxT("Melee Attacks 3");
			break;
		case eZergCommandResearchGroundCarapace1:
			output = wxT("Ground Carapace 1");
			break;
		case eZergCommandResearchGroundCarapace2:
			output = wxT("Ground Carapace 2");
			break;
		case eZergCommandResearchGroundCarapace3:
			output = wxT("Ground Carapace 3");
			break;
		case eZergCommandResearchMissileAttacks1:
			output = wxT("Missile Attacks 1");
			break;
		case eZergCommandResearchMissileAttacks2:
			output = wxT("Missile Attacks 2");
			break;
		case eZergCommandResearchMissileAttacks3:
			output = wxT("Missile Attacks 3");
			break;
		case eZergCommandResearchGlialReconstitution:
			output = wxT("Glial Reconstitution");
			break;
		case eZergCommandResearchTunnelingClaws:
			output = wxT("Tunneling Claws");
			break;
		case eZergCommandResearchCentrifugalHooks:
			output = wxT("Centrifugal Hooks");
			break;
		case eZergCommandResearchBurrowAtHatchery:
		case eZergCommandResearchBurrowAtLair:
		case eZergCommandResearchBurrowAtHive:
			output = wxT("Burrow");
			break;
		case eZergCommandResearchPneumaticCarapaceAtHatchery:
		case eZergCommandResearchPneumaticCarapaceAtLair:
		case eZergCommandResearchPneumaticCarapaceAtHive:
			output = wxT("Pneumatic Carapace");
			break;
		case eZergCommandResearchVentralSacsAtHatchery:
		case eZergCommandResearchVentralSacsAtLair:
		case eZergCommandResearchVentralSacsAtHive:
			output = wxT("Ventral Sacs");
			break;
		case eZergCommandResearchGroovedSpines:
			output = wxT("Grooved Spines");
			break;
		case eZergCommandResearchPathogenGlands:
			output = wxT("Pathogen Glands");
			break;
		case eZergCommandResearchNeuralParasite:
			output = wxT("Neural Parasite");
			break;
		case eZergCommandResearchFlyerAttacks1:
			output = wxT("Flyer Attacks 1");
			break;
		case eZergCommandResearchFlyerAttacks2:
			output = wxT("Flyer Attacks 2");
			break;
		case eZergCommandResearchFlyerAttacks3:
			output = wxT("Flyer Attacks 3");
			break;
		case eZergCommandResearchFlyerCarapace1:
			output = wxT("Flyer Carapace 1");
			break;
		case eZergCommandResearchFlyerCarapace2:
			output = wxT("Flyer Carapace 2");
			break;
		case eZergCommandResearchFlyerCarapace3:
			output = wxT("Flyer Carapace 3");
			break;
		case eZergCommandResearchChitinousPlating:
			output = wxT("Chitinous Plating");
			break;

		case eZergCommandMoveDroneToGas:
			if(eOutputFormatHaploid == format)
				output = wxT("+1 on gas");
			else
				output = wxT("Move Drone To Gas");
			break;
		case eZergCommandMoveDroneToMinerals:
			if(eOutputFormatHaploid == format)
				output = wxT("+1 off gas");
			else
				output = wxT("Move Drone To Minerals");
			break;
		case eZergCommandMoveThreeDronesToGas:
			if(eOutputFormatHaploid == format)
				output = wxT("+3 on gas");
			else
				output = wxT("Move Three Drones To Gas");
			break;
		case eZergCommandMoveThreeDronesToMinerals:
			if(eOutputFormatHaploid == format)
				output = wxT("+3 off gas");
			else
				output = wxT("Move Three Drones To Minerals");
			break;
		case eZergCommandWaitOneSecond:
			output = wxString::Format("Wait %us", count);
			break;
		}

		if(count > 1 && command != eProtossCommandWaitOneSecond && command != eTerranCommandWaitOneSecond && command != eZergCommandWaitOneSecond)
			output = wxString::Format("%u*%s", count, output);

		break;

	case eOutputFormatDetailed:
	case eOutputFormatFull:
		switch(command)
		{
		case eProtossCommandBuildNexus:
			output = wxT("Build Nexus");
			break;
		case eProtossCommandBuildPylon:
			output = wxT("Build Pylon");
			break;
		case eProtossCommandBuildAssimilator:
			output = wxT("Build Assimilator");
			break;
		case eProtossCommandBuildGateway:
			output = wxT("Build Gateway");
			break;
		case eProtossCommandConvertGatewayToWarpGate:
			output = wxT("Convert Gateway To Warp Gate");
			break;
		case eProtossCommandBuildForge:
			output = wxT("Build Forge");
			break;
		case eProtossCommandBuildPhotonCannon:
			output = wxT("Build Photon Cannon");
			break;
		case eProtossCommandBuildCyberneticsCore:
			output = wxT("Build Cybernetics Core");
			break;
		case eProtossCommandBuildTwilightCouncil:
			output = wxT("Build Twilight Council");
			break;
		case eProtossCommandBuildTemplarArchives:
			output = wxT("Build Templar Archives");
			break;
		case eProtossCommandBuildDarkShrine:
			output = wxT("Build Dark Shrine");
			break;
		case eProtossCommandBuildRoboticsFacility:
			output = wxT("Build Robotics Facility");
			break;
		case eProtossCommandBuildRoboticsBay:
			output = wxT("Build Robotics Bay");
			break;
		case eProtossCommandBuildStargate:
			output = wxT("Build Stargate");
			break;
		case eProtossCommandBuildFleetBeacon:
			output = wxT("Build Fleet Beacon");
			break;

		case eProtossCommandBuildProbe:
			output = wxT("Build Probe");
			break;
		case eProtossCommandBuildZealot:
			output = wxT("Build Zealot");
			break;
		case eProtossCommandBuildStalker:
			output = wxT("Build Stalker");
			break;
		case eProtossCommandBuildSentry:
			output = wxT("Build Sentry");
			break;
		case eProtossCommandBuildHighTemplar:
			output = wxT("Build High Templar");
			break;
		case eProtossCommandBuildDarkTemplar:
			output = wxT("Build Dark Templar");
			break;
		case eProtossCommandBuildArchon2HT:
			output = wxT("Build Archon (2 HTs)");
			break;
		case eProtossCommandBuildArchon1HT1DT:
			output = wxT("Build Archon (1HT 1DT)");
			break;
		case eProtossCommandBuildArchon2DT:
			output = wxT("Build Archon (2 DTs)");
			break;
		case eProtossCommandBuildWarpPrism:
			output = wxT("Build Warp Prism");
			break;
		case eProtossCommandBuildObserver:
			output = wxT("Build Observer");
			break;
		case eProtossCommandBuildImmortal:
			output = wxT("Build Immortal");
			break;
		case eProtossCommandBuildColossus:
			output = wxT("Build Colossus");
			break;
		case eProtossCommandBuildPhoenix:
			output = wxT("Build Phoenix");
			break;
		case eProtossCommandBuildVoidRay:
			output = wxT("Build Void Ray");
			break;
		case eProtossCommandBuildCarrier:
			output = wxT("Build Carrier");
			break;
		case eProtossCommandBuildMothership:
			output = wxT("Build Mothership");
			break;

		case eProtossCommandChronoNexus:
			output = wxT("Chrono Nexus");
			break;
		case eProtossCommandChronoGateway:
			output = wxT("Chrono Gateway");
			break;
		case eProtossCommandChronoWarpGate:
			output = wxT("Chrono Warp Gate");
			break;
		case eProtossCommandChronoForge:
			output = wxT("Chrono Forge");
			break;
		case eProtossCommandChronoCyberneticsCore:
			output = wxT("Chrono Cybernetics Core");
			break;
		case eProtossCommandChronoTwilightCouncil:
			output = wxT("Chrono Twilight Council");
			break;
		case eProtossCommandChronoTemplarArchives:
			output = wxT("Chrono Templar Archives");
			break;
		case eProtossCommandChronoRoboticsFacility:
			output = wxT("Chrono Robotics Facility");
			break;
		case eProtossCommandChronoRoboticsBay:
			output = wxT("Chrono Robotics Bay");
			break;
		case eProtossCommandChronoStargate:
			output = wxT("Chrono Stargate");
			break;
		case eProtossCommandChronoFleetBeacon:
			output = wxT("Chrono Fleet Beacon");
			break;

		case eProtossCommandResearchGroundWeapons1:
			output = wxT("Research Ground Weapons 1");
			break;
		case eProtossCommandResearchGroundWeapons2:
			output = wxT("Research Ground Weapons 2");
			break;
		case eProtossCommandResearchGroundWeapons3:
			output = wxT("Research Ground Weapons 3");
			break;
		case eProtossCommandResearchGroundArmor1:
			output = wxT("Research Ground Armor 1");
			break;
		case eProtossCommandResearchGroundArmor2:
			output = wxT("Research Ground Armor 2");
			break;
		case eProtossCommandResearchGroundArmor3:
			output = wxT("Research Ground Armor 3");
			break;
		case eProtossCommandResearchShields1:
			output = wxT("Research Shields 1");
			break;
		case eProtossCommandResearchShields2:
			output = wxT("Research Shields 2");
			break;
		case eProtossCommandResearchShields3:
			output = wxT("Research Shields 3");
			break;
		case eProtossCommandResearchAirWeapons1:
			output = wxT("Research Air Weapons 1");
			break;
		case eProtossCommandResearchAirWeapons2:
			output = wxT("Research Air Weapons 2");
			break;
		case eProtossCommandResearchAirWeapons3:
			output = wxT("Research Air Weapons 3");
			break;
		case eProtossCommandResearchAirArmor1:
			output = wxT("Research Air Armor 1");
			break;
		case eProtossCommandResearchAirArmor2:
			output = wxT("Research Air Armor 2");
			break;
		case eProtossCommandResearchAirArmor3:
			output = wxT("Research Air Armor 3");
			break;
		case eProtossCommandResearchWarpGateTransformation:
			output = wxT("Research Warp Gate Transformation");
			break;
		case eProtossCommandResearchHallucination:
			output = wxT("Research Hallucination");
			break;
		case eProtossCommandResearchCharge:
			output = wxT("Research Charge");
			break;
		case eProtossCommandResearchBlink:
			output = wxT("Research Blink");
			break;
		case eProtossCommandResearchPsionicStorm:
			output = wxT("Research Psionic Storm");
			break;
		case eProtossCommandResearchKhaydarinAmulet:
			output = wxT("Research Khaydarin Amulet");
			break;
		case eProtossCommandResearchGraviticDrive:
			output = wxT("Research Gravitic Drive");
			break;
		case eProtossCommandResearchGraviticBoosters:
			output = wxT("Research Gravitic Boosters");
			break;
		case eProtossCommandResearchExtendedThermalLance:
			output = wxT("Research Extended Thermal Lance");
			break;
		case eProtossCommandResearchFluxVanes:
			output = wxT("Research Flux Vanes");
			break;
		case eProtossCommandResearchAnionPulseCrystals:
			output = wxT("Research Anion Pulse Crystals");
			break;
		case eProtossCommandResearchGravitonCatapult:
			output = wxT("Research Graviton Catapult");
			break;

		case eProtossCommandMoveProbeToGas:
			output = wxT("Move Probe To Gas");
			break;
		case eProtossCommandMoveProbeToMinerals:
			output = wxT("Move Probe To Minerals");
			break;
		case eProtossCommandMoveThreeProbesToGas:
			output = wxT("Move Three Probes To Gas");
			break;
		case eProtossCommandMoveThreeProbesToMinerals:
			output = wxT("Move Three Probes To Minerals");
			break;
		case eProtossCommandWaitOneSecond:
			output = wxString::Format("Wait %us", count);
			break;

		case eTerranCommandBuildCommandCenter:
			output = wxT("Build Command Center");
			break;
		case eTerranCommandBuildRefinery:
			output = wxT("Build Refinery");
			break;
		case eTerranCommandBuildSupplyDepot:
			output = wxT("Build Supply Depot");
			break;
		case eTerranCommandBuildBarracksNaked:
			output = wxT("Build Barracks (Naked)");
			break;
		case eTerranCommandBuildBarracksOnTechLab:
			output = wxT("Build Barracks (on Tech Lab)");
			break;
		case eTerranCommandBuildBarracksOnReactor:
			output = wxT("Build Barracks (on Reactor)");
			break;
		case eTerranCommandBuildOrbitalCommand:
			output = wxT("Build Orbital Command");
			break;
		case eTerranCommandBuildEngineeringBay:
			output = wxT("Build Engineering Bay");
			break;
		case eTerranCommandBuildBunker:
			output = wxT("Build Bunker");
			break;
		case eTerranCommandBuildMissileTurret:
			output = wxT("Build Missile Turret");
			break;
		case eTerranCommandBuildSensorTower:
			output = wxT("Build Sensor Tower");
			break;
		case eTerranCommandBuildPlanetaryFortress:
			output = wxT("Build Planetary Fortress");
			break;
		case eTerranCommandBuildGhostAcademy:
			output = wxT("Build Ghost Academy");
			break;
		case eTerranCommandBuildFactoryNaked:
			output = wxT("Build Factory (Naked)");
			break;
		case eTerranCommandBuildFactoryOnTechLab:
			output = wxT("Build Factory (on Tech Lab)");
			break;
		case eTerranCommandBuildFactoryOnReactor:
			output = wxT("Build Factory (on Reactor)");
			break;
		case eTerranCommandBuildArmory:
			output = wxT("Build Armory");
			break;
		case eTerranCommandBuildStarportNaked:
			output = wxT("Build Starport (Naked)");
			break;
		case eTerranCommandBuildStarportOnTechLab:
			output = wxT("Build Starport (on Tech Lab)");
			break;
		case eTerranCommandBuildStarportOnReactor:
			output = wxT("Build Starport (on Reactor)");
			break;
		case eTerranCommandBuildFusionCore:
			output = wxT("Build Fusion Core");
			break;

		case eTerranCommandBuildBarracksTechLab:
			output = wxT("Build Barracks Tech Lab");
			break;
		case eTerranCommandBuildBarracksReactor:
			output = wxT("Build Barracks Reactor");
			break;
		case eTerranCommandBuildFactoryTechLab:
			output = wxT("Build Factory Tech Lab");
			break;
		case eTerranCommandBuildFactoryReactor:
			output = wxT("Build Factory Reactor");
			break;
		case eTerranCommandBuildStarportTechLab:
			output = wxT("Build Starport Tech Lab");
			break;
		case eTerranCommandBuildStarportReactor:
			output = wxT("Build Starport Reactor");
			break;

		case eTerranCommandLiftBarracksTechLab:
			output = wxT("Lift Barracks (Tech Lab)");
			break;
		case eTerranCommandLiftBarracksReactor:
			output = wxT("Lift Barracks (Reactor)");
			break;
		case eTerranCommandLiftBarracksNaked:
			output = wxT("Lift Barracks (Naked)");
			break;
		case eTerranCommandLiftFactoryTechLab:
			output = wxT("Lift Factory (Tech Lab)");
			break;
		case eTerranCommandLiftFactoryReactor:
			output = wxT("Lift Factory (Reactor)");
			break;
		case eTerranCommandLiftFactoryNaked:
			output = wxT("Lift Factory (Naked)");
			break;
		case eTerranCommandLiftStarportTechLab:
			output = wxT("Lift Starport (Tech Lab)");
			break;
		case eTerranCommandLiftStarportReactor:
			output = wxT("Lift Starport (Reactor)");
			break;
		case eTerranCommandLiftStarportNaked:
			output = wxT("Lift Starport (Naked)");
			break;

		case eTerranCommandLandBarracksTechLab:
			output = wxT("Land Barracks (Tech Lab)");
			break;
		case eTerranCommandLandBarracksReactor:
			output = wxT("Land Barracks (Reactor)");
			break;
		case eTerranCommandLandBarracksNaked:
			output = wxT("Land Barracks (Naked)");
			break;
		case eTerranCommandLandFactoryTechLab:
			output = wxT("Land Factory (Tech Lab)");
			break;
		case eTerranCommandLandFactoryReactor:
			output = wxT("Land Factory (Reactor)");
			break;
		case eTerranCommandLandFactoryNaked:
			output = wxT("Land Factory (Naked)");
			break;
		case eTerranCommandLandStarportTechLab:
			output = wxT("Land Starport (Tech Lab)");
			break;
		case eTerranCommandLandStarportReactor:
			output = wxT("Land Starport (Reactor)");
			break;
		case eTerranCommandLandStarportNaked:
			output = wxT("Land Starport (Naked)");
			break;

		case eTerranCommandSalvageBunker:
			output = wxT("Salvage Bunker");
			break;

		case eTerranCommandBuildSCV:
			output = wxT("Build SCV");
			break;
		case eTerranCommandBuildMarine:
			output = wxT("Build Marine");
			break;
		case eTerranCommandBuildMarineNaked:
			output = wxT("Build Marine (@ Naked Barracks)");
			break;
		case eTerranCommandBuildMarineReactor:
			output = wxT("Build Marine (@ Reactor Barracks)");
			break;
		case eTerranCommandBuildMarineTechLab:
			output = wxT("Build Marine (@ Tech Lab Barracks)");
			break;
		case eTerranCommandBuildMarauder:
			output = wxT("Build Marauder");
			break;
		case eTerranCommandBuildReaper:
			output = wxT("Build Reaper");
			break;
		case eTerranCommandBuildGhost:
			output = wxT("Build Ghost");
			break;
		case eTerranCommandBuildHellion:
			output = wxT("Build Hellion");
			break;
		case eTerranCommandBuildHellionNaked:
			output = wxT("Build Hellion (@ Naked Factory)");
			break;
		case eTerranCommandBuildHellionReactor:
			output = wxT("Build Hellion (@ Reactor Factory)");
			break;
		case eTerranCommandBuildHellionTechLab:
			output = wxT("Build Hellion (@ Tech Lab Factory)");
			break;
		case eTerranCommandBuildSiegeTank:
			output = wxT("Build Siege Tank");
			break;
		case eTerranCommandBuildThor:
			output = wxT("Build Thor");
			break;
		case eTerranCommandBuildViking:
			output = wxT("Build Viking");
			break;
		case eTerranCommandBuildVikingNaked:
			output = wxT("Build Viking (@ Naked Starport)");
			break;
		case eTerranCommandBuildVikingReactor:
			output = wxT("Build Viking (@ Reactor Starport)");
			break;
		case eTerranCommandBuildVikingTechLab:
			output = wxT("Build Viking (@ Tech Lab Starport)");
			break;
		case eTerranCommandBuildMedivac:
			output = wxT("Build Medivac");
			break;
		case eTerranCommandBuildMedivacNaked:
			output = wxT("Build Medivac (@ Naked Starport)");
			break;
		case eTerranCommandBuildMedivacReactor:
			output = wxT("Build Medivac (@ Reactor Starport)");
			break;
		case eTerranCommandBuildMedivacTechLab:
			output = wxT("Build Medivac (@ Tech Lab Starport)");
			break;
		case eTerranCommandBuildRaven:
			output = wxT("Build Raven");
			break;
		case eTerranCommandBuildBanshee:
			output = wxT("Build Banshee");
			break;
		case eTerranCommandBuildBattleCruiser:
			output = wxT("Build Battle Cruiser");
			break;

		case eTerranCommandCalldownMULE:
			output = wxT("Calldown MULE");
			break;
		case eTerranCommandCalldownExtraSupplies:
			output = wxT("Calldown Extra Supplies");
			break;
		case eTerranCommandScannerSweep:
			output = wxT("Scanner Sweep");
			break;

		case eTerranCommandResearchStimpack:
			output = wxT("Research Stimpack");
			break;
		case eTerranCommandResearchCombatShield:
			output = wxT("Research Combat Shield");
			break;
		case eTerranCommandResearchNitroPacks:
			output = wxT("Research Nitro Packs");
			break;
		case eTerranCommandResearchConcussiveShells:
			output = wxT("Research Concussive Shells");
			break;
		case eTerranCommandResearchInfantryWeapons1:
			output = wxT("Research Infantry Weapons 1");
			break;
		case eTerranCommandResearchInfantryWeapons2:
			output = wxT("Research Infantry Weapons 2");
			break;
		case eTerranCommandResearchInfantryWeapons3:
			output = wxT("Research Infantry Weapons 3");
			break;
		case eTerranCommandResearchInfantryArmor1:
			output = wxT("Research Infantry Armor 1");
			break;
		case eTerranCommandResearchInfantryArmor2:
			output = wxT("Research Infantry Armor 2");
			break;
		case eTerranCommandResearchInfantryArmor3:
			output = wxT("Research Infantry Armor 3");
			break;
		case eTerranCommandResearchBuildingArmor:
			output = wxT("Research Building Armor");
			break;
		case eTerranCommandResearchHiSecAutoTracking:
			output = wxT("Research Hi-Sec Auto Tracking");
			break;
		case eTerranCommandResearchNeoSteelFrame:
			output = wxT("Research Neo-steel Frame");
			break;
		case eTerranCommandResearchMoebiusReactor:
			output = wxT("Research Moebius Reactor");
			break;
		case eTerranCommandResearchPersonalCloaking:
			output = wxT("Research Personal Cloaking");
			break;
		case eTerranCommandArmNuke:
			output = wxT("Arm Nuke");
			break;
		case eTerranCommandResearchInfernalPreIgniter:
			output = wxT("Research Infernal Pre-igniter");
			break;
		case eTerranCommandResearchSiegeTech:
			output = wxT("Research Siege Tech");
			break;
		case eTerranCommandResearch250mmStrikeCannons:
			output = wxT("Research 250mm Strike Cannons");
			break;
		case eTerranCommandResearchVehicleWeapons1:
			output = wxT("Research Vehicle Weapons 1");
			break;
		case eTerranCommandResearchVehicleWeapons2:
			output = wxT("Research Vehicle Weapons 2");
			break;
		case eTerranCommandResearchVehicleWeapons3:
			output = wxT("Research Vehicle Weapons 3");
			break;
		case eTerranCommandResearchVehiclePlating1:
			output = wxT("Research Vehicle Plating 1");
			break;
		case eTerranCommandResearchVehiclePlating2:
			output = wxT("Research Vehicle Plating 2");
			break;
		case eTerranCommandResearchVehiclePlating3:
			output = wxT("Research Vehicle Plating 3");
			break;
		case eTerranCommandResearchShipWeapons1:
			output = wxT("Research Ship Weapons 1");
			break;
		case eTerranCommandResearchShipWeapons2:
			output = wxT("Research Ship Weapons 2");
			break;
		case eTerranCommandResearchShipWeapons3:
			output = wxT("Research Ship Weapons 3");
			break;
		case eTerranCommandResearchShipPlating1:
			output = wxT("Research Ship Plating 1");
			break;
		case eTerranCommandResearchShipPlating2:
			output = wxT("Research Ship Plating 2");
			break;
		case eTerranCommandResearchShipPlating3:
			output = wxT("Research Ship Plating 3");
			break;
		case eTerranCommandResearchDurableMaterials:
			output = wxT("Research Durable Materials");
			break;
		case eTerranCommandResearchCorvidReactor:
			output = wxT("Research Corvid Reactor");
			break;
		case eTerranCommandResearchCaduceusReactor:
			output = wxT("Research Caduceus Reactor");
			break;
		case eTerranCommandResearchSeekerMissile:
			output = wxT("Research Seeker Missile");
			break;
		case eTerranCommandResearchCloakingField:
			output = wxT("Research Cloaking Field");
			break;
		case eTerranCommandResearchBehemothReactor:
			output = wxT("Research Behemoth Reactor");
			break;
		case eTerranCommandResearchWeaponRefit:
			output = wxT("Research Weapon Refit");
			break;

		case eTerranCommandMoveSCVToGas:
			output = wxT("Move SCV To Gas");
			break;
		case eTerranCommandMoveSCVToMinerals:
			output = wxT("Move SCV To Minerals");
			break;
		case eTerranCommandMoveThreeSCVsToGas:
			output = wxT("Move Three SCVs To Gas");
			break;
		case eTerranCommandMoveThreeSCVsToMinerals:
			output = wxT("Move Three SCVs To Minerals");
			break;
		case eTerranCommandWaitOneSecond:
			output = wxString::Format("Wait %us", count);
			break;


		case eZergCommandBuildHatchery:
			output = wxT("Build Hatchery");
			break;
		case eZergCommandBuildExtractor:
			output = wxT("Build Extractor");
			break;
		case eZergCommandBuildSpawningPool:
			output = wxT("Build Spawning Pool");
			break;
		case eZergCommandExpandCreepTumor:
			output = wxT("Expand Creep Tumor");
			break;
		case eZergCommandBuildEvolutionChamber:
			output = wxT("Build Evolution Chamber");
			break;
		case eZergCommandBuildSpineCrawler:
			output = wxT("Build Spine Crawler");
			break;
		case eZergCommandBuildSporeCrawler:
			output = wxT("Build Spore Crawler");
			break;
		case eZergCommandBuildRoachWarren:
			output = wxT("Build Roach Warren");
			break;
		case eZergCommandBuildLair:
			output = wxT("Build Lair");
			break;
		case eZergCommandBuildHydraliskDen:
			output = wxT("Build Hydralisk Den");
			break;
		case eZergCommandBuildBanelingNest:
			output = wxT("Build Baneling Nest");
			break;
		case eZergCommandBuildSpire:
			output = wxT("Build Spire");
			break;
		case eZergCommandBuildInfestationPit:
			output = wxT("Build Infestation Pit");
			break;
		case eZergCommandBuildNydusNetwork:
			output = wxT("Build Nydus Network");
			break;
		case eZergCommandBuildHive:
			output = wxT("Build Hive");
			break;
		case eZergCommandBuildUltraliskCavern:
			output = wxT("Build Ultralisk Cavern");
			break;
		case eZergCommandBuildGreaterSpire:
			output = wxT("Build Greater Spire");
			break;

		case eZergCommandCancelExtractor:
			output = wxT("Cancel Extractor");
			break;

		case eZergCommandBuildDrone:
			output = wxT("Build Drone");
			break;
		case eZergCommandBuildOverlord:
			output = wxT("Build Overlord");
			break;
		case eZergCommandBuildQueen:
			output = wxT("Build Queen");
			break;
		case eZergCommandBuildZergling:
			output = wxT("Build Zergling");
			break;
		case eZergCommandBuildRoach:
			output = wxT("Build Roach");
			break;
		case eZergCommandBuildBaneling:
			output = wxT("Build Baneling");
			break;
		case eZergCommandBuildHydralisk:
			output = wxT("Build Hydralisk");
			break;
		case eZergCommandBuildOverseer:
			output = wxT("Build Overseer");
			break;
		case eZergCommandBuildInfestor:
			output = wxT("Build Infestor");
			break;
		case eZergCommandBuildMutalisk:
			output = wxT("Build Mutalisk");
			break;
		case eZergCommandBuildCorruptor:
			output = wxT("Build Corruptor");
			break;
		case eZergCommandBuildUltralisk:
			output = wxT("Build Ultralisk");
			break;
		case eZergCommandBuildBroodlord:
			output = wxT("Build Broodlord");
			break;

		case eZergCommandExtractorTrickDrone:
			output = wxT("Extractor Trick Drone");
			break;

		case eZergCommandQueenSpawnCreepTumor:
			output = wxT("Queen Spawn Creep Tumor");
			break;
		case eZergCommandQueenSpawnLarvae:
			output = wxT("Queen Spawn Larvae");
			break;

		case eZergCommandResearchAdrenalGlands:
			output = wxT("Research Adrenal Glands");
			break;
		case eZergCommandResearchMetabolicBoost:
			output = wxT("Research Metabolic Boost");
			break;
		case eZergCommandResearchMeleeAttacks1:
			output = wxT("Research Melee Attacks 1");
			break;
		case eZergCommandResearchMeleeAttacks2:
			output = wxT("Research Melee Attacks 2");
			break;
		case eZergCommandResearchMeleeAttacks3:
			output = wxT("Research Melee Attacks 3");
			break;
		case eZergCommandResearchGroundCarapace1:
			output = wxT("Research Ground Carapace 1");
			break;
		case eZergCommandResearchGroundCarapace2:
			output = wxT("Research Ground Carapace 2");
			break;
		case eZergCommandResearchGroundCarapace3:
			output = wxT("Research Ground Carapace 3");
			break;
		case eZergCommandResearchMissileAttacks1:
			output = wxT("Research Missile Attacks 1");
			break;
		case eZergCommandResearchMissileAttacks2:
			output = wxT("Research Missile Attacks 2");
			break;
		case eZergCommandResearchMissileAttacks3:
			output = wxT("Research Missile Attacks 3");
			break;
		case eZergCommandResearchGlialReconstitution:
			output = wxT("Research Glial Reconstitution");
			break;
		case eZergCommandResearchTunnelingClaws:
			output = wxT("Research Tunneling Claws");
			break;
		case eZergCommandResearchCentrifugalHooks:
			output = wxT("Research Centrifugal Hooks");
			break;
		case eZergCommandResearchBurrowAtHatchery:
			output = wxT("Research Burrow (at Hatchery)");
			break;
		case eZergCommandResearchBurrowAtLair:
			output = wxT("Research Burrow (at Lair)");
			break;
		case eZergCommandResearchBurrowAtHive:
			output = wxT("Research Burrow (at Hive)");
			break;
		case eZergCommandResearchPneumaticCarapaceAtHatchery:
			output = wxT("Research Pneumatic Carapace (at Hatchery)");
			break;
		case eZergCommandResearchPneumaticCarapaceAtLair:
			output = wxT("Research Pneumatic Carapace (at Lair)");
			break;
		case eZergCommandResearchPneumaticCarapaceAtHive:
			output = wxT("Research Pneumatic Carapace (at Hive)");
			break;
		case eZergCommandResearchVentralSacsAtHatchery:
			output = wxT("Research Ventral Sacs (at Hatchery)");
			break;
		case eZergCommandResearchVentralSacsAtLair:
			output = wxT("Research Ventral Sacs (at Lair)");
			break;
		case eZergCommandResearchVentralSacsAtHive:
			output = wxT("Research Ventral Sacs (at Hive)");
			break;
		case eZergCommandResearchGroovedSpines:
			output = wxT("Research Grooved Spines");
			break;
		case eZergCommandResearchPathogenGlands:
			output = wxT("Research Pathogen Glands");
			break;
		case eZergCommandResearchNeuralParasite:
			output = wxT("Research Neural Parasite");
			break;
		case eZergCommandResearchFlyerAttacks1:
			output = wxT("Research Flyer Attacks 1");
			break;
		case eZergCommandResearchFlyerAttacks2:
			output = wxT("Research Flyer Attacks 2");
			break;
		case eZergCommandResearchFlyerAttacks3:
			output = wxT("Research Flyer Attacks 3");
			break;
		case eZergCommandResearchFlyerCarapace1:
			output = wxT("Research Flyer Carapace 1");
			break;
		case eZergCommandResearchFlyerCarapace2:
			output = wxT("Research Flyer Carapace 2");
			break;
		case eZergCommandResearchFlyerCarapace3:
			output = wxT("Research Flyer Carapace 3");
			break;
		case eZergCommandResearchChitinousPlating:
			output = wxT("Research Chitinous Plating");
			break;

		case eZergCommandMoveDroneToGas:
			output = wxT("Move Drone To Gas");
			break;
		case eZergCommandMoveDroneToMinerals:
			output = wxT("Move Drone To Minerals");
			break;
		case eZergCommandMoveThreeDronesToGas:
			output = wxT("Move Three Drones To Gas");
			break;
		case eZergCommandMoveThreeDronesToMinerals:
			output = wxT("Move Three Drones To Minerals");
			break;
		case eZergCommandWaitOneSecond:
			output = wxString::Format("Wait %us", count);
			break;
		}

		if(count > 1 && command != eProtossCommandWaitOneSecond && command != eTerranCommandWaitOneSecond && command != eZergCommandWaitOneSecond)
			output = wxString::Format("%u*%s", count, output);
		break;

	case eOutputFormatYABOT:
		// YABOT format codes provided by Turtle36 - thanks!!
		switch(command)
		{
		case eProtossCommandBuildNexus:
			output = wxT("0 25 0");
			break;
		case eProtossCommandBuildPylon:
			output = wxT("0 27 0");
			break;
		case eProtossCommandBuildAssimilator:
			output = wxT("0 19 0");
			break;
		case eProtossCommandBuildGateway:
			output = wxT("0 24 0");
			break;
		case eProtossCommandConvertGatewayToWarpGate:
			output = wxT("2 2 0");
			break;
		case eProtossCommandBuildForge:
			output = wxT("0 23 0");
			break;
		case eProtossCommandBuildPhotonCannon:
			output = wxT("0 26 0");
			break;
		case eProtossCommandBuildCyberneticsCore:
			output = wxT("0 20 0");
			break;
		case eProtossCommandBuildTwilightCouncil:
			output = wxT("0 32 0");
			break;
		case eProtossCommandBuildTemplarArchives:
			output = wxT("0 31 0");
			break;
		case eProtossCommandBuildDarkShrine:
			output = wxT("0 21 0");
			break;
		case eProtossCommandBuildRoboticsFacility:
			output = wxT("0 29 0");
			break;
		case eProtossCommandBuildRoboticsBay:
			output = wxT("0 28 0");
			break;
		case eProtossCommandBuildStargate:
			output = wxT("0 30 0");
			break;
		case eProtossCommandBuildFleetBeacon:
			output = wxT("0 22 0");
			break;

		case eProtossCommandBuildProbe:
			output = wxT("1 22 0");
			break;
		case eProtossCommandBuildZealot:
			output = wxT("1 26 0");
			break;
		case eProtossCommandBuildStalker:
			output = wxT("1 24 0");
			break;
		case eProtossCommandBuildSentry:
			output = wxT("1 23 0");
			break;
		case eProtossCommandBuildHighTemplar:
			output = wxT("1 17 0");
			break;
		case eProtossCommandBuildDarkTemplar:
			output = wxT("1 16 0");
			break;
		case eProtossCommandBuildArchon2HT:
		case eProtossCommandBuildArchon1HT1DT:
		case eProtossCommandBuildArchon2DT:
			output = wxT("1 13 0");
			break;
		case eProtossCommandBuildWarpPrism:
			output = wxT("1 39 0");
			break;
		case eProtossCommandBuildObserver:
			output = wxT("1 20 0");
			break;
		case eProtossCommandBuildImmortal:
			output = wxT("1 18 0");
			break;
		case eProtossCommandBuildColossus:
			output = wxT("1 15 0");
			break;
		case eProtossCommandBuildPhoenix:
			output = wxT("1 21 0");
			break;
		case eProtossCommandBuildVoidRay:
			output = wxT("1 25 0");
			break;
		case eProtossCommandBuildCarrier:
			output = wxT("1 14 0");
			break;
		case eProtossCommandBuildMothership:
			output = wxT("1 19 0");
			break;

		case eProtossCommandChronoNexus:
		case eProtossCommandChronoGateway:
		case eProtossCommandChronoWarpGate:
		case eProtossCommandChronoForge:
		case eProtossCommandChronoCyberneticsCore:
		case eProtossCommandChronoTwilightCouncil:
		case eProtossCommandChronoTemplarArchives:
		case eProtossCommandChronoRoboticsFacility:
		case eProtossCommandChronoRoboticsBay:
		case eProtossCommandChronoStargate:
		case eProtossCommandChronoFleetBeacon:
			output = wxT("");
			break;

		case eProtossCommandResearchGroundWeapons1:
			output = wxT("3 19 0");
			break;
		case eProtossCommandResearchGroundWeapons2:
			output = wxT("3 19 0");
			break;
		case eProtossCommandResearchGroundWeapons3:
			output = wxT("3 19 0");
			break;
		case eProtossCommandResearchGroundArmor1:
			output = wxT("3 18 0");
			break;
		case eProtossCommandResearchGroundArmor2:
			output = wxT("3 18 0");
			break;
		case eProtossCommandResearchGroundArmor3:
			output = wxT("3 18 0");
			break;
		case eProtossCommandResearchShields1:
			output = wxT("3 22 0");
			break;
		case eProtossCommandResearchShields2:
			output = wxT("3 22 0");
			break;
		case eProtossCommandResearchShields3:
			output = wxT("3 22 0");
			break;
		case eProtossCommandResearchAirWeapons1:
			output = wxT("3 21 0");
			break;
		case eProtossCommandResearchAirWeapons2:
			output = wxT("3 21 0");
			break;
		case eProtossCommandResearchAirWeapons3:
			output = wxT("3 21 0");
			break;
		case eProtossCommandResearchAirArmor1:
			output = wxT("3 20 0");
			break;
		case eProtossCommandResearchAirArmor2:
			output = wxT("3 20 0");
			break;
		case eProtossCommandResearchAirArmor3:
			output = wxT("3 20 0");
			break;
		case eProtossCommandResearchWarpGateTransformation:
			output = wxT("3 26 0");
			break;
		case eProtossCommandResearchHallucination:
			output = wxT("3 23 0");
			break;
		case eProtossCommandResearchCharge:
			output = wxT("3 27 0");
			break;
		case eProtossCommandResearchBlink:
			output = wxT("3 25 0");
			break;
		case eProtossCommandResearchPsionicStorm:
			output = wxT("3 24 0");
			break;
		case eProtossCommandResearchKhaydarinAmulet:
			output = wxT("3 48 0");
			break;
		case eProtossCommandResearchGraviticDrive:
			output = wxT("");
			break;
		case eProtossCommandResearchGraviticBoosters:
			output = wxT("");
			break;
		case eProtossCommandResearchExtendedThermalLance:
			output = wxT("3 47 0");
			break;
		case eProtossCommandResearchFluxVanes:
			output = wxT("");
			break;
		case eProtossCommandResearchAnionPulseCrystals:
			output = wxT("");
			break;
		case eProtossCommandResearchGravitonCatapult:
			output = wxT("");
			break;

		case eProtossCommandMoveProbeToGas:
		case eProtossCommandMoveProbeToMinerals:
		case eProtossCommandMoveThreeProbesToGas:
		case eProtossCommandMoveThreeProbesToMinerals:
			output = wxT("");
			break;
		case eProtossCommandWaitOneSecond:
			output = wxT("");
			break;

		case eTerranCommandBuildCommandCenter:
			return L"0 3 0";
		case eTerranCommandBuildRefinery:
			return L"0 12 0";
		case eTerranCommandBuildSupplyDepot:
			return L"0 15 0";
		case eTerranCommandBuildBarracksNaked:
		case eTerranCommandBuildBarracksOnTechLab:
		case eTerranCommandBuildBarracksOnReactor:
			return L"0 1 0";
		case eTerranCommandBuildOrbitalCommand:
			return L"2 0 0";
		case eTerranCommandBuildEngineeringBay:
			return L"0 4 0";
		case eTerranCommandBuildBunker:
			return L"0 2 0";
		case eTerranCommandBuildMissileTurret:
			return L"0 8 0";
		case eTerranCommandBuildSensorTower:
			return L"0 13 0";
		case eTerranCommandBuildPlanetaryFortress:
			return L"2 1 0";
		case eTerranCommandBuildGhostAcademy:
			return L"0 7 0";
		case eTerranCommandBuildFactoryNaked:
		case eTerranCommandBuildFactoryOnTechLab:
		case eTerranCommandBuildFactoryOnReactor:
			return L"0 5 0";
		case eTerranCommandBuildArmory:
			return L"0 0 0";
		case eTerranCommandBuildStarportNaked:
		case eTerranCommandBuildStarportOnTechLab:
		case eTerranCommandBuildStarportOnReactor:
			return L"0 14 0";
		case eTerranCommandBuildFusionCore:
			return L"0 6 0";

		case eTerranCommandBuildBarracksTechLab:
			return L"0 16 0";
		case eTerranCommandBuildFactoryTechLab:
			return L"0 17 0";
		case eTerranCommandBuildStarportTechLab:
			return L"0 18 0";
		case eTerranCommandBuildBarracksReactor:
			return L"0 9 0";
		case eTerranCommandBuildFactoryReactor:
			return L"0 10 0";
		case eTerranCommandBuildStarportReactor:
			return L"0 11 0";

		case eTerranCommandLiftBarracksTechLab:
		case eTerranCommandLiftBarracksReactor:
		case eTerranCommandLiftBarracksNaked:
		case eTerranCommandLiftFactoryTechLab:
		case eTerranCommandLiftFactoryReactor:
		case eTerranCommandLiftFactoryNaked:
		case eTerranCommandLiftStarportTechLab:
		case eTerranCommandLiftStarportReactor:
		case eTerranCommandLiftStarportNaked:
			return L"";

		case eTerranCommandLandBarracksTechLab:
		case eTerranCommandLandBarracksReactor:
		case eTerranCommandLandBarracksNaked:
		case eTerranCommandLandFactoryTechLab:
		case eTerranCommandLandFactoryReactor:
		case eTerranCommandLandFactoryNaked:
		case eTerranCommandLandStarportTechLab:
		case eTerranCommandLandStarportReactor:
		case eTerranCommandLandStarportNaked:
			return L"";

		case eTerranCommandBuildSCV:
			return L"1 9 0";
		case eTerranCommandBuildMarine:
		case eTerranCommandBuildMarineNaked:
		case eTerranCommandBuildMarineReactor:
		case eTerranCommandBuildMarineTechLab:
			return L"1 5 0";
		case eTerranCommandBuildMarauder:
			return L"1 4 0";
		case eTerranCommandBuildReaper:
			return L"1 8 0";
		case eTerranCommandBuildGhost:
			return L"1 2 0";
		case eTerranCommandBuildHellion:
		case eTerranCommandBuildHellionNaked:
		case eTerranCommandBuildHellionReactor:
		case eTerranCommandBuildHellionTechLab:
			return L"1 3 0";
		case eTerranCommandBuildSiegeTank:
			return L"1 10 0";
		case eTerranCommandBuildThor:
			return L"1 11 0";
		case eTerranCommandBuildViking:
		case eTerranCommandBuildVikingNaked:
		case eTerranCommandBuildVikingReactor:
		case eTerranCommandBuildVikingTechLab:
			return L"1 12 0";
		case eTerranCommandBuildMedivac:
		case eTerranCommandBuildMedivacNaked:
		case eTerranCommandBuildMedivacReactor:
		case eTerranCommandBuildMedivacTechLab:
			return L"1 6 0";
		case eTerranCommandBuildRaven:
			return L"1 7 0";
		case eTerranCommandBuildBanshee:
			return L"1 0 0";
		case eTerranCommandBuildBattleCruiser:
			return L"1 1 0";

		case eTerranCommandCalldownMULE:
			return L"";
		case eTerranCommandCalldownExtraSupplies:
			return L"";
		case eTerranCommandScannerSweep:
			return L"";

		case eTerranCommandResearchStimpack:
			return L"3 11 0";
		case eTerranCommandResearchCombatShield:
			return L"3 16 0";
		case eTerranCommandResearchNitroPacks:
			return L"3 17 0";
		case eTerranCommandResearchConcussiveShells:
			return L"3 15 0";
		case eTerranCommandResearchInfantryWeapons1:
			return L"3 2 0";
		case eTerranCommandResearchInfantryWeapons2:
			return L"3 2 0";
		case eTerranCommandResearchInfantryWeapons3:
			return L"3 2 0";
		case eTerranCommandResearchInfantryArmor1:
			return L"3 1 0";
		case eTerranCommandResearchInfantryArmor2:
			return L"3 1 0";
		case eTerranCommandResearchInfantryArmor3:
			return L"3 1 0";
		case eTerranCommandResearchBuildingArmor:
			return L"3 0 0";
		case eTerranCommandResearchHiSecAutoTracking:
			return L"";
		case eTerranCommandResearchNeoSteelFrame:
			return L"3 14 0";
		case eTerranCommandResearchMoebiusReactor:
			return L"3 46 0";
		case eTerranCommandResearchPersonalCloaking:
			return L"3 9 0";
		case eTerranCommandArmNuke:
			return L"";
		case eTerranCommandResearchInfernalPreIgniter:
			return L"3 10 0";
		case eTerranCommandResearchSiegeTech:
			return L"3 13 0";
		case eTerranCommandResearch250mmStrikeCannons:
			return L"3 7 0";
		case eTerranCommandResearchVehicleWeapons1:
			return L"3 6 0";
		case eTerranCommandResearchVehicleWeapons2:
			return L"3 6 0";
		case eTerranCommandResearchVehicleWeapons3:
			return L"3 6 0";
		case eTerranCommandResearchVehiclePlating1:
			return L"3 5 0";
		case eTerranCommandResearchVehiclePlating2:
			return L"3 5 0";
		case eTerranCommandResearchVehiclePlating3:
			return L"3 5 0";
		case eTerranCommandResearchShipWeapons1:
			return L"3 4 0";
		case eTerranCommandResearchShipWeapons2:
			return L"3 4 0";
		case eTerranCommandResearchShipWeapons3:
			return L"3 4 0";
		case eTerranCommandResearchShipPlating1:
			return L"3 3 0";
		case eTerranCommandResearchShipPlating2:
			return L"3 3 0";
		case eTerranCommandResearchShipPlating3:
			return L"3 3 0";
		case eTerranCommandResearchDurableMaterials:
			return L"";
		case eTerranCommandResearchCorvidReactor:
			return L"";
		case eTerranCommandResearchCaduceusReactor:
			return L"";
		case eTerranCommandResearchSeekerMissile:
			return L"";
		case eTerranCommandResearchCloakingField:
			return L"3 8 0";
		case eTerranCommandResearchBehemothReactor:
			return L"";
		case eTerranCommandResearchWeaponRefit:
			return L"";

		case eTerranCommandMoveSCVToGas:
		case eTerranCommandMoveSCVToMinerals:
		case eTerranCommandMoveThreeSCVsToGas:
		case eTerranCommandMoveThreeSCVsToMinerals:
			return L"";
		case eTerranCommandWaitOneSecond:
			return L"";


		case eZergCommandBuildHatchery:
			return L"0 36 0";
		case eZergCommandBuildExtractor:
			return L"0 35 0";
		case eZergCommandBuildSpawningPool:
			return L"0 41 0";
		case eZergCommandExpandCreepTumor:
			return L"0 46 0";
		case eZergCommandBuildEvolutionChamber:
			return L"0 34 0";
		case eZergCommandBuildSpineCrawler:
			return L"0 42 0";
		case eZergCommandBuildSporeCrawler:
			return L"0 44 0";
		case eZergCommandBuildRoachWarren:
			return L"0 40 0";
		case eZergCommandBuildLair:
			return L"2 3 0";
		case eZergCommandBuildHydraliskDen:
			return L"0 37 0";
		case eZergCommandBuildBanelingNest:
			return L"0 33 0";
		case eZergCommandBuildSpire:
			return L"0 43 0";
		case eZergCommandBuildInfestationPit:
			return L"0 38 0";
		case eZergCommandBuildNydusNetwork:
			return L"0 39 0";
		case eZergCommandBuildHive:
			return L"2 4 0";
		case eZergCommandBuildUltraliskCavern:
			return L"0 45 0";
		case eZergCommandBuildGreaterSpire:
			return L"2 5 0";

		case eZergCommandCancelExtractor:
			return L"0 35 1";

		case eZergCommandBuildDrone:
			return L"1 28 0";
		case eZergCommandBuildOverlord:
			return L"1 31 0";
		case eZergCommandBuildQueen:
			return L"1 32 0";
		case eZergCommandBuildZergling:
			return L"1 35 0";
		case eZergCommandBuildRoach:
			return L"1 33 0";
		case eZergCommandBuildBaneling:
			return L"2 7 0";
		case eZergCommandBuildHydralisk:
			return L"1 29 0";
		case eZergCommandBuildOverseer:
			return L"2 8 0";
		case eZergCommandBuildInfestor:
			return L"1 38 0";
		case eZergCommandBuildMutalisk:
			return L"1 30 0";
		case eZergCommandBuildCorruptor:
			return L"1 27 0";
		case eZergCommandBuildUltralisk:
			return L"1 34 0";
		case eZergCommandBuildBroodlord:
			return L"2 6 0";

		case eZergCommandExtractorTrickDrone:
			return L"0 35 1";

		case eZergCommandQueenSpawnCreepTumor:
			return L"1 46 0";
		case eZergCommandQueenSpawnLarvae:
			return L"";

		case eZergCommandResearchAdrenalGlands:
			return L"3 41 0";
		case eZergCommandResearchMetabolicBoost:
			return L"3 42 0";
		case eZergCommandResearchMeleeAttacks1:
			return L"3 29 0";
		case eZergCommandResearchMeleeAttacks2:
			return L"3 29 0";
		case eZergCommandResearchMeleeAttacks3:
			return L"3 29 0";
		case eZergCommandResearchGroundCarapace1:
			return L"3 28 0";
		case eZergCommandResearchGroundCarapace2:
			return L"3 28 0";
		case eZergCommandResearchGroundCarapace3:
			return L"3 28 0";
		case eZergCommandResearchMissileAttacks1:
			return L"3 32 0";
		case eZergCommandResearchMissileAttacks2:
			return L"3 32 0";
		case eZergCommandResearchMissileAttacks3:
			return L"3 32 0";
		case eZergCommandResearchGlialReconstitution:
			return L"3 36 0";
		case eZergCommandResearchTunnelingClaws:
			return L"3 38 0";
		case eZergCommandResearchCentrifugalHooks:
			return L"3 45 0";
		case eZergCommandResearchBurrowAtHatchery:
		case eZergCommandResearchBurrowAtLair:
		case eZergCommandResearchBurrowAtHive:
			return L"3 44 0";
		case eZergCommandResearchPneumaticCarapaceAtHatchery:
		case eZergCommandResearchPneumaticCarapaceAtLair:
		case eZergCommandResearchPneumaticCarapaceAtHive:
			return L"3 34 0";
		case eZergCommandResearchVentralSacsAtHatchery:
		case eZergCommandResearchVentralSacsAtLair:
		case eZergCommandResearchVentralSacsAtHive:
			return L"3 35 0";
		case eZergCommandResearchGroovedSpines:
			return L"3 33 0";
		case eZergCommandResearchPathogenGlands:
			return L"3 50 0";
		case eZergCommandResearchNeuralParasite:
			return L"3 49 0";
		case eZergCommandResearchFlyerAttacks1:
			return L"3 31 0";
		case eZergCommandResearchFlyerAttacks2:
			return L"3 31 0";
		case eZergCommandResearchFlyerAttacks3:
			return L"3 31 0";
		case eZergCommandResearchFlyerCarapace1:
			return L"3 30 0";
		case eZergCommandResearchFlyerCarapace2:
			return L"3 30 0";
		case eZergCommandResearchFlyerCarapace3:
			return L"3 30 0";
		case eZergCommandResearchChitinousPlating:
			return L"3 40 0";

		case eZergCommandMoveDroneToGas:
		case eZergCommandMoveDroneToMinerals:
		case eZergCommandMoveThreeDronesToGas:
		case eZergCommandMoveThreeDronesToMinerals:
			return L"";
		case eZergCommandWaitOneSecond:
			return L"";
		}
		break;

	}
	
	return output;
}

bool DisplayCommand(EOutputFormat format, ESC2Command command)
{
	switch(format)
	{
	case eOutputFormatMinimal:
		return false;
	case eOutputFormatSimple:
		return eProtossCommandBuildProbe != command
			&& eTerranCommandBuildSCV != command
			&& eZergCommandBuildDrone != command;
	case eOutputFormatDetailed:
		return true;
	case eOutputFormatFull:
		return true;
	case eOutputFormatHaploid:
		return eProtossCommandBuildProbe != command
			&& eProtossCommandWaitOneSecond != command
			&& eTerranCommandBuildSCV != command
			&& eTerranCommandWaitOneSecond != command
			&& eZergCommandBuildDrone != command
			&& eZergCommandWaitOneSecond != command;
	case eOutputFormatYABOT:
		switch(command)
		{
		case eProtossCommandBuildProbe:
		case eProtossCommandMoveProbeToGas:
		case eProtossCommandMoveProbeToMinerals:
		case eProtossCommandMoveThreeProbesToGas:
		case eProtossCommandMoveThreeProbesToMinerals:
		case eProtossCommandWaitOneSecond:

		case eProtossCommandChronoNexus:
		case eProtossCommandChronoGateway:
		case eProtossCommandChronoWarpGate:
		case eProtossCommandChronoForge:
		case eProtossCommandChronoCyberneticsCore:
		case eProtossCommandChronoTwilightCouncil:
		case eProtossCommandChronoTemplarArchives:
		case eProtossCommandChronoRoboticsFacility:
		case eProtossCommandChronoRoboticsBay:
		case eProtossCommandChronoStargate:
		case eProtossCommandChronoFleetBeacon:

		case eProtossCommandResearchGraviticDrive:
		case eProtossCommandResearchGraviticBoosters:
		case eProtossCommandResearchFluxVanes:
		case eProtossCommandResearchAnionPulseCrystals:
		case eProtossCommandResearchGravitonCatapult:
			return false;

		case eTerranCommandLiftBarracksTechLab:
		case eTerranCommandLiftBarracksReactor:
		case eTerranCommandLiftBarracksNaked:
		case eTerranCommandLiftFactoryTechLab:
		case eTerranCommandLiftFactoryReactor:
		case eTerranCommandLiftFactoryNaked:
		case eTerranCommandLiftStarportTechLab:
		case eTerranCommandLiftStarportReactor:
		case eTerranCommandLiftStarportNaked:

		case eTerranCommandLandBarracksTechLab:
		case eTerranCommandLandBarracksReactor:
		case eTerranCommandLandBarracksNaked:
		case eTerranCommandLandFactoryTechLab:
		case eTerranCommandLandFactoryReactor:
		case eTerranCommandLandFactoryNaked:
		case eTerranCommandLandStarportTechLab:
		case eTerranCommandLandStarportReactor:
		case eTerranCommandLandStarportNaked:

		case eTerranCommandBuildSCV:
		case eTerranCommandCalldownMULE:
		case eTerranCommandCalldownExtraSupplies:
		case eTerranCommandScannerSweep:

		case eTerranCommandResearchNitroPacks:
		case eTerranCommandArmNuke:
		case eTerranCommandResearchHiSecAutoTracking:
		case eTerranCommandResearchDurableMaterials:
		case eTerranCommandResearchCorvidReactor:
		case eTerranCommandResearchCaduceusReactor:
		case eTerranCommandResearchSeekerMissile:
		case eTerranCommandResearchBehemothReactor:
		case eTerranCommandResearchWeaponRefit:

		case eTerranCommandMoveSCVToGas:
		case eTerranCommandMoveSCVToMinerals:
		case eTerranCommandMoveThreeSCVsToGas:
		case eTerranCommandMoveThreeSCVsToMinerals:
		case eTerranCommandWaitOneSecond:
			return false;

		case eZergCommandQueenSpawnLarvae:
		case eZergCommandBuildDrone:
		case eZergCommandMoveDroneToGas:
		case eZergCommandMoveDroneToMinerals:
		case eZergCommandMoveThreeDronesToGas:
		case eZergCommandMoveThreeDronesToMinerals:
		case eZergCommandWaitOneSecond:
			return false;
		}

		return true;
	case eOutputFormatSC2Gears:
		return eProtossCommandBuildProbe != command
			&& eProtossCommandWaitOneSecond != command
			&& eTerranCommandBuildSCV != command
			&& eTerranCommandWaitOneSecond != command
			&& eZergCommandBuildDrone != command
			&& eZergCommandWaitOneSecond != command;
	}

	return false;
}

bool InterpretCommand(const wxString &strCommand, ESC2Command &command)
{
	for(size_t i=0; i < eSC2CommandEnd; i++)
	{
		if(strCommand.Lower() == tostring(eOutputFormatSimple, (ESC2Command)i).Lower())
		{
			command = (ESC2Command)i;
			return true;
		}
	}

	return false;
}
