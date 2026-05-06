// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using System;
using System.Collections.Generic;

namespace WindowsML.Shared
{
    public static class PerformanceConfigurator
    {
        /// <summary>
        /// Returns EP-specific session options for the given execution provider name.
        ///
        /// The string literals below come from two sources that may use different names
        /// for the same provider:
        ///   1. ORT runtime — OrtEpDevice.EpName returned by ExecutionProviderCatalog.FindAllProviders()
        ///   2. Model catalog JSON — the "executionProviders[].name" field in files like
        ///      Resources/SqueezeNetModelCatalog.json
        ///
        /// When a provider appears under different names in these two sources, both forms
        /// must be listed here so the lookup succeeds regardless of which name is passed in.
        /// If a new EP is added to the ORT runtime or the catalog introduces an alias,
        /// a corresponding entry should be added below.
        /// </summary>
        public static Dictionary<string, string> GetEpOptions(string epName, PerformanceMode mode)
        {
            string normalized = (epName ?? string.Empty).Trim().ToUpperInvariant();
            return normalized switch
            {
                "OPENVINOEXECUTIONPROVIDER" => GetOpenVinoOptions(mode),           // ORT runtime name
                "QNNEXECUTIONPROVIDER" => GetQnnOptions(mode),                     // ORT runtime name
                "VITISAIEXECUTIONPROVIDER" => GetVitisAiOptions(mode),             // ORT runtime name
                "MIGRAPHXEXECUTIONPROVIDER" => GetMiGraphxOptions(mode),           // ORT runtime name
                "TENSORRTRTXEXECUTIONPROVIDER"                                     // ORT runtime name
                    or "NVTENSORRTRTXEXECUTIONPROVIDER"                            // catalog JSON name (SqueezeNetModelCatalog.json)
                    => GetTensorRtRtxOptions(mode),
                _ => new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase)
            };
        }

        private static Dictionary<string, string> GetOpenVinoOptions(PerformanceMode mode)
        {
            var options = CreateOptions();
            if (mode == PerformanceMode.MaxPerformance)
            {
                // Use config file for OpenVINO EP options (preferred approach)
                // Config file: openvino_perf.json with "PERFORMANCE_HINT": "LATENCY" and "EXECUTION_MODE_HINT": "PERFORMANCE"
                // "PERFORMANCE_HINT" can have values "LATENCY" or "THROUGHPUT" and both are related to max performance but for different use cases.
                options["load_config"] = "openvino_perf.json";
            }
            else if (mode == PerformanceMode.MaxEfficiency)
            {
                // Use config file for OpenVINO EP options (preferred approach)
                // Config file: openvino_efficiency.json
                options["load_config"] = "openvino_efficiency.json";
            }

            return options;
        }

        private static Dictionary<string, string> GetQnnOptions(PerformanceMode mode)
        {
            var options = CreateOptions();
            if (mode == PerformanceMode.MaxPerformance)
            {
                options["htp_performance_mode"] = "burst";
            }
            else if (mode == PerformanceMode.MaxEfficiency)
            {
                options["htp_performance_mode"] = "high_power_saver";
            }
            else
            {
                options["htp_performance_mode"] = "balanced";
            }

            return options;
        }

        private static Dictionary<string, string> GetVitisAiOptions(PerformanceMode mode)
        {
            var options = CreateOptions();
            return options;
        }

        private static Dictionary<string, string> GetMiGraphxOptions(PerformanceMode mode)
        {
            var options = CreateOptions();
            options["migraphx_exhaustive_tune"] = mode == PerformanceMode.MaxPerformance ? "1" : "0";
            return options;
        }

        private static Dictionary<string, string> GetTensorRtRtxOptions(PerformanceMode mode)
        {
            var options = CreateOptions();

            if (mode == PerformanceMode.MaxPerformance)
            {
                options["enable_cuda_graph"] = "1";
            }

            return options;
        }

        private static Dictionary<string, string> CreateOptions()
        {
            return new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
        }
    }
}
