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
        if (normalized == "TENSORRTEXECUTIONPROVIDER")
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
            options["hint"] = "THROUGHPUT";
            options["enable_qdq_optimizer"] = "true";
            options["ep.context_enable"] = "1";
        }
        else if (mode == PerformanceMode::MaxEfficiency)
        {
            options["hint"] = "LATENCY";
            options["enable_opencl_throttling"] = "1";
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
            options["htp_performance_mode"] = "low_power_saver";
            options["rpc_control_latency"] = "1000";
        }
        else
        {
            options["htp_performance_mode"] = "balanced";
        }

        return options;
    }

    static std::unordered_map<std::string, std::string> GetVitisAiOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        if (mode == PerformanceMode::MaxPerformance)
        {
            options["config_file"] = "vaip_perf.json";
        }
        else if (mode == PerformanceMode::MaxEfficiency)
        {
            options["config_file"] = "vaip_efficiency.json";
        }

        return options;
    }

    static std::unordered_map<std::string, std::string> GetMiGraphXOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        if (mode == PerformanceMode::MaxPerformance)
        {
            options["exhaustive_tune"] = "1";
        }
        else
        {
            options["exhaustive_tune"] = "0";
        }

        return options;
    }

    static std::unordered_map<std::string, std::string> GetTensorRtRtxOptions(PerformanceMode mode)
    {
        std::unordered_map<std::string, std::string> options;
        options["trt_engine_cache_enable"] = "1";
        options["trt_timing_cache_enable"] = "1";

        if (mode == PerformanceMode::MaxPerformance)
        {
            options["trt_builder_optimization_level"] = "5";
        }
        else if (mode == PerformanceMode::MaxEfficiency)
        {
            options["trt_builder_optimization_level"] = "2";
        }
        else
        {
            options["trt_builder_optimization_level"] = "3";
        }

        return options;
    }
};
