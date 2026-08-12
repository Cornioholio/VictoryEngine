#pragma once

#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
#endif
// ^ To streamline the inclusion of Windows.h

// Prevent windows from defining min/max macros, silly macros
#ifndef NOMINMAX
	#define NOMINMAX
#endif

#ifndef UNICODE
	#define UNICODE
#endif

#ifndef _UNICODE
	#define _UNICODE
#endif

#include <Windows.h>
#include <memory>
#include <iostream>

// If building the engine dll, export symbols. if compiling the client app, import symbols
#ifdef ENGINE_BUILD_DLL
	#define VICTORY_API __declspec(dllexport)
#else
	#define VICTORY_API __declspec(dllimport)
#endif