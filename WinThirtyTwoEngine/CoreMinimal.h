#pragma once
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// ^ To streamline the inclusion of Windows.h
#include <Windows.h>

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

#include <memory>
#include <iostream>