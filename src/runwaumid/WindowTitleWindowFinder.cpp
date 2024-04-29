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

HWND WindowTitleWindowFinder::FindWindow(LPCWSTR windowTitleToFind, const BOOL useExactMatch, LPCWSTR skipAppUserModelId)
{
    return FindWindowWithRetry(windowTitleToFind, useExactMatch, skipAppUserModelId);
}

HWND WindowTitleWindowFinder::FindWindowWithRetry(LPCWSTR windowTitleToFind, const BOOL useExactMatch, LPCWSTR skipAppUserModelId)
{
    HWND foundWindowHandle = NULL;
    for (DWORD retryCount = 0; retryCount < _maxRetryCount; retryCount++)
    {
        HWND windowHandle = FindWindowByTitle(windowTitleToFind, useExactMatch, skipAppUserModelId);
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

HWND WindowTitleWindowFinder::FindWindowByTitle(LPCWSTR windowTitleToFind, const BOOL useExactMatch, LPCWSTR skipAppUserModelId)
{
    FindWindowData data = { 0 };
    SecureZeroMemory(&data, sizeof(data));
    data.WindowTitleToFind = windowTitleToFind;
    data.UseExactMatch = useExactMatch;
	data.SkipAppUserModelId = skipAppUserModelId;
    data.FoundWindowHandle = NULL;
    EnumWindows(EnumWindowsProc, (LPARAM)&data);
    return data.FoundWindowHandle;
}

BOOL CALLBACK WindowTitleWindowFinder::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
    FindWindowData& data = *(FindWindowData*)lParam;
    if (!IsTargetWindow(hwnd, data.WindowTitleToFind, data.UseExactMatch, data.SkipAppUserModelId))
    {
        return TRUE;  // Continue enumeration.
    }
    data.FoundWindowHandle = hwnd;
    return FALSE;  // Found.
}

BOOL WindowTitleWindowFinder::IsTargetWindow(const HWND windowHandle, LPCWSTR windowTitleToFind, const BOOL useExactMatch, LPCWSTR skipAppUserModelId)
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

    constexpr int WINDOW_TITLE_BUFFER_LENGTH = 256;
    WCHAR windowTitle[WINDOW_TITLE_BUFFER_LENGTH] = { 0 };
    SecureZeroMemory(windowTitle, sizeof(windowTitle));
    if (GetWindowText(windowHandle, windowTitle, WINDOW_TITLE_BUFFER_LENGTH) == 0)
    {
        // The window has no title text, or has empty title text.
        return FALSE;
    }

    if (useExactMatch)
    {
        const BOOL isMatchedExactly = wcscmp(windowTitle, windowTitleToFind) == 0;
        if (!isMatchedExactly)
        {
            // The window's title does not match the window title to find.
            return FALSE;
        }
    }
    else
    {
        const BOOL hasPartialText = wcsstr(windowTitle, windowTitleToFind) != NULL;
        if (!hasPartialText)
        {
            // The window's title does not contain the partial window title text to find.
            return FALSE;
        }
    }

    const BOOL hasSameWindowAppUserModelId = HasWindowAppUserModelId(windowHandle, skipAppUserModelId);
    if (hasSameWindowAppUserModelId)
    {
        // The window already has the same AppUserModelID.
		return FALSE;
    }

    return TRUE;  // Found.
}

BOOL WindowTitleWindowFinder::HasWindowAppUserModelId(const HWND windowHandle, PCWSTR appUserModelId)
{
    IPropertyStore* propertyStore = nullptr;
    HRESULT hr = SHGetPropertyStoreForWindow(windowHandle, IID_PPV_ARGS(&propertyStore));
    if (FAILED(hr))
    {
        // Considered that the window does not have an AppUserModelID if failed to get the property store.
		return FALSE;
    }

    BOOL returnValue = FALSE;

    PROPVARIANT propVariant = { 0 };
	SecureZeroMemory(&propVariant, sizeof(propVariant));
    hr = propertyStore->GetValue(PKEY_AppUserModel_ID, &propVariant);
    if (FAILED(hr))
    {
        // Considered that the window does not have an AppUserModelID if failed to get the property value.
		goto EarlyReturn;
    }
    if (propVariant.vt == VT_LPWSTR && (wcscmp(propVariant.pwszVal, appUserModelId) == 0))
    {
		// The window has the specified AppUserModelID.
		returnValue = TRUE;
    }
    PropVariantClear(&propVariant);  // Release the memory of the propVariant.

EarlyReturn:

    propertyStore->Release();
    return returnValue;
}
