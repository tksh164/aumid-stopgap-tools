#pragma once
#include <windows.h>
#include <wchar.h>
#include <strsafe.h>

#ifdef _DEBUG
#define DEBUG_PRINT(...) { WCHAR buffer[1024] = { 0 }; if (SUCCEEDED(StringCbPrintf(buffer, sizeof(buffer), __VA_ARGS__))) { OutputDebugString(buffer); } }
#else
#define DEBUG_PRINT(...)
#endif
