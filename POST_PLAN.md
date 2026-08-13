# POST_PLAN: cleanup after the port + 5.0.16 work

Packages installed on Void Linux for this project (2026-08-12) that weren't
there before:

- `wxWidgets-common-devel` (pulls in `wxWidgets-common`)
- `wxWidgets-gtk3-devel` (pulls in `wxWidgets-gtk3`)

When the work is done (or abandoned), remove them and any dependencies they
dragged in:

```sh
sudo xbps-remove -R wxWidgets-common-devel wxWidgets-gtk3-devel
sudo xbps-remove -o   # sweep any remaining orphans
```

Note: only do this if nothing else on the system ends up using wxWidgets —
`xbps-remove -R` will refuse to remove packages something else depends on, so
it's safe to just run it.

Everything else used for the build (gcc, cmake, make, pkg-config, gdb, git)
was already installed before this project and should stay.
