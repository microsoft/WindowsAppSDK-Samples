// Copyright (C) Microsoft Corporation. All rights reserved.

//
// Windows ML Execution Provider Catalog Sample
//
// Demonstrates discovering certified ML execution providers, preparing them for use,
// and registering them with ONNX Runtime. This mirrors the behavior of the
// ExecutionProviderCatalog::RegisterCertifiedProviders WinRT API.
//

#include <WinMLEpCatalog.h>
#include <onnxruntime_cxx_api.h>

#include <windows.h>

#include <filesystem>
#include <format>
#include <iostream>
#include <string>
#include <vector>

struct RegistrationContext
{
    Ort::Env* env;
    std::vector<std::string> registeredProviders;
};

static BOOL CALLBACK RegisterCertifiedProvider(WinMLEpHandle ep, const WinMLEpInfo* info, void* context)
{
    if (!info || !info->name)
    {
        return TRUE;
    }

    auto* ctx = static_cast<RegistrationContext*>(context);

    std::cout << std::format("Provider: {}\n", info->name);

    if (info->certification != WinMLEpCertification_Certified)
    {
        std::cout << "  Skipping (not certified)\n\n";
        return TRUE;
    }

    HRESULT hr = WinMLEpEnsureReady(ep);
    if (FAILED(hr))
    {
        std::cout << std::format("  EnsureReady failed: 0x{:08X}\n\n", static_cast<uint32_t>(hr));
        return TRUE;
    }

    size_t pathSize = 0;
    hr = WinMLEpGetLibraryPathSize(ep, &pathSize);
    if (FAILED(hr) || pathSize == 0)
    {
        std::cout << "  Failed to get library path\n\n";
        return TRUE;
    }

    std::string libraryPathUtf8(pathSize, '\0');
    size_t used = 0;
    hr = WinMLEpGetLibraryPath(ep, pathSize, libraryPathUtf8.data(), &used);
    if (FAILED(hr) || used == 0)
    {
        std::cout << "  Failed to get library path\n\n";
        return TRUE;
    }
    libraryPathUtf8.resize(used - 1);

    std::cout << std::format("  Library: {}\n", libraryPathUtf8);

    try
    {
        std::filesystem::path libraryPath(libraryPathUtf8);
        ctx->env->RegisterExecutionProviderLibrary(info->name, libraryPath.wstring());
        ctx->registeredProviders.push_back(info->name);
        std::cout << "  Registered!\n\n";
    }
    catch (const Ort::Exception& e)
    {
        std::cout << std::format("  Registration failed: {}\n\n", e.what());
    }

    return TRUE;
}

int main()
{
    std::cout << "Windows ML Execution Provider Catalog Sample\n\n";

    try
    {
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "WinMLEpCatalogSample");

        WinMLEpCatalogHandle catalog = nullptr;
        HRESULT hr = WinMLEpCatalogCreate(&catalog);
        if (FAILED(hr))
        {
            std::cerr << std::format("Failed to create catalog: 0x{:08X}\n", static_cast<uint32_t>(hr));
            return 1;
        }

        RegistrationContext ctx{&env, {}};
        hr = WinMLEpCatalogEnumProviders(catalog, RegisterCertifiedProvider, &ctx);
        if (FAILED(hr))
        {
            std::cerr << std::format("Enumeration failed: 0x{:08X}\n", static_cast<uint32_t>(hr));
        }

        // The catalog is no longer needed after enumeration.
        WinMLEpCatalogRelease(catalog);

        auto epDevices = env.GetEpDevices();
        std::cout << std::format("Available EP devices ({}):\n", epDevices.size());
        for (const auto& device : epDevices)
        {
            const char* epName = device.EpName();
            if (epName)
            {
                std::cout << std::format("  - {}\n", epName);
            }
        }

        std::cout << std::format("\nRegistered {} certified provider(s).\n", ctx.registeredProviders.size());

        // Registered EP libraries hold references to resources owned by the
        // Ort::Env.  They must be explicitly unregistered before the Env
        // destructor runs, otherwise the provider DLLs may be unloaded while
        // still in use, leading to access violations during shutdown.
        for (const auto& name : ctx.registeredProviders)
        {
            env.UnregisterExecutionProviderLibrary(name.c_str());
        }
    }
    catch (const Ort::Exception& e)
    {
        std::cerr << std::format("ORT error: {}\n", e.what());
        return 1;
    }

    return 0;
}

