#pragma once
#include "Core/Common/CoreMinimal.h"

class SystemInput
{
public:
	static bool Initialize(HWND hwndTarget);

	// Message hook for processing raw input from the OS, called from winproc in main.cpp
	static void ProcessRawInput(LPARAM lParam);

	static void ClearMouseDeltas();

	static bool IsKeyDown(int keyCode);
	static void GetRawMouseDeltas(LONG& dX, LONG& dY);

private:
	static bool keyStates_[256]; // Track key states for all virtual key codes
	
	static LONG mouseDeltaX_;
	static LONG mouseDeltaY_;
};


