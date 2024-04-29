#pragma comment(lib, "kernel32.lib")
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
#include <shlguid.h>
#include <propkey.h>
#include <propvarutil.h>
#include <memory>
#include "debugutil.h"

void PrintUsage()
{
    wprintf(L"\n");
    wprintf(L"Usage: mklnkwaumid <ShortcutFilePath> <AppUserModelID> <Target> [<ParametersForTarget>]\n");
    wprintf(L"\n");
}

void ShowErrorMessage(LPCWSTR format, ...)
{
    va_list argList;
    va_start(argList, format);

    WCHAR messageBuffer[256] = { 0 };
    SecureZeroMemory(messageBuffer, sizeof(messageBuffer));
    HRESULT hr = StringCbVPrintf(messageBuffer, sizeof(messageBuffer), format, argList);
    if (SUCCEEDED(hr))
    {
        fwprintf(stderr, messageBuffer);
    }

    va_end(argList);
}

HRESULT ValidateArgStringLength(LPCWSTR argString, const size_t maxLength, LPCWSTR argName)
{
    HRESULT hr = StringCchLength(argString, maxLength, NULL);
    if (FAILED(hr))
    {
        constexpr LPCWSTR ERROR_MESSAGE_FORMAT = L"ERROR: Too long the %s parameter. The maximum length is %d characters. (HRESULT: 0x%08X)\n";
        ShowErrorMessage(ERROR_MESSAGE_FORMAT, argName, maxLength - 1, hr);
    }
    return hr;
}

HRESULT CopyStringToBuffer(LPWSTR buffer, const size_t bufferLength, LPCWSTR source, const unsigned long callerLineNumber)
{
    HRESULT hr = StringCchCopy(buffer, bufferLength, source);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: StringCchCopy() failed with 0x%08X as line %u", hr, callerLineNumber);
    }
    return hr;
}

constexpr int PATH_BUFFER_LENGTH = 4096;
constexpr int APP_USER_MODEL_ID_BUFFER_LENGTH = 128 + 1;

struct ExecutionContext
{
    WCHAR ShortcutFilePath[PATH_BUFFER_LENGTH];
    WCHAR AppUserModelID[APP_USER_MODEL_ID_BUFFER_LENGTH];
    WCHAR Target[PATH_BUFFER_LENGTH];
    WCHAR Arguments[PATH_BUFFER_LENGTH];
};

BOOL GetExecutionContext(ExecutionContext& context)
{
    BOOL returnValue = FALSE;
    BOOL hasParameterArg = FALSE;
    BOOL isArgsValid = TRUE;

    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLine(), &argc);
    if (argc < 4) goto EarlyReturn;

    hasParameterArg = argc >= 5;

    // Check the length of the command line arguments and print message for error understandability.
    isArgsValid = SUCCEEDED(ValidateArgStringLength(argv[1], PATH_BUFFER_LENGTH, L"ShortcutFilePath")) && isArgsValid;
    isArgsValid = SUCCEEDED(ValidateArgStringLength(argv[2], APP_USER_MODEL_ID_BUFFER_LENGTH, L"AppUserModelID")) && isArgsValid;
    isArgsValid = SUCCEEDED(ValidateArgStringLength(argv[3], PATH_BUFFER_LENGTH, L"Target")) && isArgsValid;
    if (hasParameterArg)
    {
        isArgsValid = SUCCEEDED(ValidateArgStringLength(argv[4], PATH_BUFFER_LENGTH, L"ParametersForTarget")) && isArgsValid;
    }
    if (!isArgsValid) goto EarlyReturn;  // There are invalid command line arguments.

    // Store the command line arguments to the app execution context.
    if (FAILED(CopyStringToBuffer(context.ShortcutFilePath, sizeof(context.ShortcutFilePath), argv[1], __LINE__))) goto EarlyReturn;
    if (FAILED(CopyStringToBuffer(context.AppUserModelID, sizeof(context.AppUserModelID), argv[2], __LINE__))) goto EarlyReturn;
    if (FAILED(CopyStringToBuffer(context.Target, sizeof(context.Target), argv[3], __LINE__))) goto EarlyReturn;
    if (hasParameterArg)
    {
        if (FAILED(CopyStringToBuffer(context.Arguments, sizeof(context.Arguments), argv[4], __LINE__))) goto EarlyReturn;
    }
    else
    {
        if (FAILED(CopyStringToBuffer(context.Arguments, sizeof(context.Arguments), L"", __LINE__))) goto EarlyReturn;
    }

    returnValue = TRUE;

    DEBUG_PRINT(L"ShortcutFilePath: %s\n", context.ShortcutFilePath);
    DEBUG_PRINT(L"AppUserModelID: %s\n", context.AppUserModelID);
    DEBUG_PRINT(L"Target: %s\n", context.Target);
    DEBUG_PRINT(L"Arguments: %s\n", context.Arguments);

EarlyReturn:

    LocalFree(argv);
    return returnValue;
}

HRESULT SetAppUserModelIDToShellLink(IShellLink* shellLink, LPCWSTR appUserModelId)
{
    IPropertyStore* propertyStore = nullptr;
    HRESULT hr = shellLink->QueryInterface(IID_IPropertyStore, (LPVOID*)&propertyStore);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: IShellLink::QueryInterface(IID_IPersistFile) failed with 0x%08X.\n", hr);
        return hr;
    }

    HRESULT lastHResult = S_OK;

    PROPVARIANT propVariant;
    hr = InitPropVariantFromString(appUserModelId, &propVariant);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: InitPropVariantFromString() failed with 0x%08X.\n", hr);
        lastHResult = hr;
        goto EarlyReturn;
    }

    hr = propertyStore->SetValue(PKEY_AppUserModel_ID, propVariant);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: IPropertyStore::SetValue() failed with 0x%08X.\n", hr);
        lastHResult = hr;
    }
    PropVariantClear(&propVariant);  // Release the memory of the propVariant.

EarlyReturn:

    propertyStore->Release();
    return lastHResult;
}

HRESULT SaveSellLinkToFile(IShellLink* shellLink, LPCWSTR shortcutFilePath)
{
    IPersistFile* persistFile = nullptr;
    HRESULT hr = shellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&persistFile);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: IShellLink::QueryInterface(IID_IPersistFile) failed with 0x%08X.\n", hr);
        return hr;
    }

    hr = persistFile->Save(shortcutFilePath, TRUE);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: IPersistFile::Save() failed with 0x%08X.\n", hr);
    }

    persistFile->Release();
    return hr;
}

HRESULT CreateShortcutFile(LPCWSTR shortcutFilePath, LPCWSTR target, LPCWSTR arguments, LPCWSTR appUserModelId)
{
    IShellLink* shellLink = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&shellLink);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: CoCreateInstance() failed with 0x%08X.\n", hr);
        return hr;
    }

    HRESULT lastHResult = S_OK;

    // Set the shotcut file's properties.
    hr = shellLink->SetPath(target);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: IShellLink::SetPath() failed with 0x%08X.\n", hr);
        lastHResult = hr;
        goto EarlyReturn;
    }

    hr = shellLink->SetArguments(arguments);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: IShellLink::SetArguments() failed with 0x%08X.\n", hr);
        lastHResult = hr;
        goto EarlyReturn;
    }

    // Set the AppUserModelID to the property store of the link.
    hr = SetAppUserModelIDToShellLink(shellLink, appUserModelId);
    if (FAILED(hr))
    {
        lastHResult = hr;
        goto EarlyReturn;
    }

    // Save the link to disk as a .lnk file.
	hr = SaveSellLinkToFile(shellLink, shortcutFilePath);
    if (FAILED(hr))
    {
        lastHResult = hr;
    }

EarlyReturn:

    shellLink->Release();
    return lastHResult;
}

int wmain(void)
{
    // Get execution context from the command line parameters.
    auto executionContext = std::make_unique<ExecutionContext>();
    if (!GetExecutionContext(*executionContext))
    {
        PrintUsage();
        return -1;
    }

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: CoInitializeEx() failed with 0x%08X.\n", hr);
        return -1;
    }

    // Crate a shortcut file.
    int exitCode = 0;
    hr = CreateShortcutFile(executionContext->ShortcutFilePath, executionContext->Target, executionContext->Arguments, executionContext->AppUserModelID);
    if (FAILED(hr))
    {
        ShowErrorMessage(L"ERROR: CreateShortcutFile() failed with 0x%08X.\n", hr);
        exitCode = -1;
    }

    CoUninitialize();
    return exitCode;
}
