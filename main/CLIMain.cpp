// scfusion-cli: all the build-order-crunching goodness, none of the MDI windows.
//
// Reads the same save-file XML as the GUI (see SampleSaveFile.xml), runs the
// genetic engine for a while, and prints the best build order it evolved.
// Usage:
//   scfusion-cli [--versions-dir DIR] [--time-limit SECONDS] [--quiet] build.xml
#include "stdafx.h"

#include <wx/xml/xml.h>
#include <wx/dir.h>
#include <wx/filename.h>

#include <chrono>
#include <cstdio>
#include <thread>

#include "SC2/SC2Version.h"
#include "SC2/SC2RaceInfo.h"
#include "SC2/SC2FitnessCalc.h"
#include "SC2/SC2Waypoint.h"
#include "SC2/SC2Output.h"
#include "SC2/SC2Command.h"
#include "SC2/GasMicro.h"
#include "GA/GAEngine.h"
#include "GA/GAPopulationSort.h"

namespace
{

struct COptions
{
	wxString versionsDir;
	wxString buildFile;
	double timeLimit = 30.0;
	bool quiet = false;
};

void PrintUsage()
{
	fprintf(stderr,
		"scfusion-cli: StarCraft II build order optimizer (headless)\n"
		"\n"
		"usage: scfusion-cli [options] <buildfile.xml>\n"
		"  --versions-dir DIR   directory of game-data XMLs (default: auto-detect)\n"
		"  --time-limit SECS    how long to evolve before answering (default: 30)\n"
		"  --quiet              no progress chatter on stderr\n");
}

wxString NodeText(const wxXmlNode *node)
{
	return node->GetNodeContent().Trim().Trim(false);
}

const wxXmlNode *FindChild(const wxXmlNode *parent, const wxString &name)
{
	for(const wxXmlNode *child = parent->GetChildren(); child; child = child->GetNext())
	{
		if(child->GetName() == name)
			return child;
	}
	return NULL;
}

// The GUI shows these strings in a dropdown; the save file stores them verbatim
EGasMicro GasMicroFromString(const wxString &str)
{
	if(str == wxT("Full micro")) return eGasMicroOneBackAndForth;
	if(str == wxT("Three back & forth")) return eGasMicroThreeBackAndForth;
	if(str == wxT("One to gas only")) return eGasMicroOneToGasOnly;
	if(str == wxT("Three to gas only")) return eGasMicroThreeToGasOnly;
	if(str == wxT("Three when geyser ready")) return eGasMicroThreeUponCompletion;
	fprintf(stderr, "warning: unknown GasMicro '%s', using 'Three to gas only'\n", (const char *)str.utf8_str());
	return eGasMicroThreeToGasOnly;
}

const CSC2Command *FindCommandByName(const CSC2RaceInfo &raceInfo, const wxString &name)
{
	const CVector<const CSC2Command *> &commands = raceInfo.GetCommands();
	for(size_t i = 0; i < commands.size(); i++)
	{
		if(commands[i]->GetName() == name)
			return commands[i];
	}
	return NULL;
}

const CSC2Target *FindTargetByName(const CSC2RaceInfo &raceInfo, const wxString &name)
{
	const CVector<CSC2Target *> &targets = raceInfo.GetTargets();
	for(size_t i = 0; i < targets.size(); i++)
	{
		if(targets[i]->GetName() == name)
			return targets[i];
	}
	return NULL;
}

bool ReadOptionalCount(const wxXmlNode *targetNode, const wxString &childName, size_t &value)
{
	const wxXmlNode *node = FindChild(targetNode, childName);
	if(!node)
		return false;
	long v = 0;
	if(!NodeText(node).ToLong(&v))
		return false;
	value = (size_t)v;
	return true;
}

// Mirror of CSC2Engine::BuildWaypoint, reading save-file XML instead of a wxPropertyGrid
bool ParseWaypoint(const wxXmlNode *waypointNode, const CSC2RaceInfo &raceInfo, CSC2Waypoint &waypoint)
{
	bool hasTarget = false;

	for(const wxXmlNode *node = waypointNode->GetChildren(); node; node = node->GetNext())
	{
		if(node->GetName() == wxT("MinTime"))
		{
			NodeText(node).ToCDouble(&waypoint.m_targetTime.min);
		}
		else if(node->GetName() == wxT("MaxTime"))
		{
			NodeText(node).ToCDouble(&waypoint.m_targetTime.max);
		}
		else if(node->GetName() == wxT("Target"))
		{
			wxString name = node->GetAttribute(wxT("name"));

			if(name == wxT("Supply"))
			{
				hasTarget = true;
				size_t v;
				if(ReadOptionalCount(node, wxT("Minimum"), v)) { waypoint.m_supplyCount.haveMin = true; waypoint.m_supplyCount.min = v; }
				if(ReadOptionalCount(node, wxT("Maximum"), v)) { waypoint.m_supplyCount.haveMax = true; waypoint.m_supplyCount.max = v; }
			}
			else if(name == wxT("Bases"))
			{
				hasTarget = true;
				size_t v;
				if(ReadOptionalCount(node, wxT("Minimum"), v)) { waypoint.m_baseCount.haveMin = true; waypoint.m_baseCount.min = v; }
				if(ReadOptionalCount(node, wxT("Maximum"), v)) { waypoint.m_baseCount.haveMax = true; waypoint.m_baseCount.max = v; }
				if(ReadOptionalCount(node, wxT("MinimumUnderConstruction"), v)) { waypoint.m_baseUnderConstructionCount.haveMin = true; waypoint.m_baseUnderConstructionCount.min = v; }
				if(ReadOptionalCount(node, wxT("MaximumUnderConstruction"), v)) { waypoint.m_baseUnderConstructionCount.haveMax = true; waypoint.m_baseUnderConstructionCount.max = v; }
			}
			else if(name == wxT("Minerals") || name == wxT("Gas") || name == wxT("Minerals Mined") || name == wxT("Gas Mined"))
			{
				const wxXmlNode *valueNode = FindChild(node, wxT("Value"));
				if(valueNode)
				{
					double v = 0.0;
					NodeText(valueNode).ToCDouble(&v);
					hasTarget = true;
					if(name == wxT("Minerals")) waypoint.m_minerals = v;
					else if(name == wxT("Gas")) waypoint.m_gas = v;
					else if(name == wxT("Minerals Mined")) waypoint.m_mineralsMined = v;
					else waypoint.m_gasMined = v;
				}
			}
			else
			{
				const CSC2Target *target = FindTargetByName(raceInfo, name);
				if(!target)
				{
					fprintf(stderr, "warning: unknown target '%s' (skipped)\n", (const char *)name.utf8_str());
					continue;
				}

				hasTarget = true;

				switch(target->GetRestrictionType())
				{
				case CSC2Target::eBooleanMinMaxAndUnderConstruction:
					{
						const wxXmlNode *valueNode = FindChild(node, wxT("Value"));
						wxString value = valueNode ? NodeText(valueNode) : wxString();
						if(value == wxT("Ignored"))
							waypoint.m_maxRequirements.push_back(CSC2Waypoint::STargetRequirement(target, 0));
						else if(value == wxT("Required"))
							waypoint.m_minRequirements.push_back(CSC2Waypoint::STargetRequirement(target, 1));
						else if(value == wxT("Under Construction"))
							waypoint.m_minUnderConstructionRequirements.push_back(CSC2Waypoint::STargetRequirement(target, 1));
					}
					break;
				case CSC2Target::eSizeTMinMax:
				case CSC2Target::eSizeTMinMaxAndUnderConstruction:
					{
						size_t v;
						if(ReadOptionalCount(node, wxT("Minimum"), v))
							waypoint.m_minRequirements.push_back(CSC2Waypoint::STargetRequirement(target, v));
						if(ReadOptionalCount(node, wxT("Maximum"), v))
							waypoint.m_maxRequirements.push_back(CSC2Waypoint::STargetRequirement(target, v));
						if(ReadOptionalCount(node, wxT("MinimumUnderConstruction"), v))
							waypoint.m_minUnderConstructionRequirements.push_back(CSC2Waypoint::STargetRequirement(target, v));
						if(ReadOptionalCount(node, wxT("MaximumUnderConstruction"), v))
							waypoint.m_maxUnderConstructionRequirements.push_back(CSC2Waypoint::STargetRequirement(target, v));
					}
					break;
				case CSC2Target::eBoolean:
					{
						const wxXmlNode *valueNode = FindChild(node, wxT("Value"));
						if(valueNode && NodeText(valueNode) == wxT("Required"))
							waypoint.m_stateDurationRequirements.push_back(target);
					}
					break;
				}
			}
		}
	}

	return hasTarget;
}

} // namespace

int main(int argc, char **argv)
{
	// The GUI does this in MyApp::OnInit; without it every allocation sulks
	CMemoryPoolManager::InitialiseSingleton();
	CMemPoolNodePoolManager::Get()->InitialiseThread();

	COptions opts;
	for(int i = 1; i < argc; i++)
	{
		wxString arg = wxString::FromUTF8(argv[i]);
		if(arg == wxT("--versions-dir") && i + 1 < argc)
			opts.versionsDir = wxString::FromUTF8(argv[++i]);
		else if(arg == wxT("--time-limit") && i + 1 < argc)
			wxString::FromUTF8(argv[++i]).ToCDouble(&opts.timeLimit);
		else if(arg == wxT("--quiet"))
			opts.quiet = true;
		else if(arg == wxT("--help") || arg == wxT("-h"))
		{
			PrintUsage();
			return 0;
		}
		else if(!arg.StartsWith(wxT("--")))
			opts.buildFile = arg;
		else
		{
			fprintf(stderr, "error: unknown option '%s'\n", argv[i]);
			PrintUsage();
			return 2;
		}
	}

	if(opts.buildFile.IsEmpty())
	{
		PrintUsage();
		return 2;
	}

	// --- Find the game-data directory ---
	if(opts.versionsDir.IsEmpty())
	{
		const wxString candidates[] = { wxT("Versions"), wxT("main/Versions"), wxT("../main/Versions") };
		for(const wxString &c : candidates)
		{
			if(wxDir::Exists(c))
			{
				opts.versionsDir = c;
				break;
			}
		}
		if(opts.versionsDir.IsEmpty())
		{
			fprintf(stderr, "error: couldn't find a Versions directory; use --versions-dir\n");
			return 1;
		}
	}

	// --- Load game versions ---
	CVector<CSC2Version *> versions;
	{
		wxArrayString files;
		wxDir::GetAllFiles(opts.versionsDir, &files, wxT("*.xml"));
		for(size_t i = 0; i < files.size(); i++)
		{
			CSC2Version *version = new CSC2Version();
			if(version->Load(files[i]))
				versions.push_back(version);
			else
			{
				fprintf(stderr, "error: failed to load game data from '%s'\n", (const char *)files[i].utf8_str());
				delete version;
			}
		}
	}
	if(versions.size() == 0)
	{
		fprintf(stderr, "error: no game data found in '%s'\n", (const char *)opts.versionsDir.utf8_str());
		return 1;
	}

	// --- Read the build file ---
	wxXmlDocument doc;
	{
		wxLogNull shush; // wxXml grumbles on stderr; we do our own grumbling
		if(!doc.Load(opts.buildFile) || doc.GetRoot()->GetName() != wxT("SCFusionBuild"))
		{
			fprintf(stderr, "error: '%s' is not an SCFusion build file\n", (const char *)opts.buildFile.utf8_str());
			return 1;
		}
	}
	const wxXmlNode *root = doc.GetRoot();

	const wxXmlNode *raceNode = FindChild(root, wxT("Race"));
	if(!raceNode)
	{
		fprintf(stderr, "error: build file has no <Race>\n");
		return 1;
	}
	wxString raceName = NodeText(raceNode);

	// Pick the version matching the file's game description, or fall back to the first
	const CSC2Version *version = versions[0];
	if(const wxXmlNode *gameNode = FindChild(root, wxT("GameLongDescription")))
	{
		wxString wanted = NodeText(gameNode);
		bool matched = false;
		for(size_t i = 0; i < versions.size(); i++)
		{
			if(versions[i]->GetGameLongDescription() == wanted)
			{
				version = versions[i];
				matched = true;
				break;
			}
		}
		if(!matched && !opts.quiet)
			fprintf(stderr, "note: no game data for '%s'; using '%s'\n",
				(const char *)wanted.utf8_str(),
				(const char *)version->GetGameLongDescription().utf8_str());
	}

	const CSC2RaceInfo *raceInfo = version->GetRace(raceName);
	if(!raceInfo)
	{
		fprintf(stderr, "error: race '%s' not found in game data\n", (const char *)raceName.utf8_str());
		return 1;
	}

	// --- Settings ---
	double maxTime = 1200.0;
	EGasMicro gasMicro = eGasMicroThreeToGasOnly;
	bool allowWaitCommands = false;
	double minimumCommandDuration = 0.0;
	CVector<const CSC2Command *> initialBuildOrder;
	int scoutMode = 0; // 0 none, 1 stays, 2 returns, 3 dies
	double scoutTime = 0.0, scoutEndTime = 0.0;

	if(const wxXmlNode *settings = FindChild(root, wxT("Settings")))
	{
		if(const wxXmlNode *n = FindChild(settings, wxT("MaximumTime")))
			NodeText(n).ToCDouble(&maxTime);
		if(const wxXmlNode *n = FindChild(settings, wxT("GasMicro")))
			gasMicro = GasMicroFromString(NodeText(n));
		if(const wxXmlNode *n = FindChild(settings, wxT("AllowWaitCommands")))
			allowWaitCommands = (NodeText(n) == wxT("True"));
		if(const wxXmlNode *n = FindChild(settings, wxT("MaxAPM")))
		{
			double apm = 0.0;
			if(NodeText(n).ToCDouble(&apm) && apm > 0.0)
				minimumCommandDuration = 60.0 / apm;
		}
		if(const wxXmlNode *n = FindChild(settings, wxT("ScoutingWorker")))
		{
			NodeText(n).ToCDouble(&scoutTime);
			wxString endStr;
			if(n->GetAttribute(wxT("returnTime"), &endStr)) { scoutMode = 2; endStr.ToCDouble(&scoutEndTime); }
			else if(n->GetAttribute(wxT("deathTime"), &endStr)) { scoutMode = 3; endStr.ToCDouble(&scoutEndTime); }
			else scoutMode = 1;
		}
		if(const wxXmlNode *n = FindChild(settings, wxT("InitialBuildOrder")))
		{
			for(const wxXmlNode *cmd = n->GetChildren(); cmd; cmd = cmd->GetNext())
			{
				if(cmd->GetName() != wxT("Command"))
					continue;
				const CSC2Command *command = FindCommandByName(*raceInfo, NodeText(cmd));
				if(command)
					initialBuildOrder.push_back(command);
				else
					fprintf(stderr, "warning: unknown initial build order command '%s' (skipped)\n", (const char *)NodeText(cmd).utf8_str());
			}
		}
	}

	// --- Fitness calculator + waypoints (mirrors CSC2Engine::InitialiseFitnessCalc) ---
	ESC2Race race = (raceName == wxT("Terran")) ? eTerran : (raceName == wxT("Zerg")) ? eZerg : eProtoss;
	CSC2FitnessCalc fitnessCalc(race, *raceInfo, maxTime, gasMicro, minimumCommandDuration);

	bool hasTarget = false;
	if(const wxXmlNode *waypoints = FindChild(root, wxT("Waypoints")))
	{
		for(const wxXmlNode *wpNode = waypoints->GetChildren(); wpNode; wpNode = wpNode->GetNext())
		{
			if(wpNode->GetName() != wxT("Waypoint"))
				continue;

			bool isFinal = (wpNode->GetAttribute(wxT("target")) == wxT("true"));
			CSC2Waypoint waypoint(*raceInfo, isFinal);
			if(!ParseWaypoint(wpNode, *raceInfo, waypoint))
				continue;

			if(isFinal)
				waypoint.m_targetTime.max = DBL_MAX;
			else
				waypoint.m_targetTime.min = 0.0;

			fitnessCalc.AddWaypoint(waypoint);
			hasTarget = true;
		}
	}

	if(!hasTarget)
	{
		fprintf(stderr, "error: build file contains no targets to optimize for\n");
		return 1;
	}

	CVector<const CSC2Command *> alphabet;
	fitnessCalc.BuildAlphabet(alphabet);

	CSC2WaitCommand waitPoint1(raceInfo->GetData(), 0.1);
	CSC2WaitCommand wait1(raceInfo->GetData(), 1.0);
	CSC2WaitCommand wait5(raceInfo->GetData(), 5.0);
	if(allowWaitCommands)
	{
		alphabet.push_back(&waitPoint1);
		alphabet.push_back(&wait1);
		alphabet.push_back(&wait5);
	}

	switch(scoutMode)
	{
	case 1:
		fitnessCalc.AddCustomEvent(CSC2Event(scoutTime, CSC2Event::eSendScout));
		break;
	case 2:
		fitnessCalc.AddCustomEvent(CSC2Event(scoutTime, CSC2Event::eSendScout));
		fitnessCalc.AddCustomEvent(CSC2Event(scoutEndTime, CSC2Event::eReturnScout));
		break;
	case 3:
		fitnessCalc.AddCustomEvent(CSC2Event(scoutTime, CSC2Event::eSendScout));
		fitnessCalc.AddCustomEvent(CSC2Event(scoutEndTime, CSC2Event::eKillScout));
		break;
	}

	fitnessCalc.SetInitialBuildOrder(initialBuildOrder);
	if(!fitnessCalc.InitialiseBuildOrder())
	{
		fprintf(stderr, "error: initial build order is not valid for this race/version\n");
		return 1;
	}

	// --- Seed chromosome from the file's saved BuildOrder ---
	CSC2NewChromosome seed;
	if(const wxXmlNode *buildOrder = FindChild(root, wxT("BuildOrder")))
	{
		for(const wxXmlNode *cmd = buildOrder->GetChildren(); cmd; cmd = cmd->GetNext())
		{
			if(cmd->GetName() != wxT("Command"))
				continue;
			const CSC2Command *command = FindCommandByName(*raceInfo, NodeText(cmd));
			if(command)
				seed.GetValue().push_back(command);
			else
				fprintf(stderr, "warning: unknown seed command '%s' (skipped)\n", (const char *)NodeText(cmd).utf8_str());
		}
	}

	// --- The engine (same village layout as the GUI) ---
	CSC2NewMutator mutator(alphabet, 0.3);
	CGAEngine<CSC2NewChromosome, CSC2NewMutator, CSC2FitnessCalc, CFitnessValue, CGAPopulationPartialSortAndRemoveDuplicates> engine;

	engine.InitConfiguration(mutator, fitnessCalc, 1000);
	engine.AddVillage(200, 100);
	engine.AddVillage(200, 100);
	engine.AddVillage(200, 100);
	engine.AddVillage(50, 30);
	engine.AddVillage(10, 8);
	engine.AddSeed(seed);

	if(!opts.quiet)
		fprintf(stderr, "%s %s: evolving for up to %.0fs...\n",
			(const char *)raceName.utf8_str(),
			(const char *)version->GetGameLongDescription().utf8_str(),
			opts.timeLimit);

	engine.Start();

	auto startTime = std::chrono::steady_clock::now();
	auto elapsed = [&]{ return std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count(); };

	while(elapsed() < opts.timeLimit && engine.IsRunning())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		if(!opts.quiet)
			fprintf(stderr, "\r  t=%4.0fs evolutions=%zu games=%llu best=%g   ",
				elapsed(), engine.CityEvolution(), engine.TotalGameCount(), (double)engine.CityBestFitness());
	}
	if(!opts.quiet)
		fprintf(stderr, "\n");

	engine.Stop();

	CSC2NewChromosome best;
	engine.GetBestGame(best);

	if(best.GetValue().size() == 0)
	{
		fprintf(stderr, "error: no build order found (this usually means the target is impossible)\n");
		return 1;
	}

	// --- Print the result ---
	CSC2OutputMinimal output;
	output.Reset();
	CSC2State *finalState = fitnessCalc.PrintGame(output, best);

	wxString text;
	output.GetOutput(text);
	printf("%s", (const char *)text.utf8_str());

	// Did the champion actually hit the final target? (Mid-waypoints are judged
	// at their own times during the sim; only the final target is judged here.)
	bool satisfied = true;
	const CVector<const CSC2Waypoint *> &targets = fitnessCalc.GetTargets();
	for(size_t i = 0; i < targets.size(); i++)
	{
		if(targets[i]->IsFinalTarget())
			satisfied = satisfied && targets[i]->satisfiesTarget(*finalState);
	}
	delete finalState;

	if(!opts.quiet)
		fprintf(stderr, satisfied ? "target satisfied \\o/\n" : "target NOT satisfied (best attempt printed anyway)\n");

	return satisfied ? 0 : 3;
}
