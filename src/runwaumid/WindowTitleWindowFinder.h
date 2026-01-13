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
#include <shobjidl.h>
#include <propkey.h>
#include "debugutil.h"

class WindowTitleWindowFinder
{
public:
    WindowTitleWindowFinder();
    ~WindowTitleWindowFinder();

    HWND FindWindow(LPCWSTR windowTitleToFind, BOOL useExactMatch, LPCWSTR skipAppUserModelId, const DWORD timeoutMilliseconds);

private:
    static constexpr DWORD RETRY_INTERVAL_MILLISECONDS = 300;

    struct FindWindowData
    {
        LPCWSTR WindowTitleToFind;
        BOOL UseExactMatch;
        LPCWSTR SkipAppUserModelId;
        HWND FoundWindowHandle;
    };

    HWND FindWindowWithRetry(LPCWSTR windowTitleToFind, BOOL useExactMatch, LPCWSTR skipAppUserModelId, const DWORD timeoutMilliseconds);
    HWND FindWindowByTitle(LPCWSTR windowTitleToFind, BOOL useExactMatch, LPCWSTR skipAppUserModelId);
    static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
    static BOOL IsTargetWindow(const HWND windowHandle, LPCWSTR windowTitleToFind, BOOL useExactMatch, LPCWSTR skipAppUserModelId);
    static BOOL HasWindowAppUserModelId(const HWND windowHandle, PCWSTR appUserModelId);
};
