// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

#include <windows.h>
#include <wil/resource.h>
#include <iostream>

#include "winrt\Windows.Foundation.h"
#include "winrt\Windows.Foundation.Collections.h"
#include "winrt\Microsoft.ApplicationModel.Resources.h"

#include <MddBootstrap.h>

using namespace winrt;
using namespace winrt::Microsoft::ApplicationModel::Resources;

HRESULT LoadProjectReunion()
{
    // Take a dependency on Project Reunion v0.8 preview.
    const UINT32 majorMinorVersion{ 0x00000008 };
    PCWSTR versionTag{ L"preview" };
    const PACKAGE_VERSION minVersion{};
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%08X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    return S_OK;
}

int wmain(int argc, wchar_t* argv[])
{
    // Print usage help.
    if ((argc < 2) || (_wcsicmp(argv[1], L"--help") == 0) || (_wcsicmp(argv[1], L"-?") == 0))
    {
        std::wcout << "Usage: console_unpackaged_app.exe [options] [mode]\n"
            "options:\n"
            "  -?, --help = Display help\n"
            "mode:\n"
            "  Mode for app run. Can be...\n"
            "       Default or default\n"
            "       Override or override\n"
            "       Fallback or fallback\n"
            "  Default and override retrieve a sample string from string resource files.\n"
            "  For the override case, this sample uses the German language.\n"
            "  Fallback corresponds to the resource-not-found case, where we fallback to a legacy resource loader.\n"
            "\n"
            "Examples:\n"
            "  Get the sample string for the default resource context\n"
            "    console_unpackaged_app.exe default\n"
            "  Get the sample string for the override resource context (sample uses the German language for the override context)\n"
            "    console_unpackaged_app.exe override\n"
            "  Get the sample string for the resource-not-found fallback case\n"
            "    console_unpackaged_app.exe fallback\n";
        return 1;
    }

    // Initialize dynamic dependencies so we can consume the Project Reunion APIs in the Project Reunion framework package from this unpackaged app. 
    HRESULT loadProjectReunionHr = LoadProjectReunion();
    if (FAILED(loadProjectReunionHr))
    {
        std::wcout << "Could not load Project Reunion!" << std::endl;
        return 1;
    }

    // Uninitialize dynamic dependencies.
    auto cleanup = wil::scope_exit([]
        {
            MddBootstrapShutdown();
        });


    // Required for C++/WinRT. This call associates this thread with an apartment and initializes COM runtime.
    init_apartment();

    // Create a resource manager using the resources index (PRI file) generated during build.
    auto factory = winrt::get_activation_factory<ResourceManager, IResourceManagerFactory>();
    ResourceManager manager = factory.CreateInstance(L"console_unpackaged_app.pri");

    manager.ResourceNotFound([](ResourceManager const&, ResourceNotFoundEventArgs const& args)
        {
            // There could be a resource in a legacy resource file that we retrieve using the corresponding legacy resource loader. For example, the C#
            // MRT Core sample apps in this repo use the .NET resource loader as their fallback. Instead, we just hardcode a resource value here.
            if (args.Name() == L"Resources/LegacyString")
            {
                auto candidate = ResourceCandidate(ResourceCandidateKind::String, L"Legacy Sample");
                args.SetResolvedCandidate(candidate);
            }
        });

    if ((_wcsicmp(argv[1], L"Default") == 0) || (_wcsicmp(argv[1], L"default") == 0))
    {
        std::wcout << manager.MainResourceMap().GetValue(L"Resources/SampleString").ValueAsString().c_str() << std::endl;
    }
    else if ((_wcsicmp(argv[1], L"Override") == 0) || _wcsicmp(argv[1], L"override") == 0)
    {
        // Create a custom resource context. Set the language to German.
        ResourceContext overrideResourceContext = manager.CreateResourceContext();
        overrideResourceContext.QualifierValues().Insert(L"Language", L"de-DE");

        // Use the custom resource context for the resource lookup.
        std::wcout << manager.MainResourceMap().GetValue(L"Resources/SampleString", overrideResourceContext).ValueAsString().c_str() << std::endl;
    }
    else if ((_wcsicmp(argv[1], L"Fallback") == 0) || (_wcsicmp(argv[1], L"fallback") == 0))
    {
        std::wcout << manager.MainResourceMap().GetValue(L"Resources/LegacyString").ValueAsString().c_str() << std::endl;
    }
    else
    {
        std::wcout << "Invalid argument!" << std::endl;
    }

    return 0;
}
