#pragma once

// Basically means we make this function available to other programs that load this dll.
#ifdef VICTORYENGINE_EXPORTS
#define VICTORY_API __declspec(dllexport)
#else
#define VICTORY_API __declspec(dllimport)
#endif

// Makes the functions we put here easier for .net to locate, because c++ wacky asf
extern "C" 
{
	VICTORY_API bool VictoryEngine_Test();
}

