#pragma once
#include "Core/Common/CoreMinimal.h"

class VICTORY_API SystemInput
{
public:
	static bool Initialize(HWND hwndTarget);

	// Message hook for processing raw input from the OS, called from winproc in main.cpp
	static void ProcessRawInput(LPARAM lParam);

	// Get and clear updates in mouse movement
	static void GetRawMouseDeltas(LONG& dX, LONG& dY);
	static void ClearMouseDeltas();

	static bool IsKeyDown(int keyCode);

private:
	static bool keyStates_[256]; // Track key states for all virtual key codes
	
	static LONG mouseDeltaX_;
	static LONG mouseDeltaY_;
};


