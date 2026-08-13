# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## Unreleased (v2.3.0)

### Linux / headless support

* New CMake build (`cmake -B build && cmake --build build`) producing a
  headless `scfusion-cli` that reads the same save-file XML as the GUI,
  runs the genetic engine, and prints the best build order. Links against
  wxBase only (no GUI libraries). The Windows GUI build in
  `Super Fusion.sln` is unchanged.
* Ported `Core/` threading from Win32 to `std::thread`/`std::mutex`
  (identical behavior on Windows; the code is now portable C++17).
* Fixed a 64-bit portability bug: the GA scaled its roulette-wheel and
  mutation indices by `RAND_MAX`, which matches `rand_sse()`'s 15-bit range
  on MSVC but not on glibc (where it overflowed and silently disabled
  evolution). Now uses an explicit `RAND_SSE_MAX`.
* The GA engine now keeps its best game available after `Stop()` instead of
  discarding it (the GUI polled continuously and never noticed).
* Added `baselines/` — per-version reference outputs from `scfusion-cli`
  for sanity-checking balance-data changes.
* The full GUI now builds and runs on Linux (`scfusion-gui`, built
  automatically when wxGTK is installed): WinSparkle and Win32 priority
  calls are Windows-only via `#ifdef`, colors follow the OS light/dark
  theme (with paired accent palettes for the output grid and Gantt
  chart), and `Versions/` is found relative to the executable as well as
  the working directory.
* GitHub Actions CI: Linux build + per-race GA smoke test, and a Windows
  MSBuild compile check of the Visual Studio project against the official
  wxWidgets 3.1.7 binaries (`wxWidgets/wxwidgets.props` is now committed).

### Upgrade for [Patch v5.0.16](https://news.blizzard.com/en-us/article/24245740/starcraft-ii-5-0-16-patch-notes)

#### Economy
* Starting workers reduced from 12 to 8 (all races).

#### Protoss
* Nexus supply provided reduced from 15 to 13.
* Warpgate research now speeds up Gateway unit production by 40%: modeled as
  research-gated "Boosted Gateway" build commands (Zealot 16s, Adept 18s,
  Stalker 16s, Sentry 14s, High/Dark Templar 26s) that the existing
  first-match Multi commands prefer over the regular Gateway ones.
* Warp-in time increased from 3.6 to 4 seconds.
* Warpgate cooldowns: Zealot/Adept 20→22, Stalker/Sentry 23→22,
  High/Dark Templar 32→35.
* 'Transform to Warpgate' now costs 25/25, charged on the
  Convert Gateway To Warp Gate command (exact per-gateway one-time cost as
  long as no gateway is converted back and forth, which the optimizer has
  no incentive to do).

#### Terran
* Command Center (and Orbital Command / Planetary Fortress) supply provided
  reduced from 15 to 13.
* Ghost supply cost increased from 2 to 3.

#### Zerg
* Hatchery/Lair/Hive supply provided reduced from 6 to 4.
* Hatchery cost increased from 275 to 300 (Macro Hatchery keeps its +25
  offset: 300 → 325).
* Larva spawn interval: Blizzard's notes say 10.7s → 9.5s; this data set
  said 11.0 (0.3s above Blizzard's old number, presumably spawn overhead),
  so it becomes 9.8 to keep that offset.
* Carapace upgrade costs reduced: L1 150/150→100/100, L2 200/200→150/150,
  L3 250/250→200/200.
* Microbial Shroud no longer requires an upgrade: research removed from the
  data (the energy-based ability target remains).

#### Not modeled (intentionally skipped)
* Map pool; mineral patch / vespene geyser resource totals (the income
  model is saturation-rate based with no depletion); rich geyser return;
  all combat stats (damage/range/health/speed); Shield Battery starting
  energy (battery energy is not used by any build-order command); Overlord
  speed; subgroup priorities; all bug fixes / QoL items.

## Super Fusion v2.2.4

### Upgrade for [Patch v5.0.15](https://news.blizzard.com/en-us/article/24225313/starcraft-ii-5-0-15-patch-notes)

#### Spire
* Spire cost reduced from 200/200 to 150/150
* Spire build time reduced from 71 seconds to 66 seconds

#### Microbial Shroud
* Microbial Shroud cost increased from 75 to 100
* Microbial Shroud now requires an upgrade again (150/150)

#### Viking
* Viking cost reduced from 150/75 to 125/75

#### Hyperflight Rotors
* Hyperflight Rotors research time decreased to from 100 to 79 seconds

#### Ghost
* Ghost supply reduced from 3 to 2

## Super Fusion v2.2.3

### Upgrade for [Patch v5.0.14](https://news.blizzard.com/en-us/article/24162754/starcraft-ii-5-0-14-patch-notes)

#### Nexus
* Battery Overcharge removed.
* New ability: Energy Recharge (50 Energy / 60 second cooldown). 

#### Stalker
* Train time from Gateway reduced from 30 to 27 seconds.

#### Tempest
* Supply Cost reduced from 5 to 4.

#### Immortal
* Cost reduced from 275/100 to 250/100.

#### Mothership
* Cost increased from 300/300 to 400/400.
* Supply cost increased from 6 to 8.

#### Cyclone
* Reverted to patch 5.0.11 Cyclone.

#### Sensor Tower
* Cost reduced from 125/100 to 100/50.

#### Ghost
* Supply cost increased from 2 to 3.

#### Queen
* Cost increased from 150/0 to 175/0.

#### Hatchery
* Cost reduced from 300/0 to 275/0.

#### Hydralisk
* New research: Nanomuscular Swell (researches new Lunge ability). Cost: 100/100, 64 seconds. Requires Hive.

### Upgrade for [Patch v5.0.13](https://news.blizzard.com/en-us/article/24078322/starcraft-ii-5-0-13-patch-notes)

#### Armory
* Cost reduced from 150/100 to 150/50.

#### Engineering Bay
* Infantry weapons/armor upgrades cost reduced from 100/175/250 to 100/150/200.

#### Observer
* Build time reduced from 21.4 to 17.9 seconds.

### Upgrade for [Patch v5.0.12](https://news.blizzard.com/en-us/article/24009150/starcraft-ii-5-0-12-patch-notes)

#### Cyclone
* Cost reduced from 150/100 to 125/50.
* Tech Lab requirement removed.
* Supply cost reduced from 3 to 2.
* Mag-Field Accelerator upgrade removed.
* New upgrade, increasing move speed to 4.73 (Cost: 100/100, 100 seconds, Researched at the Factory's Tech Lab).

#### Medivac
* Speed upgrade replaced with Caduceus Reactor - Increases Medivac's energy regeneration rate by 100% (Cost: 100/100/53.57 seconds, Upgraded from the Fusion Core).

### Raven
* Interference Matrix now requires a research (Cost: 50/50, 57.14 seconds research time, Researched at the Starport's Tech Lab).

#### Lurker
* Adaptive Talons cost reduced from 150/150 to 100/100.

#### Infestor
* Pathogen Glands upgrade removed.
* Infestor starting energy increased from 50 to 75.

#### Ultralisk
* Cost reduced from 300/200 to 275/200.

#### Hydralisk
* Speed upgrade research time reduced from 71 to 64 seconds.
* Range upgrade research cost / time reduced from 100/100 / 71 seconds to 75/75 / 50 seconds.

#### Baneling
* Centrifugal Hooks cost reduced from 150/150 to 100/100.
* Centrifugal Hooks research time reduced from 79 to 71.

#### Spire
* Air Armor upgrade cost reduced from 150/150, 225/225, 300/300 to 100/100, 175/175, 250/250 (Same as Air Weapons).

#### Evolution Chamber
* Armor upgrade cost reduced from 150/150, 225/225, 300/300 to 150/150, 200/200, 250/250.

#### Forge
* Shield upgrade cost reduced from 150/150, 225/225, 300/300 to 150/150, 200/200, 250/250.

#### Cybernetics Core
* Air Armor upgrade cost reduced from 150/150, 225/225, 300/300 to 100/100, 175/175, 250/250 (Same as Air Weapons).

#### Disruptor
* Supply cost increased from 3 to 4.

#### Mothership
* Cost reduced from 400/400 to 300/300.
* Supply cost reduced from 8 to 6.
* Build time reduced from 114 to 79.
* No longer has energy (Spells are now cooldown based and require no energy to cast).

### Upgrade for [Patch v5.0.11](https://news.blizzard.com/en-us/article/23893118/starcraft-ii-5-0-11-patch-notes)

#### Ravager
* Build time increased from 8.57 to 12.14 seconds and removed the random delay of up to 0.36 seconds.

#### Sentry
* Build time reduced from 26.4 to 22.9 seconds.

#### Forge
* Level 1 upgrades research time reduced from 128.6 to 121.6.
* Level 2 upgrades research time reduced from 153.6 to 144.6.
* Level 3 upgrades research time reduced from 178.6 to 167.9.

#### Liberator
* Cost reduced from 150/150 to 150/125.

#### Ghost
* Enhanced Shockwaves upgrade removed.

#### Banshee
* Hyperflight Rotors research time reduced from 121.4 to 100 seconds.
* Hyperflight Rotors cost reduced from 150/150 to 125/125.

#### Raven
* Gas cost reduced from 200 to 150.
* Build time reduced from 42.9 to 34.3 seconds.
* Corvid Reactor upgrade removed.
