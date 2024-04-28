#pragma once

#ifndef _UNICODE
#define _UNICODE
#endif
#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <wchar.h>
#include <strsafe.h>
#include "debugutil.h"

class WindowTitleWindowFinder
{
public:
    WindowTitleWindowFinder();
    ~WindowTitleWindowFinder();

    HWND FindWindow(LPCWSTR windowTitleToFind, BOOL useExactMatch);

private:
    DWORD _maxRetryCount;
    DWORD _retryIntervalMilliseconds;

    struct FindWindowData
    {
        LPCWSTR WindowTitleToFind;
        BOOL UseExactMatch;
        HWND FoundWindowHandle;
    };

    HWND FindWindowWithRetry(LPCWSTR windowTitleToFind, BOOL useExactMatch);
    HWND FindWindowByTitle(LPCWSTR windowTitleToFind, BOOL useExactMatch);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    static BOOL IsTargetWindow(const HWND windowHandle, LPCWSTR windowTitleToFind, BOOL useExactMatch);
};
