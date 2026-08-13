# PLAN: Linux port + StarCraft II 5.0.16 update

> **Status (2026-08-12): Phases 1–5 done**, plus two Phase 6 items: the
> wxGTK GUI runs on Linux (`./build/scfusion-gui`, built automatically when
> full wxWidgets is installed) and GitHub Actions CI covers a Linux
> build+smoke and a Windows MSBuild compile check. See CHANGELOG.md
> (Unreleased v2.3.0). Remaining Phase 6 items (TUI, wx-free core, memory
> pool profiling, ARM PRNG, Rust) stay open.

Two goals, one critical path:

1. **Portable** — build and run on Linux (then macOS/Windows), headless first, GUI later.
2. **Current** — apply the 5.0.16 balance patch to the game data/mechanics.

Order matters: the headless port comes first because it's the only way to
validate XML/mechanics changes on Linux (no tests exist; the only validation
loop is running the engine). The port work is a strict subset of the long-term
portability goal — nothing here is throwaway.

Key shortcut: wxWidgets is cross-platform and the engine layers only use
**wxBase** types (`wxString`, `wxXmlNode` — no GUI classes). So the headless
port links against wxBase and does NOT require stripping wx out of the SC2
layer. Only the Win32-specific code in `Core/` needs rewriting.

---

## Phase 1 — Minimal headless Linux build

Goal: `cmake && make && ./scfusion-cli` runs a GA optimization on Linux.

### 1.1 CMake

- [ ] Top-level `CMakeLists.txt`; library target `scfusion-core` from
      `main/Core/`, `main/GA/`, `main/SC2/`, `main/AStar/`.
- [ ] `find_package(wxWidgets COMPONENTS base xml)` — wxBase only, no GUI.
- [ ] Precompiled header `main/stdafx.h` may need a slim non-Windows variant
      (it currently pulls wx GUI headers — check and split if needed).
- [ ] Keep MSVC build working (either leave .sln alone or generate from CMake
      later; don't block on it).

### 1.2 Port Win32-isms in Core/ (the only real code work)

- [ ] `Core/ThreadPool.{h,cpp}` — `CreateThread`/`HANDLE`/`WaitForSingleObject`/
      semaphores → `std::thread` + `std::mutex`/`std::condition_variable`
      (or `std::counting_semaphore`, C++20).
- [ ] `Core/Lock.h` — `CRITICAL_SECTION`/`Interlocked*` → `std::mutex`/`std::atomic`.
- [ ] `Core/LockFreeCircularQueue.h`, `Core/LockFreeMemoryPool.*` —
      `Interlocked*`/`volatile` → `std::atomic` with explicit memory ordering.
- [ ] `Core/MemoryPool.h` — `HANDLE m_semaphore` → std equivalent.
- [ ] `__declspec(thread)` → `thread_local` (`Core/Vector.h`, `MemoryPool*`,
      `MemoryPoolManager.h`).
- [ ] `Core/FastRand.cpp` — `__declspec(align(16))` → `alignas(16)`. SSE
      intrinsics compile fine on Linux x86-64 (`-msse4`); ARM/Apple Silicon can
      wait, or swap in xoshiro256++ behind the same interface (as fast, fully
      portable).
- [ ] MSVC-isms long tail: `stricmp`→`strcasecmp`, `sprintf_s`, include-path
      case sensitivity, permissive template code. Expect many small errors, no
      hard problems. Build with GCC or Clang, `-O2`, fix until clean.

### 1.3 CLI runner (`scfusion-cli`)

Thin frontend replacing what `MDIChild`/`SC2Engine` do via the GUI. Model it on
`CSC2Engine` (main/SC2Engine.cpp) but without wxPropertyGrid — the engine's
GUI coupling is exactly the part to bypass; call `CSC2FitnessCalc`/`CGAEngine`
directly.

- [ ] Load a version XML from `Versions/` (same discovery as
      `MDIParent::LoadVersions`, minus wxDir if inconvenient).
- [ ] Read target spec: reuse the save-file XML format (`main/SampleSaveFile.xml`)
      so existing saves work as inputs.
- [ ] Run GA: configure villages (one per core, as the GUI does), run until
      stagnation limit or `--time-limit`.
- [ ] Output: best build order + timings to stdout (reuse `CSC2OutputMinimal`);
      print village/city stats to stderr periodically.
- [ ] `--seed` flag for reproducible runs if feasible (FastRand is global;
      good enough for regression use).

**Done when:** a Protoss, Terran, and Zerg sample target each produce a
plausible build order on Linux.

---

## Phase 2 — Baseline (before touching data)

- [ ] Commit sample target files for standard openings, one per race
      (e.g. reach X units by Y time).
- [ ] Run each vs current 5.0.15 data; save outputs to `baselines/5.0.15/`.
- [ ] If a Windows machine is handy: sanity-check one run against the Windows
      GUI build for parity. Optional, not blocking.

---

## Phase 3 — 5.0.16 data changes (XML only)

All edits in `main/Versions/StarCraft.xml`. Bump `<MinVersion>` to 5.0.16.
Patch-note triage — what this simulator actually models:

### Economy
- [ ] Starting workers 12 → 8: `GameStartCount` for Probe (~line 83),
      SCV (~1606), Drone (~3602).
- [ ] Mineral patch / geyser resource totals: **skip** — income model
      (`SC2/GameCalcs.cpp`) is a saturation-rate table with no depletion.
      Note it in the commit message so nobody hunts for it later.
- [ ] Rich geyser return 8 → 6: skip (not modeled).

### Protoss
- [ ] Nexus `ProvidedSupply` 15 → 13.
- [ ] Warp-in time: `UnitComplete time="3.6"` → `4` on all five
      "Build X At Warp Gate" commands.
- [ ] Warpgate cooldowns (`AppliesSourceBuildingStatus duration=` on warp-in
      commands): Zealot 20→22, Adept 20→22, Stalker 23→22, Sentry 23→22,
      High Templar 32→35, Dark Templar 32→35.
- [ ] Shield Battery: `StartingEnergy` 75 (currently absent or different).
- [ ] "Convert Gateway To Warp Gate" cost 25/25 — see Phase 4 for the
      one-time-cost modeling question.

### Terran
- [ ] Command Center `ProvidedSupply` 15 → 13.
- [ ] Ghost supply 2 → 3.

### Zerg
- [ ] Hatchery/Lair/Hive supply 6 → 4; Hatchery cost 275 → 300.
- [ ] Larva spawn rate — **judgment call**: patch says 10.7 → 9.5s but XML has
      `LarvaeSpawnTime` 11.0, so the XML doesn't mirror Blizzard's raw number.
      Investigate what the 11.0 represents (overhead?). Either scale
      (11.0 × 9.5/10.7 ≈ 9.77) or set 9.5. Document the choice in CHANGELOG.
- [ ] Carapace upgrades: L1 150/150→100/100, L2 200/200→150/150,
      L3 250/250→200/200 (if present in XML).
- [ ] Microbial Shroud: remove upgrade research requirement (added in 5.0.15;
      check how it was modeled then and revert).
- [ ] Overlord speed 0.9 → 0.85: skip unless scouting modeling uses it.

### Not modeled — skip entirely
Map pool, all combat stats (damage/range/health/speed except supply changes
above), subgroup priority, and every Bug Fix / QoL item.

---

## Phase 4 — 5.0.16 mechanics change (the one real one)

**Warpgate Research now speeds up Gateway unit production by 40%.**
Post-research Gateway times: Zealot 16, Adept 18, Stalker 16, Sentry 14,
HT 26, DT 26. Gateway build times are now conditional on research state —
currently every command has a fixed duration.

- [ ] Investigate whether existing machinery can express it:
      - Building statuses support `ProductionBoostFactor` (chrono boost = 1.5×).
        Can a research apply a permanent status to all buildings of a type?
        Check `SC2Research.cpp` / `SC2BuildingStatus.cpp` for
        research-completion side effects.
      - Alternative: duplicate commands — "Build Zealot At Gateway (fast)" with
        `ResearchRequirement`, original gated on research *absent*. Check
        whether a research-absent requirement exists (`RequiresSourceBuildingStatusAbsent`
        exists for statuses; research equivalent may not).
- [ ] If neither works: small C++ addition (research completion event applies a
      status or swaps command durations). Keep it generic — XML-driven, in the
      spirit of the existing design.
- [ ] One-time 25/25 Warpgate transform cost: cheap approximation is putting
      25/25 on the "Warp Gate Transformation" research itself (one-time by
      definition). Exact modeling (first conversion costs, later free) needs
      per-building-count state — probably not worth it; document the
      approximation.

---

## Phase 5 — Validate

- [ ] Re-run Phase 2 targets vs 5.0.16 data → `baselines/5.0.16/`.
- [ ] Diff vs 5.0.15 baselines. Everything shifts (8 workers!) — check shifts
      are directionally sane: early game uniformly slower, Zerg larva timing
      coherent, warpgate-vs-gateway tradeoff visible for Protoss.
- [ ] Update `CHANGELOG.md` (new version entry, 5.0.16 patch summary, note
      skipped/approximated items).
- [ ] Update version per README release process if cutting a release.

---

## Phase 6 — Later / optional (not on the critical path)

- [x] Full GUI on Linux: link full wxGTK, port `MDIParent`/`MDIChild`
      (`#ifdef` out WinSparkle) — done; macOS/wxOSX untested but nothing
      known to block it.
- [ ] TUI frontend (FTXUI) over the same core lib.
- [ ] Strip wx types from SC2 layer (std::string + pugixml) for a truly
      dependency-free core.
- [ ] Profile on Linux: the custom `MemoryPoolManager` predates modern
      allocators — measure whether it still earns its complexity.
- [ ] ARM/Apple Silicon: portable PRNG (xoshiro256++), drop SSE requirement.
- [x] CI: GitHub Actions running the CLI regression targets on every push
      (plus a Windows MSBuild compile check of the VS project).
- [ ] Rust rewrite: only after the CLI exists as a parity reference. Separate
      project, separate decision.
