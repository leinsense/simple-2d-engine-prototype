#include <windows.h>

int frameCount = 0;
int currentFPS = 0;
TCHAR fpsText[32] = TEXT("FPS: 0");

int posX = 100, posY = 100;
bool keyUp = false, keyDown = false, keyLeft = false, keyRight = false;

#define FPS_LOCK 60.0;
#define FRAME_CAP (1.0 / FPS_LOCK)
#define SPEED 200.0


struct Timer {
    LARGE_INTEGER freq;
    LARGE_INTEGER last;
    
    void Init() {
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&last);
    }
    
    double GetDelta(LARGE_INTEGER& now) {
        double dt = (double)(now.QuadPart - last.QuadPart) / freq.QuadPart;
        last = now;
        return dt;
    }
    
    double ToDelta(LARGE_INTEGER start, LARGE_INTEGER end) {
        return (double)(end.QuadPart - start.QuadPart) / freq.QuadPart;
    }
    
    DWORD ToMS(double sec) {
        return (DWORD)(sec * 1000.0);
    }
};

Timer frameTimer;

struct FramePerSec {
    LARGE_INTEGER lastCount;
    int lastFPS = -1;
    
    void Init(LARGE_INTEGER now) {
        lastCount = now;
    }
    
    void Update(LARGE_INTEGER now) {
        frameCount++;
        double elapsed = frameTimer.ToDelta(lastCount, now);
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
};

FramePerSec FPS;

inline void Update(double dt) {
    if (keyLeft)  posX -= (int)(SPEED * dt);
    if (keyRight) posX += (int)(SPEED * dt);
    if (keyUp)    posY -= (int)(SPEED * dt);
    if (keyDown)  posY += (int)(SPEED * dt);
}

inline void Render(HWND hwnd) {
    InvalidateRect(hwnd, NULL, TRUE);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE:
        return 0;

    case WM_KEYDOWN:
        if (wParam == VK_LEFT) keyLeft = true;
        if (wParam == VK_RIGHT) keyRight = true;
        if (wParam == VK_UP) keyUp = true;
        if (wParam == VK_DOWN) keyDown = true;
        return 0;

    case WM_KEYUP:
        if (wParam == VK_LEFT) keyLeft = false;
        if (wParam == VK_RIGHT) keyRight = false;
        if (wParam == VK_UP) keyUp = false;
        if (wParam == VK_DOWN) keyDown = false;
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

        HBRUSH greenBrush = CreateSolidBrush(RGB(0, 255, 0));
        RECT rect = { posX, posY, posX + 60, posY + 60 };
        FillRect(hdc, &rect, greenBrush);
        DeleteObject(greenBrush);

        SetTextColor(hdc, RGB(0, 0, 0));
        SetBkMode(hdc, TRANSPARENT);
        TextOut(hdc, 10, 10, fpsText, lstrlen(fpsText));

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

        FPS.Update(frameStart);
        Update(deltaTime);
        Render(hwnd);

        QueryPerformanceCounter(&frameEnd);
        double elapsed = frameTimer.ToDelta(frameStart, frameEnd);
        double sleepTime = FRAME_CAP - elapsed;
        if (sleepTime > 0) {
            DWORD ms = frameTimer.ToMS(sleepTime);
            if (ms > 0) Sleep(ms);
        }
    }

    return 0;
}