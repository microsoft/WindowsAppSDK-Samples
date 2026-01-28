// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <unordered_map>

/// <summary>
/// Execution provider performance mode for GenAI
/// </summary>
enum class PerformanceMode
{
    Default,
    MaxPerformance,
    MaxEfficiency
};

/// <summary>
/// Helper class to get EP-specific performance options for GenAI.
/// Returns std::unordered_map for use with OgaConfig::SetProviderOption.
/// </summary>
class GenAIPerformanceConfigurator
{
public:
    static std::unordered_map<std::string, std::string> GetEpOptions(const std::string& ep_name, PerformanceMode mode)
    {
        std::string normalized = NormalizeName(ep_name);
        if (normalized == "OPENVINOEXECUTIONPROVIDER")
        {
            return GetOpenVinoOptions(mode);
        }
        if (normalized == "QNNEXECUTIONPROVIDER")
        {
            return GetQnnOptions(mode);
        }
        if (normalized == "VITISAIEXECUTIONPROVIDER")
        {
            return GetVitisAiOptions(mode);
        }
        if (normalized == "MIGRAPHXEXECUTIONPROVIDER")
        {
            return GetMiGraphXOptions(mode);
        }
        if (normalized == "TENSORRTRTXEXECUTIONPROVIDER")
        {
            return GetTensorRtRtxOptions(mode);
        }

        return {};
    }

private:
    static std::string NormalizeName(const std::string& ep_name)
    {
        std::string normalized = ep_name;
        std::transform(normalized.begin(), normalized.end(), normalized.begin(), [](unsigned char ch) {
            return static_cast<char>(std::toupper(ch));
        });
        return normalized;
    }

    static std::unordered_map<std::string, std::string> GetOpenVinoOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        if (mode == PerformanceMode::MaxPerformance)
        {
            options["load_config"] = "openvino_perf.json";
        }
        else if (mode == PerformanceMode::MaxEfficiency)
        {
            options["load_config"] = "openvino_efficiency.json";
        }
        return options;
    }

    static std::unordered_map<std::string, std::string> GetQnnOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        if (mode == PerformanceMode::MaxPerformance)
        {
            options["htp_performance_mode"] = "burst";
        }
        else if (mode == PerformanceMode::MaxEfficiency)
        {
            options["htp_performance_mode"] = "high_power_saver";
        }
        else
        {
            options["htp_performance_mode"] = "balanced";
        }

        return options;
    }

    static std::unordered_map<std::string, std::string> GetVitisAiOptions(PerformanceMode /*mode*/)
    {
        return std::unordered_map<std::string, std::string>();
    }

    static std::unordered_map<std::string, std::string> GetMiGraphXOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        if (mode == PerformanceMode::MaxPerformance)
        {
            options["migraphx_exhaustive_tune"] = "1";
        }

        return options;
    }

    static std::unordered_map<std::string, std::string> GetTensorRtRtxOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        if (mode == PerformanceMode::MaxPerformance)
        {
            options["enable_cuda_graph"] = "1";
        }

        return options;
    }
};
