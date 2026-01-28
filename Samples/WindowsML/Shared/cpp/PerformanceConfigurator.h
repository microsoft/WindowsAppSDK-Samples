// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <winml/onnxruntime_cxx_api.h>
#include "ArgumentParser.h"

namespace WindowsML
{
namespace Shared
{
    class PerformanceConfigurator
    {
    public:
        static Ort::KeyValuePairs GetEpOptions(const std::string& ep_name, PerformanceMode mode)
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

            Ort::KeyValuePairs options;
            return options;
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

        static Ort::KeyValuePairs GetOpenVinoOptions(PerformanceMode mode)
        {
            Ort::KeyValuePairs options;
            if (mode == PerformanceMode::MaxPerformance)
            {
                // Use config file for OpenVINO EP options (preferred approach)
                // Config file: openvino_perf.json with "PERFORMANCE_HINT": "LATENCY" and "EXECUTION_MODE_HINT": "PERFORMANCE"
                // "PERFORMANCE_HINT" can have values "LATENCY" or "THROUGHPUT" and both are related to max performance but for different use cases.
                options.Add("load_config", "openvino_perf.json");
            }
            else if (mode == PerformanceMode::MaxEfficiency)
            {
                // Use config file for OpenVINO EP options (preferred approach)
                // Config file: openvino_efficiency.json
                options.Add("load_config", "openvino_efficiency.json");
            }
            return options;
        }

        static Ort::KeyValuePairs GetQnnOptions(PerformanceMode mode)
        {
            Ort::KeyValuePairs options;
            if (mode == PerformanceMode::MaxPerformance)
            {
                options.Add("htp_performance_mode", "burst");
            }
            else if (mode == PerformanceMode::MaxEfficiency)
            {
                options.Add("htp_performance_mode", "high_power_saver");
            }
            else
            {
                options.Add("htp_performance_mode", "balanced");
            }

            return options;
        }

        static Ort::KeyValuePairs GetVitisAiOptions(PerformanceMode /*mode*/)
        {
            return Ort::KeyValuePairs(nullptr);
        }

        static Ort::KeyValuePairs GetMiGraphXOptions(PerformanceMode mode)
        {
            Ort::KeyValuePairs options;
            if (mode == PerformanceMode::MaxPerformance)
            {
                options.Add("migraphx_exhaustive_tune", "1");
            }

            return options;
        }

        static Ort::KeyValuePairs GetTensorRtRtxOptions(PerformanceMode mode)
        {
            Ort::KeyValuePairs options;

            if (mode == PerformanceMode::MaxPerformance)
            {
                options.Add("enable_cuda_graph", "1");
            }

            return options;
        }
    };

} // namespace Shared
} // namespace WindowsML
