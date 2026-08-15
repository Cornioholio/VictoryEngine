#pragma once
#include "Core/Tools/Logger.h"
#include <Windows.h> 

// Basically means we make this function available to other programs that load this dll.
#ifdef ENGINE_BUILD_DLL
#define VICTORY_API __declspec(dllexport)
#else
#define VICTORY_API __declspec(dllimport)
#endif

// Makes the functions we put here easier for .net to locate, because c++ wacky asf
extern "C" 
{
	VICTORY_API bool VictoryEngine_Initialize(HWND hwnd, int width, int height);

	VICTORY_API void VictoryEngine_RequestViewportResize(int width, int height);

	VICTORY_API int VictoryEngine_GetLogCount();
	VICTORY_API const char* VictoryEngine_GetLogMessage(int index);
	VICTORY_API int VictoryEngine_GetLogLevel(int index);

	VICTORY_API void VictoryEngine_Shutdown();
}

