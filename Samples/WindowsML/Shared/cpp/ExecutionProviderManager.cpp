// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ExecutionProviderManager.h"
#include "ArgumentParser.h"
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <winrt/Windows.Foundation.h>
#include <algorithm>
#include <winrt/base.h>

namespace {
// UTF-16 (wstring) -> UTF-8 using C++/WinRT helper
std::string WideToUtf8(const std::wstring& wide)
{
    return winrt::to_string(winrt::hstring{ wide });
}
}

namespace WindowsML
{
namespace Shared
{

    void ExecutionProviderManager::InitializeProviders(bool allowDownload)
    {
        std::cout << "Getting available providers..." << std::endl;
        auto catalog = winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderCatalog::GetDefault();
        auto providers = catalog.FindAllProviders();
        for (const auto& provider : providers)
        {
            std::wcout << L"Provider: " << provider.Name().c_str() << std::endl;
            try
            {
                auto readyState = provider.ReadyState();
                std::wcout << L"  Ready state: " << static_cast<int>(readyState) << std::endl;

                // Only call EnsureReadyAsync if we allow downloads or if the provider is already ready
                if (allowDownload || readyState != winrt::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState::NotPresent)
                {
                    provider.EnsureReadyAsync().get();
                }

                provider.TryRegister();
            }
            catch (...)
            {
                // Continue if provider fails to initialize
            }
        }
    }

    bool ExecutionProviderManager::ConfigureSelectedExecutionProvider(Ort::SessionOptions& session_options,
                                                                      Ort::Env& env,
                                                                      const std::wstring& ep_name,
                                                                      const std::optional<std::wstring>& device_type)
    {
        // Discover devices
        std::vector<Ort::ConstEpDevice> ep_devices = env.GetEpDevices();

        // Accumulate devices by ep_name
        std::unordered_map<std::string, std::vector<Ort::ConstEpDevice>> ep_device_map;
        for (const auto& device : ep_devices)
        {
            std::string name(device.EpName());
            ep_device_map[name].push_back(device);
        }

        if (ep_name.empty())
        {
            std::wcout << L"ERROR: --ep_name is required when not using --ep_policy." << std::endl;
            PrintExecutionProviderInfo(ep_devices);
            return false;
        }

        const std::string ep_name_utf8 = WideToUtf8(ep_name);
        auto it = ep_device_map.find(ep_name_utf8);
        if (it == ep_device_map.end())
        {
            std::cout << "ERROR: Execution provider '" << ep_name_utf8 << "' not found among discovered devices." << std::endl;
            PrintExecutionProviderInfo(ep_devices);
            return false;
        }

        const auto& devices_for_ep = it->second;
        std::vector<Ort::ConstEpDevice> selected_devices;

        if (device_type.has_value())
        {
            std::cout << "Searching for device type '" << WideToUtf8(*device_type) << "' for provider '" << ep_name_utf8 << "'."
                      << std::endl;

            const std::string requested_type = WideToUtf8(*device_type);
            auto devices_itr = std::find_if(devices_for_ep.begin(), devices_for_ep.end(), [&](const Ort::ConstEpDevice& d) {
                return ArgumentParser::ToString(d.Device().Type()) == requested_type;
            });
            if (devices_itr == devices_for_ep.end())
            {
                std::cout << "ERROR: Device type '" << requested_type << "' not found for provider '" << ep_name_utf8 << "'." << std::endl;
                return false; // bail out
            }
            selected_devices.push_back(*devices_itr);
            std::cout << "Using device type '" << requested_type << "' for provider '" << ep_name_utf8 << "'." << std::endl;
        }
        else
        {
            // No device type specified: use all devices for the EP
            selected_devices = devices_for_ep;
            std::cout << "No device type specified; using the following device(s) for provider '" << ep_name_utf8 << "':" << std::endl;
            for (const auto& d : selected_devices)
            {
                std::cout << "  " << ArgumentParser::ToString(d.Device().Type()) << std::endl;
            }
        }

        // Finally, call append with the selected devices
        Ort::KeyValuePairs ep_options;
        try
        {
            session_options.AppendExecutionProvider_V2(env, selected_devices, ep_options);
            std::cout << "Successfully added " << ep_name_utf8 << " execution provider" << std::endl;
            return true;
        }
        catch (const Ort::Exception& ex)
        {
            std::cout << "ERROR: Failed to append execution provider '" << ep_name_utf8 << "': " << ex.what() << std::endl;
            return false;
        }
    }

    void ExecutionProviderManager::PrintExecutionProviderInfo(const std::vector<Ort::ConstEpDevice>& ep_devices)
    {
        std::unordered_map<std::string, std::vector<Ort::ConstEpDevice>> ep_device_map;
        for (const auto& device : ep_devices)
        {
            std::string name(device.EpName());
            ep_device_map[name].push_back(device);
        }

        std::wcout << L"Execution Provider Information:\n";
        std::wcout << L"-------------------------------------------------------------\n";
        std::wcout << std::left << std::setw(32) << L"Provider" << std::setw(16) << L"Vendor" << std::setw(12) << L"Device Type" << std::endl;
        std::wcout << L"-------------------------------------------------------------\n";
        for (auto& kv : ep_device_map)
        {
            const std::string& ep_name = kv.first;
            auto& devices = kv.second;
            for (const auto& device : devices)
            {
                std::cout << std::left << std::setw(32) << ep_name
                          << std::setw(16) << device.EpVendor() << std::setw(12)
                          << ArgumentParser::ToString(device.Device().Type()) << std::endl;
            }
        }
        std::wcout << L"-------------------------------------------------------------\n";
    }

    void ExecutionProviderManager::PrintExecutionProviderHelpTable()
    {
        try
        {
            Ort::Env env{};
            auto devices = env.GetEpDevices();
            if (!devices.empty())
            {
                PrintExecutionProviderInfo(devices);
            }
            else
            {
                std::wcout << L"(No execution provider devices discovered)\n";
            }
        }
        catch (...)
        {
            std::wcout << L"(Failed to query execution provider devices)\n";
        }
    }

} // namespace Shared
} // namespace WindowsML
