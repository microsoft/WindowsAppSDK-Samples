// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "InferenceEngine.h"
#include "ExecutionProviderManager.h"
#include "ModelManager.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <optional>
#include <system_error>

namespace
{
    enum class ExistingCompiledModelStatus
    {
        Optimal,
        MissingMetadata,
        NotApplicable,
        PreferRecompilation,
        Unsupported,
        NoMatchingDevices,
        ValidationError
    };

    struct CompatibilityProbe
    {
        std::string executionProvider;
        std::vector<std::string> deviceTypes;
        std::optional<OrtCompiledModelCompatibility> compatibility;
        std::string detail;
    };

    struct ExistingCompiledModelCheck
    {
        ExistingCompiledModelStatus status = ExistingCompiledModelStatus::ValidationError;
        std::string detail;
        std::vector<CompatibilityProbe> probes;
    };

    struct EpDeviceGroup
    {
        std::string name;
        std::vector<Ort::ConstEpDevice> devices;
    };

    std::string WideToUtf8(const std::wstring& value)
    {
        if (value.empty())
        {
            return {};
        }

        const int size = WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), nullptr, 0, nullptr, nullptr);
        if (size <= 0)
        {
            return {};
        }

        std::string result(static_cast<size_t>(size), '\0');
        WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.size()), result.data(), size, nullptr, nullptr);
        return result;
    }

    std::vector<EpDeviceGroup> GroupDevicesByExecutionProvider(const std::vector<Ort::ConstEpDevice>& devices)
    {
        std::vector<EpDeviceGroup> groups;
        std::map<std::string, size_t> groupIndexes;

        for (const auto& device : devices)
        {
            const std::string epName(device.EpName());
            auto [it, inserted] = groupIndexes.emplace(epName, groups.size());
            if (inserted)
            {
                groups.push_back({epName, {}});
            }
            groups[it->second].devices.push_back(device);
        }

        return groups;
    }

    OrtHardwareDeviceType GetPreferredDeviceType(OrtExecutionProviderDevicePolicy policy)
    {
        switch (policy)
        {
        case OrtExecutionProviderDevicePolicy_PREFER_NPU:
        case OrtExecutionProviderDevicePolicy_MAX_EFFICIENCY:
        case OrtExecutionProviderDevicePolicy_MIN_OVERALL_POWER:
            return OrtHardwareDeviceType_NPU;
        case OrtExecutionProviderDevicePolicy_PREFER_GPU:
        case OrtExecutionProviderDevicePolicy_MAX_PERFORMANCE:
            return OrtHardwareDeviceType_GPU;
        case OrtExecutionProviderDevicePolicy_DEFAULT:
        case OrtExecutionProviderDevicePolicy_PREFER_CPU:
        default:
            return OrtHardwareDeviceType_CPU;
        }
    }

    std::vector<EpDeviceGroup> SelectDeviceGroups(
        const std::vector<EpDeviceGroup>& groups,
        const WindowsML::Shared::CommandLineOptions& options)
    {
        std::vector<EpDeviceGroup> selectedGroups;

        if (!options.ep_name.empty())
        {
            const std::string requestedEp = WideToUtf8(options.ep_name);
            const auto group = std::find_if(groups.begin(), groups.end(), [&](const EpDeviceGroup& candidate) {
                return candidate.name == requestedEp;
            });
            if (group == groups.end())
            {
                return selectedGroups;
            }

            if (!options.device_type.has_value())
            {
                selectedGroups.push_back(*group);
                return selectedGroups;
            }

            const std::string requestedDeviceType = WideToUtf8(options.device_type.value());
            const auto device = std::find_if(group->devices.begin(), group->devices.end(), [&](const Ort::ConstEpDevice& candidate) {
                return WindowsML::Shared::ArgumentParser::ToString(candidate.Device().Type()) == requestedDeviceType;
            });
            if (device != group->devices.end())
            {
                selectedGroups.push_back({group->name, {*device}});
            }
            return selectedGroups;
        }

        const OrtHardwareDeviceType preferredType =
            GetPreferredDeviceType(options.ep_policy.value_or(OrtExecutionProviderDevicePolicy_DEFAULT));
        for (const auto& group : groups)
        {
            EpDeviceGroup selectedGroup{group.name, {}};
            std::copy_if(group.devices.begin(), group.devices.end(), std::back_inserter(selectedGroup.devices),
                [&](const Ort::ConstEpDevice& device) {
                    const OrtHardwareDeviceType deviceType = device.Device().Type();
                    return deviceType == preferredType || deviceType == OrtHardwareDeviceType_CPU;
                });
            if (!selectedGroup.devices.empty())
            {
                selectedGroups.push_back(std::move(selectedGroup));
            }
        }

        return selectedGroups;
    }

    ExistingCompiledModelStatus ToStatus(OrtCompiledModelCompatibility compatibility)
    {
        switch (compatibility)
        {
        case OrtCompiledModelCompatibility_EP_SUPPORTED_OPTIMAL:
            return ExistingCompiledModelStatus::Optimal;
        case OrtCompiledModelCompatibility_EP_SUPPORTED_PREFER_RECOMPILATION:
            return ExistingCompiledModelStatus::PreferRecompilation;
        case OrtCompiledModelCompatibility_EP_UNSUPPORTED:
            return ExistingCompiledModelStatus::Unsupported;
        case OrtCompiledModelCompatibility_EP_NOT_APPLICABLE:
        default:
            return ExistingCompiledModelStatus::NotApplicable;
        }
    }

    const char* ToString(ExistingCompiledModelStatus status)
    {
        switch (status)
        {
        case ExistingCompiledModelStatus::Optimal:
            return "optimal";
        case ExistingCompiledModelStatus::MissingMetadata:
            return "missing metadata";
        case ExistingCompiledModelStatus::NotApplicable:
            return "not applicable";
        case ExistingCompiledModelStatus::PreferRecompilation:
            return "recompilation preferred";
        case ExistingCompiledModelStatus::Unsupported:
            return "unsupported";
        case ExistingCompiledModelStatus::NoMatchingDevices:
            return "no matching devices";
        case ExistingCompiledModelStatus::ValidationError:
        default:
            return "validation error";
        }
    }

    const char* ToString(OrtCompiledModelCompatibility compatibility)
    {
        switch (compatibility)
        {
        case OrtCompiledModelCompatibility_EP_SUPPORTED_OPTIMAL:
            return "EP_SUPPORTED_OPTIMAL";
        case OrtCompiledModelCompatibility_EP_SUPPORTED_PREFER_RECOMPILATION:
            return "EP_SUPPORTED_PREFER_RECOMPILATION";
        case OrtCompiledModelCompatibility_EP_UNSUPPORTED:
            return "EP_UNSUPPORTED";
        case OrtCompiledModelCompatibility_EP_NOT_APPLICABLE:
        default:
            return "EP_NOT_APPLICABLE";
        }
    }
}

namespace WindowsML
{
namespace Shared
{

    Ort::SessionOptions InferenceEngine::CreateSessionOptions(const CommandLineOptions& options, Ort::Env& env)
    {
        Ort::SessionOptions sessionOptions;

        if (options.ep_policy.has_value())
        {
            // Set the EP selection policy based on command line
            std::cout << "Using EP Selection Policy: " << ArgumentParser::ToString(options.ep_policy.value()) << std::endl;
            sessionOptions.SetEpSelectionPolicy(options.ep_policy.value());
        }
        else
        {
            // Use explicit configuration
            std::cout << "Using explicit EP configuration" << std::endl;
            ExecutionProviderManager::ConfigureSelectedExecutionProvider(
                sessionOptions,
                env,
                options.ep_name,
                options.device_type,
                options.perf_mode);
        }

        return sessionOptions;
    }

    std::filesystem::path InferenceEngine::DetermineModelPath(
        const CommandLineOptions& options,
        const std::filesystem::path& modelPath,
        const std::filesystem::path& compiledModelPath,
        Ort::SessionOptions& sessionOptions,
        Ort::Env& env)
    {
        std::filesystem::path actualModelPath;
        std::error_code compiledModelPathError;
        bool isCompiledModelAvailable = std::filesystem::exists(compiledModelPath, compiledModelPathError);
        bool shouldCompile = options.compile_model;

        if (compiledModelPathError)
        {
            std::cout << "Could not inspect compiled model path " << compiledModelPath
                      << ": " << compiledModelPathError.message()
                      << ". Using the original model unless compilation succeeds." << std::endl;
            isCompiledModelAvailable = false;
        }

        if (isCompiledModelAvailable)
        {
            std::cout << "Validating existing compiled model: " << compiledModelPath << std::endl;
            ExistingCompiledModelCheck check;

            try
            {
                const auto deviceGroups =
                    SelectDeviceGroups(GroupDevicesByExecutionProvider(env.GetEpDevices()), options);
                if (deviceGroups.empty())
                {
                    check.status = ExistingCompiledModelStatus::NoMatchingDevices;
                    check.detail = "No execution provider devices matched the requested selection.";
                }
                else
                {
                    bool foundMetadata = false;
                    bool validationFailed = false;
                    std::optional<ExistingCompiledModelStatus> bestNonOptimalStatus;
                    Ort::AllocatorWithDefaultOptions allocator;

                    for (const auto& group : deviceGroups)
                    {
                        CompatibilityProbe probe;
                        probe.executionProvider = group.name;
                        for (const auto& device : group.devices)
                        {
                            probe.deviceTypes.push_back(ArgumentParser::ToString(device.Device().Type()));
                        }

                        try
                        {
                            auto compatibilityInfo =
                                Ort::GetCompatibilityInfoFromModelAllocated(compiledModelPath.c_str(), group.name.c_str(), allocator);
                            if (!compatibilityInfo)
                            {
                                probe.detail = "No compatibility metadata was found for this execution provider.";
                                check.probes.push_back(std::move(probe));
                                continue;
                            }

                            foundMetadata = true;
                            const OrtCompiledModelCompatibility compatibility =
                                Ort::GetModelCompatibilityForEpDevices(group.devices, compatibilityInfo.get());
                            probe.compatibility = compatibility;
                            probe.detail = ToString(compatibility);
                            check.probes.push_back(std::move(probe));

                            const ExistingCompiledModelStatus status = ToStatus(compatibility);
                            if (status != ExistingCompiledModelStatus::Optimal &&
                                (!bestNonOptimalStatus.has_value() ||
                                 status == ExistingCompiledModelStatus::PreferRecompilation ||
                                 (status == ExistingCompiledModelStatus::Unsupported &&
                                  bestNonOptimalStatus.value() == ExistingCompiledModelStatus::NotApplicable)))
                            {
                                bestNonOptimalStatus = status;
                            }
                        }
                        catch (const Ort::Exception& ex)
                        {
                            validationFailed = true;
                            probe.detail = std::string("Compatibility validation failed: ") + ex.what();
                            check.probes.push_back(std::move(probe));
                        }
                    }

                    if (validationFailed)
                    {
                        check.status = ExistingCompiledModelStatus::ValidationError;
                        check.detail = "Compatibility validation failed for one or more applicable execution provider groups.";
                    }
                    else if (!foundMetadata)
                    {
                        check.status = ExistingCompiledModelStatus::MissingMetadata;
                        check.detail = "The compiled model contains no compatibility metadata for the selected execution providers.";
                    }
                    else if (bestNonOptimalStatus.has_value())
                    {
                        check.status = bestNonOptimalStatus.value();
                        check.detail = "At least one applicable execution provider reported non-optimal compatibility.";
                    }
                    else
                    {
                        check.status = ExistingCompiledModelStatus::Optimal;
                        check.detail = "All metadata-bearing execution provider groups reported optimal compatibility.";
                    }
                }
            }
            catch (const Ort::Exception& ex)
            {
                check.status = ExistingCompiledModelStatus::ValidationError;
                check.detail = std::string("Failed to enumerate execution provider devices: ") + ex.what();
            }

            std::cout << "Compiled model compatibility: " << ToString(check.status) << ". " << check.detail << std::endl;
            for (const auto& probe : check.probes)
            {
                std::cout << "  EP: " << probe.executionProvider << "; devices:";
                for (const auto& deviceType : probe.deviceTypes)
                {
                    std::cout << " " << deviceType;
                }
                std::cout << "; result: " << probe.detail << std::endl;
            }

            if (check.status == ExistingCompiledModelStatus::Optimal)
            {
                std::cout << "Using existing compiled model: " << compiledModelPath << std::endl;
                return compiledModelPath;
            }

            if (!options.compile_model)
            {
                std::cout << "The existing compiled model is not optimal; using original model: " << modelPath << std::endl;
                return modelPath;
            }

            std::cout << "The existing compiled model is not optimal; attempting to recompile it." << std::endl;
        }
        else if (shouldCompile)
        {
            std::cout << "No compiled model found, attempting to create compiled model" << std::endl;
        }

        if (shouldCompile)
        {
            const OrtApi& ortApi = Ort::GetApi();
            OrtStatus* status = ModelManager::CompileModel(ortApi, env, sessionOptions, modelPath, compiledModelPath);
            std::error_code compiledOutputError;
            const bool compiledOutputExists = std::filesystem::exists(compiledModelPath, compiledOutputError);

            if (status == nullptr && !compiledOutputError && compiledOutputExists)
            {
                std::cout << "Compiled model created successfully at " << compiledModelPath << std::endl;
                actualModelPath = compiledModelPath;
            }
            else
            {
                if (status != nullptr)
                {
                    ortApi.ReleaseStatus(status);
                }
                if (compiledOutputError)
                {
                    std::cout << "Could not inspect compiled output " << compiledModelPath
                              << ": " << compiledOutputError.message() << std::endl;
                }
                std::cout << "Compilation did not produce a usable model; falling back to original model: " << modelPath << std::endl;
                actualModelPath = modelPath;
            }
        }
        else
        {
            std::cout << "Using original model: " << modelPath << std::endl;
            actualModelPath = modelPath;
        }

        return actualModelPath;
    }

    Ort::Value InferenceEngine::CreateInputTensor(const std::vector<float>& inputTensorValues, const std::vector<int64_t>& inputShape)
    {
        Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
        return Ort::Value::CreateTensor<float>(
            memoryInfo, const_cast<float*>(inputTensorValues.data()), inputTensorValues.size(), inputShape.data(), inputShape.size());
    }

    std::vector<Ort::Value> InferenceEngine::RunInference(Ort::Session& session, const char* inputName, const char* outputName, Ort::Value& inputTensor)
    {
        const char* inputNames[] = {inputName};
        const char* outputNames[] = {outputName};

        std::cout << "Running inference ..." << std::endl;
        return session.Run(Ort::RunOptions{nullptr}, inputNames, &inputTensor, 1, outputNames, 1);
    }

    std::vector<float> InferenceEngine::ExtractResults(std::vector<Ort::Value>& outputTensors)
    {
        float* outputData = outputTensors[0].GetTensorMutableData<float>();
        const size_t outputCount = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
        return std::vector<float>(outputData, outputData + outputCount);
    }

    std::vector<int64_t> InferenceEngine::PrepareInputShape(const std::vector<int64_t>& originalShape)
    {
        std::vector<int64_t> inputShape = originalShape;

        // For dynamic dimensions, set them to fixed values
        for (size_t i = 0; i < inputShape.size(); i++)
        {
            if (inputShape[i] == -1)
            {
                inputShape[i] = 1; // Assume batch size of 1
            }
        }

        return inputShape;
    }

} // namespace Shared
} // namespace WindowsML
