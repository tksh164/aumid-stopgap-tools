#pragma once
#include "ProcessIdWindowFinder.h"

ProcessIdWindowFinder::ProcessIdWindowFinder()
{
    _maxRetryCount = 10;
    _retryIntervalMilliseconds = 500;
}

ProcessIdWindowFinder::~ProcessIdWindowFinder()
{
}

HWND ProcessIdWindowFinder::FindWindow(const DWORD processIdToFind)
{
    return FindWindowWithRetry(processIdToFind);
}

HWND ProcessIdWindowFinder::FindWindowWithRetry(const DWORD processIdToFind)
{
    HWND foundWindowHandle = NULL;
    for (DWORD retryCount = 0; retryCount < _maxRetryCount; retryCount++)
    {
        HWND windowHandle = FindWindowByProcessId(processIdToFind);
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

HWND ProcessIdWindowFinder::FindWindowByProcessId(const DWORD processIdToFind)
{
    FindWindowData data = { 0 };
    SecureZeroMemory(&data, sizeof(data));
    data.ProcessIdToFind = processIdToFind;
    data.FoundWindowHandle = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    return data.FoundWindowHandle;
}

BOOL CALLBACK ProcessIdWindowFinder::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    FindWindowData& data = *(FindWindowData*)lParam;
    if (!IsTargetWindow(hwnd, data.ProcessIdToFind))
    {
        return TRUE;  // Continue enumeration.
    }
    data.FoundWindowHandle = hwnd;
    return FALSE;  // Found.
}

BOOL ProcessIdWindowFinder::IsTargetWindow(const HWND windowHandle, const DWORD processIdToFind)
{
    const BOOL isOwnedWindow = GetWindow(windowHandle, GW_OWNER) != NULL;
    if (isOwnedWindow)
    {
        // The window is owned another window.
        return FALSE;
    }

    const BOOL isWindowVisible = IsWindowVisible(windowHandle);
    if (!isWindowVisible)
    {
        // The window is not visible.
        return FALSE;
    }

    DWORD processId = 0;
    if (GetWindowThreadProcessId(windowHandle, &processId) == 0)
    {
        // The window handle is invalid.
        return FALSE;
    }
    DEBUG_PRINT(L"Process ID: %d\n", processId);

    const BOOL isProcessIdMatched = processIdToFind == processId;
    if (!isProcessIdMatched)
    {
        // The process ID did not match.
        return FALSE;
    }

    return TRUE;  // Found.
}
