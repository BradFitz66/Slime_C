# Slime

A remake of my other Rocket Slime reverse engineering project in C

A lot of this code is ugly (not very familiar with C), and a lot of the values (speed, jump force, etc) are magic numbers derived from what I've seen from Ghidra & emulator memory while reverse engineering the game

This isn't meant to be a 1-to-1 binary copy of the game (i.e. like the sm64 decompilation), but rather a reimplementation with a focus on accuracy. Reverse engineering via Ghidra & emulators is used to locate the exact values used for specific stuff such as jump force, movement speed, etc.

The end goal of this specific project is to have enough groundwork to recreate the tank battle minigame from Rocket Slime. Anything else is a stretch goal and is mostly limited by the fact there's currently no easy way to rip tilemaps from the ROM without a lot of manual work.

# Building
Program is built with CMake using the Visual Studio 2022 configuration. Every library is retrieved via FetchContent or provided inside /vendored.
Program has only been tested within Windows, no support for building on Linux or MacOS. 

# Current status

Rocket:

- [x] Loading and rendering of Rocket from a sprite sheet

- [x] Animation loading and running from a sprite sheet

- [ ] Complete recreation of rocket's character controller

    - [x] Idle state

    - [x] Walking state

    - [x] Jumping state

    - [x] Floating state

    - [ ] Stretching state

    - [ ] Elastoblast state

    - [ ] Charged elastoblast state

    - [ ] Wall bounce state

Graphics:

- [ ] Automated way of ripping sprites from a copy of the ROM (work being done by 0Unique on Github) 

- [x] Sprite sheet of Rocket (already existed - ripped directly from the ROM so should be 100% accurate)

- [ ] Some sort of map built from tiles ripped from the game

- [ ] Enemy sprites 

- [ ] Other character sprites (hooly, swotsy, etc)
    