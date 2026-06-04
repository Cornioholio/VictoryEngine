#include "../Core/Common/CoreMinimal.h"
#include "../Core/Timer.h"

// Forward declaration of windows procedure to handle OS messages
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Window class name, array of chars
	const wchar_t windowClassName[] = L"GameEngineWindow";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = windowClassName;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW); // Prevent loading icon stutter

	RegisterClass(&wc);

	// Create window
	HWND hwnd = CreateWindowEx(
		0,								// Window styles
		L"GameEngineWindow",			// Window class
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

	// Register for raw input
	RAWINPUTDEVICE rawInputDevices[2];

	// Mouse config
	rawInputDevices[0].usUsagePage = 0x01; // Desktop controls
	rawInputDevices[0].usUsage = 0x02;	   // Mouse
	rawInputDevices[0].dwFlags = 0;		   // Standard tracking
	rawInputDevices[0].hwndTarget = hwnd;

	// Keyboard config
	rawInputDevices[1].usUsagePage = 0x01; 
	rawInputDevices[1].usUsage = 0x06;	   // Keyboard
	rawInputDevices[1].dwFlags = 0;
	rawInputDevices[1].hwndTarget = hwnd;

	if (RegisterRawInputDevices(rawInputDevices, 2, sizeof(rawInputDevices[0])) == FALSE)
	{
		// Registration failed 
		OutputDebugStringA("Failed to register raw input devices.\n");
	}

	// Timer for deltaTime
	Timer timer;
	timer.Reset();

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

		// 1. Polling update system for input state (keyboard, mouse, gamepad)
		// 2. Engine frame starts here (Update game logic, physics)
		// 3. Render DX12 frame

	}

	return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch(uMsg) 
	{
	case WM_INPUT: 
	{
		UINT size = 0;

		// First call determines the required buffer size
		GetRawInputData(
			(HRAWINPUT)lParam,      
			RID_INPUT,				
			nullptr,				
			&size,					 
			sizeof(RAWINPUTHEADER)); 

		if (size > 0) 
		{
			// Allocate dynamic buffer on stack for input packet
			std::unique_ptr<BYTE[]> buffer = std::make_unique<BYTE[]>(size);

			// Second call populates the data buffer
			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.get(), &size, sizeof(RAWINPUTHEADER)) == size) 
			{
				RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.get());

				// Handle raw mouse movement
				if (raw->header.dwType == RIM_TYPEMOUSE) 
				{
					// Relative movements
					LONG dX = raw->data.mouse.lLastX;
					LONG dY = raw->data.mouse.lLastY;

					// Mouse button handling
					USHORT buttonFlags = raw->data.mouse.usButtonFlags;
					if (buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) 
					{
						// Handle left button down
					}
				}
				else if (raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					USHORT keyCode = raw->data.keyboard.VKey;
					USHORT flags = raw->data.keyboard.Flags;

					bool isKeyPressed = (flags & RI_KEY_BREAK) == 0;

					if (keyCode == 'W') 
					{
						// Move forward state
						OutputDebugStringA(isKeyPressed ? "W pressed\n" : "W released\n");
					}
				}
			}
		}

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