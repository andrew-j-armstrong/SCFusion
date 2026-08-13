# STATE — where things actually stand (2026-08-12)

Non-obvious state that the repo alone doesn't tell you. Newest session first;
prune freely once items stop being true.

## Verified vs. not verified

- **Linux: fully verified.** CLI + GUI build, GUI smoke-tested interactively
  (dark theme, splitters, milestones, GA runs). CI re-verifies every push.
- **Windows: compile/link verified only.** CI proves MSVC v143 builds and
  packages the app, but **nobody has run the new code on real Windows** —
  the `std::thread` rewrite of `Core/`, the theme-aware colors, and
  WinSparkle behavior are all runtime-untested there. Easiest test: download
  the `SCFusion-win32` artifact from any green Actions run (it's a runnable
  app: Fusion.exe + DLLs + Versions/) and click around.
- **macOS: never attempted.** Nothing known to block wxOSX, but zero data.

## Release bookkeeping (v2.3.0 is NOT released)

- CHANGELOG says "Unreleased (v2.3.0)". The README release checklist has NOT
  been run: version strings in `MDIParent.cpp` and `SCFusion.rc` still say
  the old version; Installer and appcast untouched.
- `release/2.3.0` branch still exists (local + origin) even though it's
  merged to master — the repo's convention keeps release branches around.

## Traps and pins that will bite if forgotten

- **wxWidgets 3.1.4 is pinned on Windows** because the vcxproj deploys
  `wx*314u_vc14x.dll` by literal filename. Upgrading wx on Windows means
  editing those `CopyFileToFolders` entries + `wxwidgets.props` +
  `.github/workflows/ci.yml` (cache key too) in the same change.
- **The official wx archives ship their own `wxwidgets.props`** which will
  silently clobber ours if extracted carelessly (CI excludes it; humans
  following download.txt must too).
- **`RAND_SSE_MAX` (0x7FFF), never `RAND_MAX`,** in GA scaling code — glibc's
  RAND_MAX silently kills evolution. Same trap awaits any new random-scaling
  code.
- **wxGTK sizes windows asynchronously** — anything geometric done in a
  constructor (splitter positions etc.) needs `CallAfter`. There are two
  copies of `IsDarkTheme()` (GridOutput.cpp, ChartPanel.cpp) — promote to a
  header if a third appears.
- GTK "Negative content width" warnings at GUI startup are known cosmetic
  noise from wx's emulated MDI; wxLog debug spam is intentionally filtered
  in `Application.cpp` (`wxLog::SetLogLevel(wxLOG_Warning)`).

## Environment (this dev box, Void Linux)

- Installed for this work: `wxWidgets-common-devel`, `wxWidgets-gtk3-devel`.
  POST_PLAN.md says to uninstall them when done — but the Linux GUI build
  now depends on them, so keeping them is the likely call. Decide, then
  delete POST_PLAN.md.

## Possible next moves (nobody committed to these)

- Upstream: this fork's work is a candidate PR to andrew-j-armstrong/SCFusion
  (see upstream issue #30 asking for exactly this Linux support).
- PLAN.md Phase 6 leftovers: TUI, wx-free core, memory-pool profiling vs
  modern allocators, ARM/xoshiro PRNG, Rust rewrite.
- Dead code candidates for deletion: `main/OutputFormat.cpp`,
  `main/SC2/SC2AStar.*`, `main/VisualItem.*` (nothing compiles them).
