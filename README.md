# Particles

A real-time particle physics simulation built in C++ with SFML.

The goal is a sandbox where particles interact through basic physical forces — gravity, collisions, and eventually simple fluid-like behavior. Performance is a priority since the interesting stuff only happens at high particle counts.

## Build

Requires CMake 3.20+ and a C++17 compiler. SFML is fetched automatically.

```bash
cmake -S . -B build -G Ninja
cmake --build build
./build/Particles
```

On Linux, install the SFML system dependencies first:

```bash
sudo apt-get install libx11-dev libxrandr-dev libxi-dev libudev-dev \
  libopenal-dev libflac-dev libvorbis-dev libgl1-mesa-dev libfreetype-dev libxcursor-dev
```

## Controls

`Escape` — close the window
