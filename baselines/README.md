# Baselines

Reference outputs from `scfusion-cli` per game-data version, for
directional sanity checks when the balance data changes.

```sh
./build/scfusion-cli --versions-dir main/Versions --time-limit 30 --quiet \
    baselines/targets/<target>.xml > baselines/<version>/<target>.txt
```

Note: the GA is time-seeded and multithreaded, so runs are NOT
bit-reproducible. Compare target-reached times and overall build shape,
not exact command sequences. (Total optimality drift between runs is
usually a second or two on these simple targets.)
