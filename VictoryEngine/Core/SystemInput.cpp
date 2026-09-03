#include "SystemInput.h"

// Initialize all keys to not pressed and mouse deltas to zero
bool SystemInput::keyStates_[256] = { false }; 
LONG SystemInput::mouseDeltaX_ = 0;
LONG SystemInput::mouseDeltaY_ = 0;

void SystemInput::ClearMouseDeltas()
{
	// Reset deltas so mouse movmeent don't carry over into frames with no movement
	mouseDeltaX_ = 0;
	mouseDeltaY_ = 0;
}
bool SystemInput::IsKeyDown(int keyCode) 
{
	if(keyCode >= 0 && keyCode < 256)
	{
		return keyStates_[keyCode];
	}
	return false;
}
void SystemInput::GetRawMouseDeltas(LONG& oX, LONG& oY) 
{
	oX = mouseDeltaX_;
	oY = mouseDeltaY_;
}

bool SystemInput::Initialize(HWND hwndTarget)
{
	// Register for raw input
	RAWINPUTDEVICE rawInputDevices[2];

	// Mouse config
	rawInputDevices[0].usUsagePage = 0x01;
	rawInputDevices[0].usUsage = 0x02;
	rawInputDevices[0].dwFlags = 0;
	rawInputDevices[0].hwndTarget = hwndTarget;

	// Keyboard config
	rawInputDevices[1].usUsagePage = 0x01;
	rawInputDevices[1].usUsage = 0x06;
	rawInputDevices[1].dwFlags = 0;
	rawInputDevices[1].hwndTarget = hwndTarget;

	if(RegisterRawInputDevices(rawInputDevices, 2, sizeof(rawInputDevices[0])) == FALSE)
	{
		// Registration failed 
		return false;
	}
	return true;
}

void SystemInput::ProcessRawInput(LPARAM lParam) 
{
	UINT size = 0;

	// First call determines the required buffer size
	GetRawInputData(
		(HRAWINPUT)lParam,
		RID_INPUT,
		nullptr,
		&size,
		sizeof(RAWINPUTHEADER));

	if(size > 0)
	{
		// Allocate dynamic buffer on stack for input packet
		std::unique_ptr<BYTE[]> buffer = std::make_unique<BYTE[]>(size);

		// Second call populates the data buffer
		if(GetRawInputData((HRAWINPUT)lParam, RID_INPUT, buffer.get(), &size, sizeof(RAWINPUTHEADER)) == size)
		{
			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.get());

			// Handle raw mouse movement
			if(raw->header.dwType == RIM_TYPEMOUSE)
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
			else if(raw->header.dwType == RIM_TYPEKEYBOARD)
			{
				USHORT keyCode = raw->data.keyboard.VKey;
				USHORT flags = raw->data.keyboard.Flags;

				bool isKeyPressed = (flags & RI_KEY_BREAK) == 0;

				if (keyCode < 256) 
				{
					keyStates_[keyCode] = isKeyPressed;
				}
			}
		}
	}
}