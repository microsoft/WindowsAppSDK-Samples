// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ArgumentParser.h"
#include <iostream>
#include <string_view>

namespace WindowsML
{
namespace Shared
{

    bool ArgumentParser::ParseCommandLineArgs(int argc, wchar_t* argv[], CommandLineOptions& options)
    {
        std::vector<std::wstring_view> arguments{argv, argv + argc};

        if (arguments.size() < 1)
        {
            PrintUsage();
            return false;
        }

        for (size_t i = 1; i < arguments.size(); ++i)
        {
            if (arguments[i] == L"--compile")
            {
                options.compile_model = true;
            }
            else if (arguments[i] == L"--download")
            {
                options.download_packages = true;
            }
            else if (arguments[i] == L"--model" && i + 1 < arguments.size())
            {
                options.model_path = arguments[++i];
            }
            else if (arguments[i] == L"--compiled_output" && i + 1 < arguments.size())
            {
                options.output_path = arguments[++i];
            }
            else if (arguments[i] == L"--image_path" && i + 1 < arguments.size())
            {
                options.image_path = arguments[++i];
            }
            else if (arguments[i] == L"--ep_policy" && i + 1 < arguments.size())
            {
                auto policy_str = arguments[++i];
                if (policy_str == L"NPU")
                {
                    options.ep_policy = OrtExecutionProviderDevicePolicy_PREFER_NPU;
                }
                else if (policy_str == L"CPU")
                {
                    options.ep_policy = OrtExecutionProviderDevicePolicy_PREFER_CPU;
                }
                else if (policy_str == L"GPU")
                {
                    options.ep_policy = OrtExecutionProviderDevicePolicy_PREFER_GPU;
                }
                else if (policy_str == L"DEFAULT")
                {
                    options.ep_policy = OrtExecutionProviderDevicePolicy_DEFAULT;
                }
                else if (policy_str == L"DISABLE")
                {
                    options.ep_policy = std::nullopt;
                }
                else
                {
                    std::wcout << L"Unknown EP policy: " << policy_str << L", using default (DISABLE)\n";
                }
            }
        }

        return true;
    }

    std::string ArgumentParser::ToString(OrtExecutionProviderDevicePolicy policy)
    {
        switch (policy)
        {
        case OrtExecutionProviderDevicePolicy_PREFER_NPU:
            return "PREFER_NPU";
        case OrtExecutionProviderDevicePolicy_PREFER_CPU:
            return "PREFER_CPU";
        case OrtExecutionProviderDevicePolicy_PREFER_GPU:
            return "PREFER_GPU";
        case OrtExecutionProviderDevicePolicy_DEFAULT:
            return "DEFAULT";
        default:
            return "UNKNOWN";
        }
    }

    std::string ArgumentParser::ToString(OrtHardwareDeviceType policy)
    {
        switch (policy)
        {
        case OrtHardwareDeviceType_NPU:
            return "NPU";
        case OrtHardwareDeviceType_CPU:
            return "CPU";
        case OrtHardwareDeviceType_GPU:
            return "GPU";
        default:
            return "";
        }
    }

    void ArgumentParser::PrintUsage()
    {
        std::wcout << L"Usage: Application.exe [options]\n"
                   << L"Options:\n"
                   << L"  --ep_policy <policy>  Set execution provider policy (NPU, CPU, GPU, DEFAULT, DISABLE). Default: "
                      L"DISABLE\n"
                   << L"  --compile                     Compile the model\n"
                   << L"  --download                    Download required packages\n"
                   << L"  --model <path>                Path to the input ONNX model (default: SqueezeNet.onnx in executable directory)\n"
                   << L"  --compiled_output <path>      Path for compiled output model (default: SqueezeNet_ctx.onnx)\n"
                   << L"  --image_path <path>           Path to the input image (default: image.jpg in the executable directory)\n";
    }

} // namespace Shared
} // namespace WindowsML
