// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ExecutionProviderManager.h"
#include "ArgumentParser.h"
#include <iostream>
#include <iomanip>
#include <unordered_map>
#include <winrt/Windows.Foundation.h>

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
            std::wcout << "Provider: " << provider.Name().c_str() << std::endl;
            try
            {
                auto readyState = provider.ReadyState();
                std::wcout << "  Ready state: " << static_cast<int>(readyState) << std::endl;

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

    void ExecutionProviderManager::ConfigureExecutionProviders(Ort::SessionOptions& session_options, Ort::Env& env)
    {
        // Get all available EP devices from the environment
        std::vector<Ort::ConstEpDevice> ep_devices = env.GetEpDevices();

        std::cout << "Discovered " << ep_devices.size() << " execution provider device(s)" << std::endl;
        PrintExecutionProviderInfo(ep_devices);

        // Accumulate devices by ep_name
        // Passing all devices for a given EP in a single call allows the execution provider
        // to select the best configuration or combination of devices, rather than being limited
        // to a single device. This enables optimal use of available hardware if supported by the EP.
        std::unordered_map<std::string, std::vector<Ort::ConstEpDevice>> ep_device_map;
        for (const auto& device : ep_devices)
        {
            ep_device_map[device.EpName()].push_back(device);
        }

        // Configure and append each EP type only once, with all its devices
        for (const auto& [ep_name, devices] : ep_device_map)
        {
            Ort::KeyValuePairs ep_options;
            if (ep_name == "VitisAIExecutionProvider")
            {
                // Append the Vitis AI EP
                session_options.AppendExecutionProvider_V2(env, devices, ep_options);
                std::cout << "Successfully added " << ep_name << " EP" << std::endl;
            }
            else if (ep_name == "OpenVINOExecutionProvider")
            {
                // Configure OpenVINO EP - pick the first device found.
                session_options.AppendExecutionProvider_V2(env, {devices.front()}, ep_options);
                std::cout << "Successfully added " << ep_name << " EP (first device only)" << std::endl;
            }
            else if (ep_name == "QNNExecutionProvider")
            {
                // Configure performance mode for QNN EP
                ep_options.Add("htp_performance_mode", "high_performance");
                session_options.AppendExecutionProvider_V2(env, devices, ep_options);
                std::cout << "Successfully added " << ep_name << " EP" << std::endl;
            }
            else if (ep_name == "NvTensorRTRTXExecutionProvider")
            {
                // Add any specific options for the NVIDIA TensorRT RTX EP if needed
                session_options.AppendExecutionProvider_V2(env, devices, ep_options);
                std::cout << "Successfully added " << ep_name << " EP" << std::endl;
            }
            else
            {
                std::cout << "Skipping EP: " << ep_name << std::endl;
            }
        }
    }

    void ExecutionProviderManager::PrintExecutionProviderInfo(const std::vector<Ort::ConstEpDevice>& ep_devices)
    {
        std::unordered_map<std::string, std::vector<Ort::ConstEpDevice>> ep_device_map;
        for (const auto& device : ep_devices)
        {
            ep_device_map[device.EpName()].push_back(device);
        }

        std::wcout << L"Execution Provider Information:\n";
        std::wcout << L"-------------------------------------------------------------\n";
        std::wcout << std::left << std::setw(32) << L"Provider" << std::setw(16) << L"Vendor" << std::setw(12) << L"Device Type"
                   << std::endl;
        std::wcout << L"-------------------------------------------------------------\n";
        for (const auto& [ep_name, devices] : ep_device_map)
        {
            std::cout << std::left << std::setw(32) << ep_name;
            for (const auto& device : devices)
            {
                std::cout << std::setw(16) << device.EpVendor() << std::setw(12)
                          << ArgumentParser::ToString(device.Device().Type()) << std::endl;
            }
        }
        std::wcout << L"-------------------------------------------------------------\n";
    }

} // namespace Shared
} // namespace WindowsML
