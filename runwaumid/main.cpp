#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "ole32.lib")

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
#include <propvarutil.h>
#include <memory>
#include "debugutil.h"
#include "WindowTitleWindowFinder.h"
#include "ProcessIdWindowFinder.h"

constexpr LPCWSTR APP_NAME = L"Run with AppUserModelID";

void PrintUsage()
{
    constexpr LPCWSTR usageText =
        L"Usage:\n"
        L"\n"
        L"Find the target window by the partial window title text:\n"
        L"\n"
        L"   runwaumid -tp <AppUserModelID> <WindowTitle> <TargetToOpen> [<ParametersForTargetToOpen>]\n"
        L"\n"
        L"Find the target window by the exact window title text:\n"
        L"\n"
        L"   runwaumid -te <AppUserModelID> <WindowTitle> <TargetToOpen> [<ParametersForTargetToOpen>]\n"
        L"\n"
        L"Find the target window by the process ID:\n"
        L"\n"
        L"   runwaumid -p <AppUserModelID> <TargetToOpen> [<ParametersForTargetToOpen>]";
    MessageBox(NULL, usageText, APP_NAME, MB_OK | MB_ICONINFORMATION);
}

void ShowErrorMessageBox(LPCWSTR title, const UINT type, LPCWSTR format, ...)
{
    va_list argList;
    va_start(argList, format);

    WCHAR messageBuffer[256] = { 0 };
    SecureZeroMemory(messageBuffer, sizeof(messageBuffer));
    HRESULT hr = StringCbVPrintf(messageBuffer, sizeof(messageBuffer), format, argList);
    if (SUCCEEDED(hr))
    {
        MessageBox(NULL, messageBuffer, title, type);
    }
    else
    {
        MessageBox(NULL, L"StringCbVPrintf() failed", APP_NAME, MB_OK | MB_ICONERROR);
    }

    va_end(argList);
}

BOOL ValidateArgStringLength(LPCWSTR argString, size_t maxLength, LPCWSTR argName)
{
    HRESULT hr = StringCchLength(argString, maxLength, NULL);
    if (FAILED(hr))
    {
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONWARNING, L"Too long the %s parameter. The maximum length is %d characters. (HRESULT: 0x%08X)", argName, maxLength - 1, hr);
    }
    return SUCCEEDED(hr);
}

BOOL CopyStringToBuffer(LPWSTR buffer, size_t bufferLength, LPCWSTR source, unsigned long callerLineNumber)
{
    HRESULT hr = StringCchCopy(buffer, bufferLength, source);
    if (FAILED(hr))
    {
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONERROR, L"StringCchCopy() failed with 0x%08X as line %u", hr, callerLineNumber);
    }
    return SUCCEEDED(hr);
}

enum WindowFindMode
{
    Undefined = 0,
    PartialTitleText,
    ExactTitleText,
    ProcessId
};

constexpr int APP_USER_MODEL_ID_BUFFER_LENGTH = 128 + 1;
constexpr int PATH_BUFFER_LENGTH = 4096;
constexpr int WINDOW_TITLE_TEXT_BUFFER_LENGTH = 1024;

struct ExecutionContext
{
    WindowFindMode WindowFindMode;
    WCHAR AppUserModelID[APP_USER_MODEL_ID_BUFFER_LENGTH];
    WCHAR TargetToOpen[PATH_BUFFER_LENGTH];
    WCHAR ParametersForTargetToOpen[PATH_BUFFER_LENGTH];
    WCHAR WindowTitleText[WINDOW_TITLE_TEXT_BUFFER_LENGTH];
};

BOOL GetExecutionContext(ExecutionContext& context)
{
    BOOL returnValue = FALSE;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (argc < 2) goto EarlyReturn;  // Too less command line arguments.

    // Ensure the Window find mode.
    if (wcscmp(L"-tp", argv[1]) == 0)
    {
        context.WindowFindMode = WindowFindMode::PartialTitleText;
    }
    else if (wcscmp(L"-te", argv[1]) == 0)
    {
        context.WindowFindMode = WindowFindMode::ExactTitleText;
    }
    else if (wcscmp(L"-p", argv[1]) == 0)
    {
        context.WindowFindMode = WindowFindMode::ProcessId;
    }
    else
    {
        goto EarlyReturn;
    }

    // Window title text mode.
    if (context.WindowFindMode == WindowFindMode::PartialTitleText || context.WindowFindMode == WindowFindMode::ExactTitleText)
    {
        if (!(argc == 5 || argc == 6)) goto EarlyReturn;  // Too less or too many command line arguments.
        BOOL hasParameterArg = argc >= 6;
        BOOL isCmdlineArgsValid = TRUE;

        // Check the length of the command line arguments and show message for error understandability.
        isCmdlineArgsValid = ValidateArgStringLength(argv[2], APP_USER_MODEL_ID_BUFFER_LENGTH, L"AppUserModelID") && isCmdlineArgsValid;
        isCmdlineArgsValid = ValidateArgStringLength(argv[3], WINDOW_TITLE_TEXT_BUFFER_LENGTH, L"WindowTitle") && isCmdlineArgsValid;
        isCmdlineArgsValid = ValidateArgStringLength(argv[4], PATH_BUFFER_LENGTH, L"TargetToOpen") && isCmdlineArgsValid;
        if (hasParameterArg)
        {
            isCmdlineArgsValid = ValidateArgStringLength(argv[5], PATH_BUFFER_LENGTH, L"ParametersForTargetToOpen") && isCmdlineArgsValid;
        }
        if (!isCmdlineArgsValid) goto EarlyReturn;  // There are invalid command line arguments.

        // Store the command line arguments to the app execution context.
        if (!CopyStringToBuffer(context.AppUserModelID, sizeof(context.AppUserModelID), argv[2], __LINE__)) goto EarlyReturn;
        if (!CopyStringToBuffer(context.WindowTitleText, sizeof(context.WindowTitleText), argv[3], __LINE__)) goto EarlyReturn;
        if (!CopyStringToBuffer(context.TargetToOpen, sizeof(context.TargetToOpen), argv[4], __LINE__)) goto EarlyReturn;
        if (hasParameterArg)
        {
            if (!CopyStringToBuffer(context.ParametersForTargetToOpen, sizeof(context.ParametersForTargetToOpen), argv[5], __LINE__)) goto EarlyReturn;
        }
        else
        {
            if (!CopyStringToBuffer(context.ParametersForTargetToOpen, sizeof(context.ParametersForTargetToOpen), L"", __LINE__)) goto EarlyReturn;
        }
        returnValue = TRUE;
    }

    // Process ID mode.
    else if (context.WindowFindMode == WindowFindMode::ProcessId)
    {
        if (!(argc == 4 || argc == 5)) goto EarlyReturn;  // Too less or too many command line arguments.
        BOOL hasParameterArg = argc >= 5;
        BOOL isCmdlineArgsValid = TRUE;

        // Check the length of the command line arguments and show message for error understandability.
        isCmdlineArgsValid = ValidateArgStringLength(argv[2], APP_USER_MODEL_ID_BUFFER_LENGTH, L"AppUserModelID") && isCmdlineArgsValid;
        isCmdlineArgsValid = ValidateArgStringLength(argv[3], PATH_BUFFER_LENGTH, L"TargetToOpen") && isCmdlineArgsValid;
        if (hasParameterArg)
        {
            isCmdlineArgsValid = ValidateArgStringLength(argv[4], PATH_BUFFER_LENGTH, L"ParametersForTargetToOpen") && isCmdlineArgsValid;
        }
        if (!isCmdlineArgsValid) goto EarlyReturn;  // There are invalid command line arguments.

        // Store the command line arguments to the app execution context.
        context.WindowFindMode = WindowFindMode::ProcessId;
        if (!CopyStringToBuffer(context.AppUserModelID, sizeof(context.AppUserModelID), argv[2], __LINE__)) goto EarlyReturn;
        if (!CopyStringToBuffer(context.TargetToOpen, sizeof(context.TargetToOpen), argv[3], __LINE__)) goto EarlyReturn;
        if (hasParameterArg)
        {
            if (!CopyStringToBuffer(context.ParametersForTargetToOpen, sizeof(context.ParametersForTargetToOpen), argv[4], __LINE__)) goto EarlyReturn;
        }
        else
        {
            if (!CopyStringToBuffer(context.ParametersForTargetToOpen, sizeof(context.ParametersForTargetToOpen), L"", __LINE__)) goto EarlyReturn;
        }
        returnValue = TRUE;
    }

    // Unexpected mode.
    else
    {
        goto EarlyReturn;
    }

    DEBUG_PRINT(L"WindowFindMode: %d\n", (int)context.WindowFindMode);
    DEBUG_PRINT(L"AppUserModelID: %s\n", context.AppUserModelID);
    DEBUG_PRINT(L"TargetToOpen: %s\n", context.TargetToOpen);
    DEBUG_PRINT(L"ParametersForTargetToOpen: %s\n", context.ParametersForTargetToOpen);
    DEBUG_PRINT(L"WindowTitleText: %s\n", context.WindowTitleText);

EarlyReturn:

    LocalFree(argv);
    return returnValue;
}

BOOL OpenTargetUsingShell(LPCWSTR target, LPCWSTR parameters, HANDLE* processHandle)
{
    SHELLEXECUTEINFO shellExecuteInfo = { 0 };
    SecureZeroMemory(&shellExecuteInfo, sizeof(shellExecuteInfo));
    shellExecuteInfo.cbSize = sizeof(shellExecuteInfo);
    shellExecuteInfo.fMask = SEE_MASK_DEFAULT | SEE_MASK_NOCLOSEPROCESS;
    shellExecuteInfo.lpVerb = L"open";
    shellExecuteInfo.lpFile = target;
    shellExecuteInfo.lpParameters = parameters;
    shellExecuteInfo.nShow = SW_SHOWNORMAL;
    BOOL result = ShellExecuteEx(&shellExecuteInfo);
    if (result)
    {
        *processHandle = shellExecuteInfo.hProcess;
    }
    else
    {
        *processHandle = INVALID_HANDLE_VALUE;
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONERROR, L"ShellExecuteEx() failed with 0x%08X", GetLastError());
    }
    return result;
}

BOOL SetAppUserModelIDToWindow(const HWND windowHandle, PCWSTR appUserModelId)
{
    BOOL returnValue = TRUE;

    //
    // NOTE:
    // Obtain the window's property store to set an explicit Application User Model ID.
    // A window's properties must be removed before the window is closed. If this is not done,
    // the resources used by those properties are not returned to the system.
    // However, this program does not provide the chance to remove the property of Application
    // User Model ID because this program set it to the not own window.
    //
    IPropertyStore* propertyStore = nullptr;
    HRESULT hr = SHGetPropertyStoreForWindow(windowHandle, IID_PPV_ARGS(&propertyStore));
    if (FAILED(hr))
    {
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONERROR, L"SHGetPropertyStoreForWindow() failed with 0x%08X", hr);
        returnValue = FALSE;
        goto EarlyReturn;
    }

    PROPVARIANT propVariant;
    hr = InitPropVariantFromString(appUserModelId, &propVariant);
    if (FAILED(hr))
    {
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONERROR, L"InitPropVariantFromString() failed with 0x%08X", hr);
        returnValue = FALSE;
        goto EarlyReturn;
    }

    hr = propertyStore->SetValue(PKEY_AppUserModel_ID, propVariant);
    if (FAILED(hr))
    {
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONERROR, L"IPropertyStore::SetValue() failed with 0x%08X.", hr);
        returnValue = FALSE;
    }

    PropVariantClear(&propVariant);  // Release the memory of the propVariant.

EarlyReturn:

    if (propertyStore)
    {
        propertyStore->Release();
    }
    return returnValue;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
    // Get execution context from the command line parameters.
    auto executionContext = std::make_unique<ExecutionContext>();
    if (!GetExecutionContext(*executionContext))
    {
        PrintUsage();
        return -1;
    }

    // Open the file using the shell.
    HANDLE processHandle = INVALID_HANDLE_VALUE;
    if (!OpenTargetUsingShell(executionContext->TargetToOpen, executionContext->ParametersForTargetToOpen, &processHandle))
    {
        return -1;
    }

    // Find the window to set the AppUserModelID.
    HWND windowHandle = NULL;
    DWORD processIdToFind = 0;
    if (executionContext->WindowFindMode == WindowFindMode::PartialTitleText)
    {
        auto windowFinder = std::make_unique<WindowTitleWindowFinder>();
        windowHandle = windowFinder->FindWindow(executionContext->WindowTitleText, FALSE);
    }
    else if (executionContext->WindowFindMode == WindowFindMode::ExactTitleText)
    {
        auto windowFinder = std::make_unique<WindowTitleWindowFinder>();
        windowHandle = windowFinder->FindWindow(executionContext->WindowTitleText, TRUE);
    }
    else if (executionContext->WindowFindMode == WindowFindMode::ProcessId)
    {
        processIdToFind = GetProcessId(processHandle);
        auto windowFinder = std::make_unique<ProcessIdWindowFinder>();
        windowHandle = windowFinder->FindWindow(processIdToFind);
    }
    else
    {
        ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONERROR, L"Unexpected window find mode.");
        return -1;
    }

    if (windowHandle != NULL)
    {
        // Set the AppUserModelID to the window.
        if (!SetAppUserModelIDToWindow(windowHandle, executionContext->AppUserModelID))
        {
            return -1;
        }
    }
    else
    {
        if (executionContext->WindowFindMode == WindowFindMode::PartialTitleText)
        {
            constexpr LPCWSTR messageText =
                L"Couldn't find the target window that contained \"%s\".\n"
                L"\n"
                L"%s %s\n";
            ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONWARNING, messageText, executionContext->WindowTitleText, executionContext->TargetToOpen, executionContext->ParametersForTargetToOpen);
        }
        else if (executionContext->WindowFindMode == WindowFindMode::ExactTitleText)
        {
            constexpr LPCWSTR messageText =
                L"Couldn't find the target window that has \"%s\".\n"
                L"\n"
                L"%s %s\n";
            ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONWARNING, messageText, executionContext->WindowTitleText, executionContext->TargetToOpen, executionContext->ParametersForTargetToOpen);
        }
        else if (executionContext->WindowFindMode == WindowFindMode::ProcessId)
        {
            constexpr LPCWSTR messageText =
                L"Couldn't find the process that process ID is %d.\n"
                L"\n"
                L"%s %s";
            ShowErrorMessageBox(APP_NAME, MB_OK | MB_ICONWARNING, messageText, processIdToFind, executionContext->TargetToOpen, executionContext->ParametersForTargetToOpen);
        }
    }

    return 0;
}
