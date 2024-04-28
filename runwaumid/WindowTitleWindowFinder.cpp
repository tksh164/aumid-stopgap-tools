#pragma once
#include "WindowTitleWindowFinder.h"

WindowTitleWindowFinder::WindowTitleWindowFinder()
{
    _maxRetryCount = 10;
    _retryIntervalMilliseconds = 500;
}

WindowTitleWindowFinder::~WindowTitleWindowFinder()
{
}

HWND WindowTitleWindowFinder::FindWindow(LPCWSTR windowTitleToFind, const BOOL useExactMatch)
{
    return FindWindowWithRetry(windowTitleToFind, useExactMatch);
}

HWND WindowTitleWindowFinder::FindWindowWithRetry(LPCWSTR windowTitleToFind, const BOOL useExactMatch)
{
    HWND foundWindowHandle = NULL;
    for (DWORD retryCount = 0; retryCount < _maxRetryCount; retryCount++)
    {
        HWND windowHandle = FindWindowByTitle(windowTitleToFind, useExactMatch);
        if (windowHandle != NULL)
        {
            foundWindowHandle = windowHandle;
            break;
        }
        DEBUG_PRINT(L"Couldn't find the target window.\n");
        Sleep(_retryIntervalMilliseconds);
    }
    return foundWindowHandle;
}

HWND WindowTitleWindowFinder::FindWindowByTitle(LPCWSTR windowTitleToFind, const BOOL useExactMatch)
{
    FindWindowData data = { 0 };
    SecureZeroMemory(&data, sizeof(data));
    data.WindowTitleToFind = windowTitleToFind;
    data.UseExactMatch = useExactMatch;
    data.FoundWindowHandle = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    return data.FoundWindowHandle;
}

BOOL CALLBACK WindowTitleWindowFinder::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    FindWindowData& data = *(FindWindowData*)lParam;
    if (!IsTargetWindow(hwnd, data.WindowTitleToFind, data.UseExactMatch))
    {
        return TRUE;  // Continue enumeration.
    }
    data.FoundWindowHandle = hwnd;
    return FALSE;  // Found.
}

BOOL WindowTitleWindowFinder::IsTargetWindow(const HWND windowHandle, LPCWSTR windowTitleToFind, const BOOL useExactMatch)
{
    const BOOL isOwnedWindow = GetWindow(windowHandle, GW_OWNER) != NULL;
    if (isOwnedWindow)
    {
        return FALSE;  // The window is owned another window.
    }

    const BOOL isWindowVisible = IsWindowVisible(windowHandle);
    if (!isWindowVisible)
    {
        return FALSE;  // The window is not visible.
    }

    constexpr int WINDOW_TITLE_BUFFER_LENGTH = 256;
    WCHAR windowTitle[WINDOW_TITLE_BUFFER_LENGTH] = { 0 };
    SecureZeroMemory(windowTitle, sizeof(windowTitle));
    if (GetWindowText(windowHandle, windowTitle, WINDOW_TITLE_BUFFER_LENGTH) == 0)
    {
        return FALSE;  // The window has no title text, or has empty title text.
    }

    if (useExactMatch)
    {
        const BOOL isMatchedExactly = wcscmp(windowTitle, windowTitleToFind) == 0;
        if (!isMatchedExactly)
        {
            return FALSE;  // The window's title does not match the window title to find.
        }
    }
    else
    {
        const BOOL hasPartialText = wcsstr(windowTitle, windowTitleToFind) != NULL;
        if (!hasPartialText)
        {
            return FALSE;  // The window's title does not contain the partial window title text to find.
        }
    }

    return TRUE;  // Found.
}
