#pragma once
#include <windows.h>

#define FPS_LOCK 90.0
#define FRAME_CAP (1.0 / FPS_LOCK)

class FrameTimer {
public:
    LARGE_INTEGER freq;
    LARGE_INTEGER last;

    void Init();
    double GetDelta(LARGE_INTEGER& now);
    double ToDelta(LARGE_INTEGER start, LARGE_INTEGER end);
    DWORD ToMS(double sec);
};

class FrameCounter {
private:
    int lastFPS = -1;
public:
    TCHAR fpsText[32] = TEXT("FPS: ...");
    LARGE_INTEGER lastCount;

    void Init(LARGE_INTEGER now);
    void Update(LARGE_INTEGER now, FrameTimer& fr);
    int GetFPS() const;
};

extern int frameCount;
extern int currentFPS;