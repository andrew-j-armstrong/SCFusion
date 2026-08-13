# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

Super Fusion (SCFusion) is a Windows desktop application that finds optimal StarCraft II build orders using a genetic algorithm. It is a C++ / wxWidgets MDI GUI app built with Visual Studio; there is no CMake, Makefile, test suite, or linter.

## Building

### Linux / headless (CMake)

```sh
cmake -B build -S . && cmake --build build -j
./build/scfusion-cli --versions-dir main/Versions --time-limit 30 baselines/targets/protoss-2gate-stalkers.xml
./build/scfusion-gui   # only built when the full wxWidgets (wxGTK) is installed
```

Builds `scfusion-core` (Core+GA+SC2, linked against wxBase/wxXml only — no GUI libs) and `scfusion-cli` (`main/CLIMain.cpp`), which reads the same save-file XML as the GUI, runs the GA, and prints the best build order (exit 0 = target satisfied, 3 = not satisfied). Requires wxWidgets 3.2 devel packages (on Void: `wxWidgets-common-devel`; CMake finds `wx-config-gtk3`). The headless build defines `SCF_HEADLESS`, which makes `stdafx.h` include wxBase headers only and shim `wxMessageBox` to stderr. `baselines/` holds per-version CLI reference outputs (GA is time-seeded — compare target-reached times, not exact sequences).

If the full wxWidgets is present (on Void: also `wxWidgets-gtk3-devel`), CMake additionally builds `scfusion-gui` — the same MDI app as Windows, on wxGTK. The engine is compiled twice: `scfusion-core` (headless) and `scfusion-core-gui` (without `SCF_HEADLESS`, so `wxMessageBox` is a real dialog). GUI colors follow the OS light/dark theme (`GridOutput.cpp`/`ChartPanel.cpp` keep paired accent palettes); property-grid splitter positions must be set via `CallAfter` because wxGTK sizes windows asynchronously.

### CI

`.github/workflows/ci.yml` runs on every push: a Linux job (CMake build + a 20s GA smoke run per race) and a Windows job that compile-checks `main/SCFusion_vc10.vcxproj` with MSBuild against the official wxWidgets 3.1.7 vc14x binaries + WinSparkle 0.8.1 (downloaded in CI, cached; `wxWidgets/wxwidgets.props` pins the layout — keep it in sync with the workflow if versions change).

### Windows GUI (Visual Studio)

Open `Super Fusion.sln` in Visual Studio 2022 (Community is fine) and build the `SCFusion Main` project (`main/SCFusion_vc10.vcxproj`, platform toolset v143, Win32/x86 only). The solution also contains the `Installer` project (`Installer/Installer.vdproj`, a .vdproj setup project).

External dependencies are expected to be downloaded manually (see `wxWidgets/download.txt` and `WinSparkle/download.txt`):
- wxWidgets Windows binaries (headers, dev files, release DLLs) from wxwidgets.org
- WinSparkle (auto-update library) from github.com/vslavik/winsparkle

### Portability gotchas (learned the hard way)

- `rand_sse()` yields 15-bit values; GA scaling code must use `RAND_SSE_MAX` (0x7FFF), never `RAND_MAX` (which is 2³¹−1 on glibc and silently breaks evolution).
- `Core/` threading is std C++ (`std::thread`/`std::mutex`/`CThreadTaskPtr` from `Core/ThreadPool.h`); don't reintroduce Win32 HANDLEs.
- `OutputFormat.cpp` and `SC2AStar.cpp` are dead code — neither build compiles them.

## Architecture

All application code lives under `main/`, split into a generic-algorithms layer and an SC2-specific layer:

- `main/Core/` — hand-rolled containers and concurrency primitives used instead of the STL in hot paths: `CVector`, `CPriorityQueue`, `Hashtable`, `ThreadPool`, lock-free queues, and a custom `MemoryPoolManager` (initialized per-thread; see `Application.cpp::OnInit`).
- `main/GA/` — a fully templated genetic-algorithm engine (`CGAEngine<TChromosome, TMutator, TFitnessCalc, TFitness, TPopulationSort>`). Its population model is "villages" evolving independently on thread-pool threads, feeding their best chromosomes into a shared "city" population. Chromosomes are command sequences (`CGASequenceChromosome<const CSC2Command *>`).
- `main/AStar/` — a templated A* search engine (an alternative search strategy; policy classes for open lists, duplicate managers, and parent tracking are mixed in via template parameters). Used via `main/SC2/SC2AStar.*`.
- `main/SC2/` — the StarCraft II domain model: `CSC2State` (game state), `CSC2Command` (build/research/ability commands), `CSC2Event` (completion events), and `CSC2FitnessCalc`, which scores a chromosome by simulating the command sequence as a discrete-event simulation against waypoint/target requirements. `SC2RaceData`/`SC2RaceInfo`/`SC2Version` load all game data (buildings, units, costs, timings) from XML.
- `main/` root — the wxWidgets GUI. `Application.cpp` is the entry point (also initializes WinSparkle auto-update in Release builds). `MDIParent` is the main frame; each `MDIChild` window is one build-order optimization holding a `CSC2Engine` (defined in `SC2Engine.h/cpp`), which glues the GUI's wxPropertyGrid inputs to the GA engine and renders results as text, grid (`GridOutput`), or Gantt chart (`ChartPanel`).

### Game data is XML, not code

Unit/building stats, costs, and build times are NOT hardcoded — they live in `main/Versions/StarCraft.xml`, discovered at runtime by scanning the `Versions/` directory (`MDIParent::LoadVersions`). Balance-patch updates (the most common kind of change in this repo, see CHANGELOG.md) are usually edits to this XML file, only requiring C++ changes when a patch introduces new mechanics. `main/SampleSaveFile.xml` shows the saved build-order file format.

## Release process (from README)

1. Update `CHANGELOG.md`
2. Update version in `MDIParent.cpp` (UI display) and `SCFusion.rc` (FILEVERSION, PRODUCTVERSION, FileVersion, ProductVersion)
3. Update Installer: OutputFilename and ProductVersion (regenerates ProductCode/PackageCode)
4. Publish updated `appcast.xml` and release notes (WinSparkle appcast is hosted at andrew-j-armstrong.github.io/SCFusion)

Release branches are merged via `release/x.y.z` branches (see git history).
