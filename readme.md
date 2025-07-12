# Simple 2D Game Engine Prototype ( WinAPI )
A lightweight 2D game engine prototype built using C++, WinAPI, and GDI — made to explore manual game loops, delta time, frame limiting, basic entity management, and native sound playback.
Fully written from scratch without external libraries or frameworks.

### Features
- Manual game loop (PeekMessage)
- High-precision delta time (QueryPerformanceCounter)
- Frame limiter (locked at 90 FPS)
- Real-time FPS counter
- Basic entity system (player, enemy, bullet)
- Simple gravity and jumping
- Collision detection
- Sound playback with .wav files (PlaySound)
- Rendering with WinAPI GDI (FillRect, TextOut)

### Purpose
Designed for learning and experimentation with low-level game programming in the Windows environment — without any external libraries

### Screenshot

![File Image](screenshot.png)
### Build Instructions
Use MinGW-w64 or any C++ compiler for Windows
#### Example:
```
g++ src/*.cpp -o run.exe -lgdi32 -mwindows -lwinmm
```