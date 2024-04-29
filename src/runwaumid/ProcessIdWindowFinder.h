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

class ProcessIdWindowFinder
{
public:
    ProcessIdWindowFinder();
    ~ProcessIdWindowFinder();

    HWND FindWindow(const DWORD processIdToFindFor);

private:
    DWORD _maxRetryCount;
    DWORD _retryIntervalMilliseconds;

    struct FindWindowData
    {
        DWORD ProcessIdToFind;
        HWND FoundWindowHandle;
    };

    HWND FindWindowWithRetry(const DWORD processIdToFindFor);
    HWND FindWindowByProcessId(const DWORD processIdToFindFor);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    static BOOL IsTargetWindow(const HWND windowHandle, const DWORD processIdToFindFor);
};
