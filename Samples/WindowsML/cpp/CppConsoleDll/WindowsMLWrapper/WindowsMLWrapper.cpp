// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

#include "WindowsMLWrapper.h"
#include <windows.h>
#include <combaseapi.h>
#include <cstring>
#include <string>

// Include ONNX Runtime headers from WindowsAppSDK.ML package
#include <winml/onnxruntime_cxx_api.h>

// Initializes an ORT environment to verify the WindowsML runtime is operational.
// Returns a success message if the runtime loads correctly.
extern "C" WINDOWSMLWRAPPER_API char* GetTestMessage()
{
    try
    {
        // Creating an Ort::Env is the canonical ORT initialization step.
        // If this succeeds, the ONNX Runtime and WindowsAppSDK ML components are functional.
        Ort::Env env(ORT_LOGGING_LEVEL_WARNING, "DllTest");
        static_cast<void>(env); // Construction is the test; suppress C4189

        const char* message = "WindowsML DLL is working correctly!";
        size_t len = strlen(message) + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result)
        {
            strcpy_s(result, len, message);
        }
        return result;
    }
    catch (const Ort::Exception& e)
    {
        std::string error = "WindowsML DLL test failed: ";
        error += e.what();
        size_t len = error.size() + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result)
        {
            strcpy_s(result, len, error.c_str());
        }
        return result;
    }
    catch (...)
    {
        const char* fallback = "WindowsML DLL test failed: unknown error";
        size_t len = strlen(fallback) + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result)
        {
            strcpy_s(result, len, fallback);
        }
        return result;
    }
}

// Demonstrates the ORT C API (OrtGetApiBase) and CoTaskMemAlloc string management.
// Returns the ONNX Runtime version string.
extern "C" WINDOWSMLWRAPPER_API char* GetOrtVersionString()
{
    try
    {
        // Get ONNX Runtime version from the actual ONNX Runtime API
        const char* version = OrtGetApiBase()->GetVersionString();

        size_t len = strlen(version) + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result)
        {
            strcpy_s(result, len, version);
        }
        return result;
    }
    catch (...)
    {
        // Fallback in case of error
        const char* fallback = "ONNX Runtime (Error getting version)";
        size_t len = strlen(fallback) + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result)
        {
            strcpy_s(result, len, fallback);
        }
        return result;
    }
}

extern "C" WINDOWSMLWRAPPER_API void FreeString(char* str)
{
    if (str)
    {
        CoTaskMemFree(str);
    }
}
