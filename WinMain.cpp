#include <Windows.h>
#include <gl/GL.h>
#include <chrono>
#include <fstream>
#include <thread>
#include <random>
#include <cmath>

#include "settings.h"
#include "trayUtils.h"
#include "DesktopUtils.h"
#include "rendering.h"
#include "color.h"

constinit bool running = true;

typedef struct FPOINT {
	float x;
	float y;
};

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	if (msg == WM_TRAYICON) {
		if (lParam == WM_RBUTTONUP) {
			// Create a popup menu
			HMENU menu = CreatePopupMenu();
			AppendMenuW(menu, MF_STRING, 1, L"Quit");

			// Get the cursor position
			POINT cursorPos;
			GetCursorPos(&cursorPos);

			// Show the menu
			SetForegroundWindow(hwnd);
			// Example with TPM_NONOTIFY to avoid blocking
			int selection = TrackPopupMenu(menu, TPM_RETURNCMD | TPM_NONOTIFY, cursorPos.x - 120, cursorPos.y - 22, 0, hwnd, nullptr);
			DestroyMenu(menu);

			// Handle the menu selection
			if (selection == 1) {
				OnQuit(&hwnd, &running);
			}
		}
	}

	return DefWindowProcW(hwnd, msg, wParam, lParam);
}

static inline void checkEvents(MSG& msg, bool& running) {
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		if (msg.message == WM_QUIT) {
			running = false;
			break;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

static inline void gameTick(float& frameTime, const float& stepInterval, float& fractionalTime) {
	if (frameTime < stepInterval) {
		// Calculate total sleep time including any leftover fractional time
		float totalSleepTime = (stepInterval - frameTime) + fractionalTime;

		// Truncate to whole milliseconds
		int sleepMilliseconds = static_cast<int>(totalSleepTime * 1e+3f);

		// Calculate remaining fractional time and ensure it�s within 0.0f to 1.0f
		fractionalTime = (totalSleepTime - sleepMilliseconds * 1e-3f);

		// Sleep for the calculated milliseconds
		std::this_thread::sleep_for(std::chrono::milliseconds(sleepMilliseconds));
	}
}


int CALLBACK WinMain(
	_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	const wchar_t* className = L"InifnityMirrorClass";
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = className;
	wc.hIconSm = nullptr;

	RegisterClassEx(&wc);

	const float Width  = static_cast<float>(GetSystemMetrics(SM_CXVIRTUALSCREEN));
	const float Height = static_cast<float>(GetSystemMetrics(SM_CYVIRTUALSCREEN));
	const int leftMost = GetSystemMetrics(SM_XVIRTUALSCREEN);
	const int topMost = GetSystemMetrics(SM_YVIRTUALSCREEN);

	HWND hwnd = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		className,
		L"Infinity Mirror",
		WS_POPUP,
		0, 0,
		Width, Height,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	if (!hwnd) return -1;

	const Settings settings = loadSettings("settings.json");

	const float stepInterval = 1.0f / settings.targetFPS;

	// Load the icon from resources
	HICON hIcon = LoadIconFromResource();

	// Add the tray icon
	AddTrayIcon(&hwnd, &hIcon, L"Just a Simple Icon");

	wchar_t* originalWallpaper = GetCurrentWallpaper();

	SetAsDesktop(hwnd);

	ShowWindow(hwnd, SW_SHOW);

	HDC hdc = GetDC(hwnd); // Get device context

	// Step 1: Set up the pixel format
	PIXELFORMATDESCRIPTOR pfd = {};
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 24;
	pfd.cStencilBits = 8;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int pixelFormat = ChoosePixelFormat(hdc, &pfd);
	SetPixelFormat(hdc, pixelFormat, &pfd);

	// Step 2: Create and set the rendering context
	HGLRC hglrc = wglCreateContext(hdc);
	wglMakeCurrent(hdc, hglrc);
	// OpenGL is now set up for this window!

	Color colorObj(settings.colors, settings.colorSpeed);

	float dt{ 0 };
	float frameTime{ 0 };
	float fractionalTime{ 0 };

	auto newF = std::chrono::high_resolution_clock::now();
	auto oldF = std::chrono::high_resolution_clock::now();
	auto endF = std::chrono::high_resolution_clock::now();

	// Message loop
	MSG msg = {};
	while (running) {
		// get delta time
		oldF = newF;
		newF = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<float>(newF - oldF).count();

		// Check for messages
		checkEvents(msg, running);

		// draw background
		glClearColor(settings.bgColor[0], settings.bgColor[1], settings.bgColor[2], settings.bgColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		POINT mouse_pos;
		GetCursorPos(&mouse_pos);

		mouse_pos.x -= leftMost;
		mouse_pos.y -= topMost;

		colorObj.update(dt);

		float scaleFactor = 1.0f;
		for (int i = 0; i < settings.iterations; ++i) {
			float rScaleFactor = -scaleFactor;
			float i_f = static_cast<float>(i);
			float i_sqrt = sqrtf(i_f);
			float cx = (mouse_pos.x / Width * 2.0f - 1.0f) * (i_sqrt / settings.iterations);
			float cy = -(mouse_pos.y / Height * 2.0f - 1.0f) * (i_sqrt / settings.iterations);

			aaline(colorObj.r, colorObj.g, colorObj.b, colorObj.a / i_f, cx - rScaleFactor, cy - scaleFactor, cx + scaleFactor, cy + scaleFactor, settings.borderWidth);
			aaline(colorObj.r, colorObj.g, colorObj.b, colorObj.a / i_f, cx - scaleFactor, cy - scaleFactor, cx + scaleFactor, cy + rScaleFactor, settings.borderWidth);
			aaline(colorObj.r, colorObj.g, colorObj.b, colorObj.a / i_f, cx - scaleFactor, cy - rScaleFactor, cx + rScaleFactor, cy + rScaleFactor, settings.borderWidth);
			aaline(colorObj.r, colorObj.g, colorObj.b, colorObj.a / i_f, cx - rScaleFactor, cy - rScaleFactor, cx + rScaleFactor, cy + scaleFactor, settings.borderWidth);

			scaleFactor *= settings.scaleMultiplier;
		}

		SwapBuffers(hdc);

		endF = std::chrono::high_resolution_clock::now();
		frameTime = std::chrono::duration<float>(endF - newF).count();

		gameTick(frameTime, stepInterval, fractionalTime);
	}

	SetParent(hwnd, nullptr);
	SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (void*)originalWallpaper, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);

	wglMakeCurrent(nullptr, nullptr);
	wglDeleteContext(hglrc);
	ReleaseDC(hwnd, hdc);

	DestroyWindow(hwnd);
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	delete[] originalWallpaper;
	DestroyIcon(hIcon);

	return 0;
}
