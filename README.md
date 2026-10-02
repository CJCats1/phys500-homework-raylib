# PHYS 500 Homework 1-2 - raylib viewer

This is a C++17/raylib visualization project for the five landscapes in the
homework PDF. The viewer renders:

- the 3D surface `z = f(x, y)`;
- equipotential contour curves;
- a sampled gradient vector field on the xy plane;
- classified critical points with Hessian eigenvalues;
- a gradient-descent trajectory with an adjustable learning rate.

## Build

The CMake project uses an installed raylib when one is available. Otherwise,
it can download raylib 5.5 through CMake FetchContent on the first configure.

```bash
cmake -S . -B build
cmake --build build -j
./build/phys500_homework
```

If raylib is installed in a non-standard prefix, point CMake at it:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/raylib
```

You can also build against a local raylib source checkout without installing
it:

```bash
cmake -S . -B build \
  -DPHYS500_FETCH_RAYLIB=OFF \
  -DPHYS500_RAYLIB_SOURCE_DIR=/path/to/raylib
cmake --build build -j
```

To require an installed copy and avoid downloading anything:

```bash
cmake -S . -B build -DPHYS500_FETCH_RAYLIB=OFF
```

## Controls

- `1`-`5`: select a landscape
- `Left`/`Right`: select the previous/next landscape
- `R`: reset the gradient-descent start and trajectory
- `Up`/`Down`: increase/decrease the learning rate
- `Space`: pause/resume gradient descent
- `W`/`A`/`S`/`D`: orbit the camera
- mouse wheel: zoom
- `Esc`: quit

The right-hand panel is intentionally part of the visualization: it labels
the current landscape, prints the analytical gradient/Hessian, lists critical
points and their classifications, and shows the active descent parameters.

See [`docs/solution_notes.md`](docs/solution_notes.md) for the analytical
derivatives and numerical critical-point results used by the viewer.
