// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::Storage;

// Windows App SDK version.
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"preview1" };
const PACKAGE_VERSION minVersion{};

WCHAR szMessage[65536];
WCHAR szExePath[MAX_PATH];
WCHAR szExePathAndIconIndex[MAX_PATH + 8];
int activationCount = 1;
event_token activationToken;

// Helpers ////////////////////////////////////////////////////////////////////

void OutputMessage(const WCHAR* message)
{
    _putws(message);
}

void OutputFormattedMessage(const WCHAR* fmt, ...)
{
    WCHAR message[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(message, fmt, args);
    va_end(args);
    OutputMessage(message);
}

void WINAPIV OutputFormattedDebugString(const WCHAR* fmt, ...)
{
    WCHAR s[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(s, fmt, args);
    va_end(args);
    OutputDebugString(s);
}

void OutputErrorString(const WCHAR* message)
{
    WCHAR err[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
    OutputFormattedDebugString(L"%s: %s", message, err);
}

std::wstring GetErrorString(const WCHAR* message)
{
    int const arraysize = 4096;
    WCHAR szTmp[arraysize];
    size_t cbTmp = arraysize * sizeof(WCHAR);

    WCHAR err[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
    StringCbPrintf(szTmp, cbTmp, L"%s: %s", message, err);

    std::wstring outString(szTmp);
    return outString;
}

std::wstring GetCurrentDateTime()
{
    time_t now = time(0);
    struct tm tstruct;
    WCHAR buf[512];
    errno_t err;
    err = localtime_s(&tstruct, &now);
    wcsftime(buf, sizeof(buf)/2, L"%Y-%m-%d %X", &tstruct);
    return std::wstring(buf);
}

void Logger(std::wstring logMsg)
{
    std::wstring filePath = L"C:\\Temp\\ErrLog.txt";
    std::wstring now = GetCurrentDateTime();
    std::wofstream ofs(filePath.c_str(), std::ios_base::out | std::ios_base::app);
    ofs << '\n' << now << '\n' << logMsg.c_str() << '\n';
    ofs.close();
}

std::vector<std::wstring> split_strings(hstring argString)
{
    std::vector<std::wstring> argStrings;
    std::wistringstream iss(argString.c_str());
    for (std::wstring s; iss >> s; )
    {
        argStrings.push_back(s);
    }
    return argStrings;
}

///////////////////////////////////////////////////////////////////////////////

void GetActivationInfo()
{
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        auto launchArgs = args.Data().as<ILaunchActivatedEventArgs>();
        if (launchArgs != NULL)
        {
            auto argString = launchArgs.Arguments().c_str();
            std::vector<std::wstring> argStrings = split_strings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == ExtendedActivationKind::File)
    {
        auto fileArgs = args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs != NULL)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
}

void RegisterForFileActivation()
{
    // Register one or more supported image filetypes, 
    // an icon (specified by binary file path plus resource index),
    // a display name to use in Shell and Settings,
    // zero or more verbs for the File Explorer context menu,
    // and the path to the EXE to register for activation.
    hstring imageFileTypes[1] = { L".moo" };
    hstring verbs[2] = { L"view", L"edit" };
    ActivationRegistrationManager::RegisterForFileTypeActivation(
        imageFileTypes,
        szExePathAndIconIndex,
        L"Montoso File Types",
        verbs,
        szExePath
    );
    OutputMessage(L"Registered for file activation");
}

void UnregisterForFileActivation()
{
    // Unregister one or more registered filetypes.
    try
    {
        hstring imageFileTypes[1] = { L".moo" };
        ActivationRegistrationManager::UnregisterForFileTypeActivation(
            imageFileTypes,
            szExePath
        );
        OutputMessage(L"Unregistered for file activation");
    }
    catch (...)
    {
        OutputMessage(L"Error unregistering file types");
    }
}

IAsyncAction ProcessTheFile(const WCHAR* fileName)
{
    // Capture calling (UI) thread.
    winrt::apartment_context ui_thread;

    // Do work on a background thread, for long enough
    // that we can handle multiple later activations.
    co_await winrt::resume_background();
    for (int i = 0; i < 20; i++)
    {
        wprintf(L"processing %s (%d)...\n", fileName, i);
        Sleep(1000);
    }

    // Switch back to the UI thread.
    co_await ui_thread;
}

void ReportLaunchArgs(hstring callLocation, AppActivationArguments args)
{
    ILaunchActivatedEventArgs launchArgs = args.Data().as<ILaunchActivatedEventArgs>();
    if (launchArgs != NULL)
    {
        winrt::hstring argString = launchArgs.Arguments().c_str();
        std::vector<std::wstring> argStrings = split_strings(argString);
        OutputFormattedMessage(L"Launch activation: %s", callLocation.c_str());
        for (std::wstring s : argStrings)
        {
            OutputMessage(s.c_str());
        }
    }
}

void ReportFileArgs(hstring callLocation, AppActivationArguments args)
{
    IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
    if (fileArgs != NULL)
    {
        IStorageItem file = fileArgs.Files().GetAt(0);
        OutputFormattedMessage(
            L"File activation: %s for '%s'", callLocation.c_str(), file.Name().c_str());
    }
}

void OnActivated(const IInspectable&, const AppActivationArguments& args)
{
    int const arraysize = 4096;
    WCHAR szTmp[arraysize];
    size_t cbTmp = arraysize * sizeof(WCHAR);
    StringCbPrintf(szTmp, cbTmp, TEXT("OnActivated, count=%d"), activationCount++);
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::File)
    {
        ReportFileArgs(szTmp, args);
    }
}

///////////////////////////////////////////////////////////////////////////////


int main()
{
    init_apartment();

    // Initialize Windows App SDK for unpackaged apps.
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, 
            minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    // Get the current executable filesystem path, so we can
    // use it later in registering for activation kinds.
    GetModuleFileName(NULL, szExePath, MAX_PATH);
    _putws(szExePath);
    wcscpy_s(szExePathAndIconIndex, szExePath);
    wcscat_s(szExePathAndIconIndex, L",1");

    // Find out what kind of activation this is.
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        // This is a launch activation: here we'll register for file activation.
        ReportLaunchArgs(L"main", args);
        RegisterForFileActivation();
    }
    else if (kind == ExtendedActivationKind::File)
    {
        ReportFileArgs(L"main", args);

        try
        {
            // This is a file activation: here we'll get the file information,
            // and register the file name as our instance key.
            IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
            if (fileArgs != NULL)
            {
                IStorageItem file = fileArgs.Files().GetAt(0);
                AppInstance keyInstance = AppInstance::FindOrRegisterForKey(file.Name());
                wcscat_s(szMessage, L"\nRegistered key = ");
                wcscat_s(szMessage, keyInstance.Key().c_str());

                // If we successfully registered the file name, we must be the
                // only instance running that was activated for this file.
                if (keyInstance.IsCurrent())
                {
                    // Report successful file name key registration.
                    wcscat_s(szMessage, L"\nIsCurrent=true; registered this instance for ");
                    wcscat_s(szMessage, file.Name().c_str());
                    _putws(szMessage);

                    activationToken = keyInstance.Activated([&keyInstance](
                            const auto& sender, const AppActivationArguments& args)
                        { OnActivated(sender, args); }
                    );
                    _putws(L"Connected Activated event");

                    // Process the selected file. Note: we're in an MTA,
                    // so it's OK to wait on the async call here.
                    ProcessTheFile(file.Name().c_str()).get();
                }
                else
                {
                    // Pause long enough to show the Redirecting message.
                    _putws(L"\nRedirecting...\n");
                    Sleep(3000);
                    keyInstance.RedirectActivationToAsync(args).get();
                    return 1;
                }
            }
        }
        catch (...)
        {
            OutputErrorString(L"Error getting instance information");
        }
    }

    // Pause long enough to show the Closing message.
    _putws(L"\nClosing...\n");
    Sleep(1000);
    MddBootstrapShutdown();
}
