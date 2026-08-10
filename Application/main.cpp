#include "Core/Common/CoreMinimal.h"
#include "Core/Timer.h"
#include "Core/SystemInput.h"

#include "Graphics/D3D12/DX12Renderer.h"

// Forward declaration of windows procedure to handle OS messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Window class name, array of chars
	const wchar_t windowClassName[] = L"VictoryEngineApplication";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = windowClassName;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Prevent loading icon stutter

	RegisterClass(&wc);

	// Create window
	HWND hwnd = CreateWindowEx(
		0,								// Window styles
		L"VictoryEngineApplication",	// Window class
		windowClassName,				// Window title text
		WS_OVERLAPPEDWINDOW,			// Window style (resizable & close button)
		CW_USEDEFAULT, CW_USEDEFAULT,	// Position on screen
		1280, 720,						// Width & height
		nullptr,						// Parent window
		nullptr,						// Menu
		hInstance,						// Instance handle
		nullptr); 						// Excess app data

	if (hwnd == nullptr)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);

	SystemInput::Initialize(hwnd);

	VictoryRenderer renderer;
	bool dx12Initialised = renderer.Initialize(hwnd, 1280, 720);

	if (!dx12Initialised) 
	{
		MessageBox(hwnd, L"DirectX 12 Initialisation failed!", L"Engine error", MB_OK | MB_ICONERROR);
		return 0;
	} 
	else 
	{
		OutputDebugString(L"[Victory Engine] DirectX 12 Device, SwapChain, and Heaps initialized perfectly!\n");
	}

	// Timer for deltaTime
	Timer timer;
	timer.Reset();

	float defaultBackgroundColour[4] = { 0.1f, 0.15f, 0.23f, 1.0f };
	float targetColour[4] = { 0.7f, 0.2f, 0.2f, 1.0f };

	float currentBackgroundColor[4] = { 0.1f, 0.15f, 0.23f, 1.0f };

	// Non blocking window (need ts for it to be a game engine)
	MSG msg = {};
	bool isRunning = true;

	while (isRunning)
	{
		// Peek message required, get message freezes frame until a mouse event etc occurs, not ideal
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Check if we exited early from the message loop
		if (!isRunning)
		{
			break;
		}

		// Tick timer at start of frame
		timer.Tick();
		float deltaTime = timer.DeltaTime();

		float* activeTarget = SystemInput::IsKeyDown('W') ? targetColour : defaultBackgroundColour;
		// 1. Polling update system for input state (keyboard, mouse, gamepad)
		// 2. Engine frame starts here (Update game logic, physics)
		// 3. Render DX12 frame
		float fadeSpeed = 5.0f * deltaTime;
		for (int i = 0; i < 3; ++i)
		{
			currentBackgroundColor[i] += (activeTarget[i] - currentBackgroundColor[i]) * fadeSpeed;
		}

		// 3. Render frame with our dynamic values
		renderer.RenderFrame(currentBackgroundColor);

		SystemInput::ClearMouseDeltas();

	}

	renderer.Shutdown();
	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INPUT:
	{
		SystemInput::ProcessRawInput(lParam);
		
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		ValidateRect(hwnd, nullptr);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}