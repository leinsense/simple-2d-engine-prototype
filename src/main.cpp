#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#include "entity.h"
#include "frame_timer.h"
#include "input.h"

bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;
bool keySpace = false, keyShoot = false;

FrameTimer frameTimer;
FrameCounter FPS;

inline void Render(HWND hwnd) {
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        return 0;

    case WM_KEYDOWN:
        switch(wParam){
            case VK_LEFT:
            case 'A': keyLeft = true; break;

            case VK_RIGHT:
            case 'D': keyRight = true; break;

            case VK_SPACE: keySpace = true; break;
            case 'Z':keyShoot = true; break;
        }

        return 0;
        
    case WM_KEYUP:
        switch(wParam){
            case VK_LEFT:
            case 'A': keyLeft = false; break;

            case VK_RIGHT:
            case 'D': keyRight = false; break;

            case VK_SPACE: keySpace = false; break;
            case 'Z':keyShoot = false; break;
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        RenderEntities(hdc);

        // Ground
        HPEN pen = CreatePen(PS_SOLID, 2, RGB(0,0,0));
        SelectObject(hdc, pen);
        MoveToEx(hdc, 0, 500, NULL);
        LineTo(hdc, 800, 500);
        DeleteObject(pen);


        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        TextOut(hdc, 10, 10, FPS.fpsText, lstrlen(FPS.fpsText));

        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    timeBeginPeriod(1);
    const TCHAR CLASS_NAME[] = TEXT("MyWindowClass");
    WNDCLASS wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, TEXT("Engine with Delta Time & FPS"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    AddEntity(ENTITY_PLAYER, 100, 100, RGB(0, 255, 0)); // player
    AddEntity(ENTITY_ENEMY, 300, 300, RGB(255, 0, 0));  // enemy


    MSG msg = {};
    frameTimer.Init();
    FPS.Init(frameTimer.last);

    while (true) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        LARGE_INTEGER frameStart, frameEnd;
        QueryPerformanceCounter(&frameStart);

        double deltaTime = frameTimer.GetDelta(frameStart);

        FPS.Update(frameStart, frameTimer);
        UpdateEntities(deltaTime);
        Render(hwnd);

        QueryPerformanceCounter(&frameEnd);
        double elapsed = frameTimer.ToDelta(frameStart, frameEnd);
        double sleepTime = FRAME_CAP - elapsed;

        if (sleepTime > 0) {
            DWORD ms = frameTimer.ToMS(sleepTime);
            if (ms >= 2)
                Sleep(ms - 1);

            do {
                QueryPerformanceCounter(&frameEnd);
                elapsed = frameTimer.ToDelta(frameStart, frameEnd);
            } while (elapsed < FRAME_CAP);
        }
    }

    timeEndPeriod(1);
    return 0;
}