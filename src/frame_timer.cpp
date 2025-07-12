#include "frame_timer.h"
//#include <windows.h>

// Global variables
int frameCount = 0;
int currentFPS = 0;

void FrameTimer::Init() {
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);
}

double FrameTimer::GetDelta(LARGE_INTEGER& now) {
    double dt = static_cast<double>(now.QuadPart - last.QuadPart) / freq.QuadPart;
    last = now;
    return dt;
}

double FrameTimer::ToDelta(LARGE_INTEGER start, LARGE_INTEGER end) {
    return static_cast<double>(end.QuadPart - start.QuadPart) / freq.QuadPart;
}

DWORD FrameTimer::ToMS(double sec) {
    return static_cast<DWORD>(sec * 1000.0);
}

void FrameCounter::Init(LARGE_INTEGER now) {
    lastCount = now;
}

void FrameCounter::Update(LARGE_INTEGER now, FrameTimer& fr) {
    frameCount++;
    double elapsed = static_cast<double>(now.QuadPart - lastCount.QuadPart) / static_cast<double>(fr.freq.QuadPart);
    if (elapsed >= 1.0) {
        currentFPS = frameCount;
        frameCount = 0;
        lastCount = now;
        if (currentFPS != lastFPS) {
            wsprintf(fpsText, TEXT("FPS: %d"), currentFPS);
            lastFPS = currentFPS;
        }
    }
}

int FrameCounter::GetFPS() const {
    return currentFPS;
}
