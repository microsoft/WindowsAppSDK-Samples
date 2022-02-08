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
PCWSTR versionTag{ L"" };
const PACKAGE_VERSION minVersion{};

WCHAR exePath[MAX_PATH];
WCHAR exePathAndIconIndex[MAX_PATH + 8];

std::vector<std::wstring> SplitStrings(hstring argString);
void RegisterForActivation();
void UnregisterForActivation();
void GetActivationInfo();
void LaunchSettingsPage();
void LaunchMainPage();

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

std::vector<std::wstring> SplitStrings(hstring argString)
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


// main ///////////////////////////////////////////////////////////////////////

int main()
{
    init_apartment();

    // Initialize Windows App SDK for unpackaged apps.
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, 
            minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    // Get the current executable filesystem path, so we can
    // use it later in registering for activation kinds.
    GetModuleFileName(NULL, exePath, MAX_PATH);
    wcscpy_s(exePathAndIconIndex, exePath);
    wcscat_s(exePathAndIconIndex, L",0");
    _putws(exePath);

    char charOption[2] = { 0 };
    int intOption = 0;
    do
    {
        _putws(L"\nMENU");
        _putws(L"1 - Get activation info");
        _putws(L"2 - Register for rich activation");
        _putws(L"3 - Unregister for rich activation");
        _putws(L"4 - Quit");
        wprintf(L"Select an option: ");

        scanf_s("%1s", charOption, (unsigned)_countof(charOption));
        intOption = atoi(charOption);
        switch (intOption)
        {
        case 1:
            GetActivationInfo();
            break;
        case 2:
            RegisterForActivation();
            break;
        case 3:
            UnregisterForActivation();
            break;
        case 4:
            break;
        default:
            printf("*** Error: %s is not a valid choice ***", charOption);
            break;
        }
    } while (intOption != 4);

    // Uninitialize Windows App SDK.
    MddBootstrapShutdown();
    return 0;
}

///////////////////////////////////////////////////////////////////////////////


// Register/Unregister for rich activation ////////////////////////////////////

void RegisterForActivation()
{
    // Register one or more supported image filetypes, 
    // an icon (specified by binary file path plus resource index),
    // a display name to use in Shell and Settings,
    // zero or more verbs for the File Explorer context menu,
    // and the path to the EXE to register for activation.
    hstring myFileTypes[3] = { L".foo", L".foo2", L".foo3" };
    hstring verbs[2] = { L"view", L"edit" };
    ActivationRegistrationManager::RegisterForFileTypeActivation(
        myFileTypes,
        exePathAndIconIndex,
        L"Contoso File Types",
        verbs,
        exePath
    );
    OutputMessage(L"Registered for file activation");

    // Register some URI schemes for protocol activation,
    // specifying the scheme name, icon, display name and EXE path.
    ActivationRegistrationManager::RegisterForProtocolActivation(
        L"foo",
        exePathAndIconIndex,
        L"Contoso Foo Protocol",
        exePath
    );
    OutputMessage(L"Registered for protocol activation");

    // Register for startup activation.
    // As we're registering for startup activation multiple times,
    // and this is a multi-instance app, we'll get multiple instances
    // activated at startup.
    ActivationRegistrationManager::RegisterForStartupActivation(
        L"ContosoStartupId",
        exePath
    );
    OutputMessage(L"Registered for startup activation");
}

void UnregisterForActivation()
{
    // Unregister one or more registered filetypes.
    try
    {
        hstring myFileTypes[3] = { L".foo", L".foo2", L".foo3" };
        ActivationRegistrationManager::UnregisterForFileTypeActivation(
            myFileTypes,
            exePath
        );
        OutputMessage(L"Unregistered for file activation");
    }
    catch (...)
    {
        OutputMessage(L"Error unregistering file types");
    }

    // Unregister a protocol scheme.
    ActivationRegistrationManager::UnregisterForProtocolActivation(
        L"foo",
        L"");
    OutputMessage(L"Unregistered for protocol activation");

    // Unregister for startup activation.
    ActivationRegistrationManager::UnregisterForStartupActivation(
        L"ContosoStartupId");
    OutputMessage(L"Unregistered for startup activation");
}

void GetActivationInfo()
{
    AppActivationArguments args =
        AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        auto launchArgs = args.Data().as<ILaunchActivatedEventArgs>();
        if (launchArgs)
        {
            winrt::hstring argString = launchArgs.Arguments();
            std::vector<std::wstring> argStrings = SplitStrings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring const& s : argStrings)
            {
                OutputMessage(s.c_str());
            }
            // If the first argument is "Settings", we'll launch the Settings page.
            if (argStrings.size() > 1 && wcscmp(argStrings[1].c_str(), L"Settings") == 0)
            {
                LaunchSettingsPage();
            }
            else
            {
                LaunchMainPage();
            }
        }
    }
    else if (kind == ExtendedActivationKind::File)
    {
        auto fileArgs = args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
    else if (kind == ExtendedActivationKind::Protocol)
    {
        auto protocolArgs = args.Data().as<IProtocolActivatedEventArgs>();
        if (protocolArgs)
        {
            Uri uri = protocolArgs.Uri();
            OutputFormattedMessage(
                L"Protocol activation: %s", uri.RawUri().c_str());
        }
    }
    else if (kind == ExtendedActivationKind::StartupTask)
    {
        auto startupArgs = args.Data().as<IStartupTaskActivatedEventArgs>();
        if (startupArgs)
        {
            OutputFormattedMessage(
                L"Startup activation: %s", startupArgs.TaskId().c_str());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////

// Dummy methods for illustration only.
void LaunchSettingsPage() { OutputMessage(L"Launching Settings page..."); }
void LaunchMainPage() { OutputMessage(L"Launching Main page..."); }
