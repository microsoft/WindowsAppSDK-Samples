// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;

// Assumes the installer exists in the following location.
// It is recommended to use a harmless "Hello, World!" type program, or a test installer built from
// the WindowsAppSDK Installer project. A released WindowsAppRuntimeInstall.exe will also work.
#define INSTALLER_PATH L"%USERPROFILE%\\Downloads\\WindowsAppRuntimeInstall.exe"

// Helper for expanding environment variables for installer path flexibility.
std::filesystem::path ExpandedPath(_In_ PCWSTR unexpandedPath)
{
    auto length = ::ExpandEnvironmentStrings(unexpandedPath, nullptr, 0);
    THROW_LAST_ERROR_IF(length == 0);
    std::wstring expandedPath;
    expandedPath.resize(length - 1);
    auto expandedLength = ::ExpandEnvironmentStrings(unexpandedPath, expandedPath.data(), length);
    THROW_LAST_ERROR_IF(expandedLength == 0 || expandedLength > length);
    return std::filesystem::path(expandedPath);
}

int main()
{
    init_apartment();

    auto installerPath = ExpandedPath(INSTALLER_PATH);
    printf("Running installer at %ws\n", installerPath.wstring().c_str());

    STARTUPINFO startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    PROCESS_INFORMATION processInformation{};

    // Create the process using CREATE_NO_WINDOW creation flag.
    // This suppresses the console window from being displayed.
    // For flag details, see https://docs.microsoft.com/windows/win32/procthread/process-creation-flags
    if (!CreateProcessW(
        installerPath.wstring().c_str(),    // Module name
        NULL,                               // Command line
        NULL,                               // Process handle not inheritable
        NULL,                               // Thread handle not inheritable
        FALSE,                              // Set handle inheritance to FALSE
        CREATE_NO_WINDOW,                   // Creation flags
        NULL,                               // Use parent's environment block
        NULL,                               // Use parent's starting directory 
        &startupInfo,                       // Pointer to STARTUPINFO structure
        &processInformation                 // Pointer to PROCESS_INFORMATION structure
    ))
    {
        printf("CreateProcess failed: %d\n", GetLastError());
        THROW_HR(HRESULT_FROM_WIN32(GetLastError()));
    }

    wil::unique_handle process{ processInformation.hProcess };
    wil::unique_handle thread{ processInformation.hThread };
    WaitForSingleObject(process.get(), (30 * 1000) /* 30 second timeout */ );

    DWORD exitCode{};
    THROW_IF_WIN32_BOOL_FALSE(GetExitCodeProcess(process.get(), &exitCode));
    printf("Installer exit code: 0x%0X", HRESULT_FROM_WIN32(exitCode));
    return HRESULT_FROM_WIN32(exitCode);
}
