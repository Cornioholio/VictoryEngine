#pragma once

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
	VICTORY_API bool VictoryEngine_Test();

	VICTORY_API bool VictoryEngine_Initialize(HWND hwnd, int width, int height);

	VICTORY_API void VictoryEngine_Resize(int width, int height);

	VICTORY_API void VictoryEngine_RenderClearFrame(float r, float g, float b, float a);

	VICTORY_API void VictoryEngine_Shutdown();
}

