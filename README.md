# CPP Particles

A real-time particle physics sandbox built in C++ with SFML. The goal is a system where hundreds of particles interact through physically correct forces, gravity, elastic collisions, and eventually fluid-like pressure behavior. Performance is a first-class concern: the interesting behavior only emerges at scale.

## Features

- **Fixed timestep simulation** at 60 Hz with a spiral-of-death guard, keeping physics deterministic regardless of frame rate
- **Elastic collisions** with area-proportional mass, larger balls carry more momentum and push smaller ones further
- **Spatial grid acceleration** for collision detection, reducing complexity from O(n²) to O(n) by bucketing particles into a uniform hash grid and only testing neighbors
- **Variable ball sizes** (3 discrete radii) randomly assigned at spawn, with correct interaction radii in all collision paths
- **Hold-to-carry and throw**: drag a ball with the cursor, push others aside, release with velocity to throw it
- **Jolt button**: applies an upward impulse to all particles with random horizontal spread

## Build

Requires CMake 3.20+, a C++17 compiler, and Ninja. SFML 2.6.1 is fetched automatically via CMake FetchContent.

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/Particles
```

On Linux, install the SFML system dependencies first:

```bash
sudo apt-get install libx11-dev libxrandr-dev libxi-dev libudev-dev
libopenal-dev libflac-dev libvorbis-dev libgl1-mesa-dev libfreetype-dev libxcursor-dev
```

## Controls

| Input | Action |
|---|---|
| Left click | Spawn a ball that falls under gravity |
| Left click and drag | Carry a ball, push others aside, release to throw |
| Jolt button (top-left) | Launch all particles upward |
| Escape | Close |

## Roadmap

- Spatial grid is in place, next is pushing particle counts into the hundreds and adding floor friction / resting state
- Fluid-like pressure and density forces (SPH-inspired)
- Colour feedback based on particle speed or density
