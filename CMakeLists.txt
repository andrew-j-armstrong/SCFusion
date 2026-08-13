# SCFusion: Linux/macOS build — headless CLI always, wxWidgets GUI when the
# full toolkit is installed. (The Windows build still lives in Super Fusion.sln)
cmake_minimum_required(VERSION 3.16)
project(SCFusion CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# wxBase only: strings + XML, no windows, no widgets, no regrets
# (Void Linux ships wx-config as wx-config-gtk3; help CMake find it)
find_program(wxWidgets_CONFIG_EXECUTABLE NAMES wx-config wx-config-gtk3 wx-config-3.2)
find_package(wxWidgets REQUIRED COMPONENTS xml base)
include(${wxWidgets_USE_FILE})
set(SCF_WXBASE_LIBS ${wxWidgets_LIBRARIES})

# The engine: everything below main/{Core,GA,SC2}. Compiled twice — once
# headless (wxMessageBox → stderr) for the CLI, once against real wxWidgets
# for the GUI, where XML load errors deserve an actual popup.
set(SCF_ENGINE_SOURCES
  # Core: hand-rolled containers and friends (LockFree* excluded — nothing uses them)
  main/Core/FastRand.cpp
  main/Core/FastString.cpp
  main/Core/HashFunction.cpp
  main/Core/Hashtable.cpp
  main/Core/LinkedList.cpp
  main/Core/MemoryPool.cpp
  main/Core/MemoryPoolManager.cpp
  main/Core/ThreadPool.cpp
  main/Core/Tokeniser.cpp
  main/Core/Vector.cpp
  # GA: the genetic engine (mostly templates; these are the non-header bits)
  main/GA/GAConfiguration.cpp
  main/GA/GAPopulation.cpp
  main/GA/GASequenceChromosome.cpp
  # SC2: the actual StarCraft
  # (OutputFormat.cpp and SC2AStar.cpp are dead code — the vcxproj doesn't build them either)
  main/SC2/GameCalcs.cpp
  main/SC2/SC2Building.cpp
  main/SC2/SC2BuildingStatus.cpp
  main/SC2/SC2Command.cpp
  main/SC2/SC2Event.cpp
  main/SC2/SC2FitnessCalc.cpp
  main/SC2/SC2Output.cpp
  main/SC2/SC2Race.cpp
  main/SC2/SC2RaceData.cpp
  main/SC2/SC2RaceInfo.cpp
  main/SC2/SC2RequiredCommand.cpp
  main/SC2/SC2Research.cpp
  main/SC2/SC2State.cpp
  main/SC2/SC2Target.cpp
  main/SC2/SC2Unit.cpp
  main/SC2/SC2Version.cpp
  main/SC2/SC2Waypoint.cpp
  main/SC2/TargetMinMax.cpp
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-msse2" HAVE_MSSE2)
find_package(Threads REQUIRED)

add_library(scfusion-core STATIC ${SCF_ENGINE_SOURCES})
target_include_directories(scfusion-core PUBLIC main)
# wxDEBUG_LEVEL=0: the old format strings say %u where 64-bit size_t arrives;
# values fit in 32 bits so the output is fine — but wx 3.2's debug asserts
# would fill stderr with existential dread.
target_compile_definitions(scfusion-core PUBLIC SCF_HEADLESS wxDEBUG_LEVEL=0)
target_link_libraries(scfusion-core PUBLIC ${SCF_WXBASE_LIBS} Threads::Threads)

# SSE2 for FastRand (x86 only; ARM can have xoshiro when it asks nicely)
if(HAVE_MSSE2)
  target_compile_options(scfusion-core PUBLIC -msse2)
endif()

add_executable(scfusion-cli main/CLIMain.cpp)
target_link_libraries(scfusion-cli PRIVATE scfusion-core)

# ---------------------------------------------------------------------------
# The GUI (Phase 6): same app the Windows build ships, on wxGTK.
# Optional — if the full wxWidgets isn't installed, the CLI still builds.
# ---------------------------------------------------------------------------
find_package(wxWidgets QUIET COMPONENTS propgrid adv core xml base)
if(wxWidgets_FOUND)
  message(STATUS "Full wxWidgets found — building scfusion-gui")

  # Second compile of the engine, this time with real message boxes.
  add_library(scfusion-core-gui STATIC ${SCF_ENGINE_SOURCES})
  target_include_directories(scfusion-core-gui PUBLIC main)
  target_compile_definitions(scfusion-core-gui PUBLIC wxDEBUG_LEVEL=0)
  target_link_libraries(scfusion-core-gui PUBLIC ${wxWidgets_LIBRARIES} Threads::Threads)
  if(HAVE_MSSE2)
    target_compile_options(scfusion-core-gui PUBLIC -msse2)
  endif()

  add_executable(scfusion-gui WIN32
    main/Application.cpp
    main/ChartPanel.cpp
    main/GridOutput.cpp
    main/MDIChild.cpp
    main/MDIParent.cpp
    main/OutputPrintout.cpp
    main/RaceChoiceDlg.cpp
    main/SC2Engine.cpp
    main/TimeTextCtrl.cpp
    main/TimeValidator.cpp
  )
  target_link_libraries(scfusion-gui PRIVATE scfusion-core-gui)
else()
  message(STATUS "Full wxWidgets not found — GUI skipped, CLI only")
endif()
