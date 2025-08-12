#include "WindowsMLWrapper.h"
#include <windows.h>
#include <combaseapi.h>
#include <cstring>

// Include ONNX Runtime headers from WindowsAppSDK.ML package
#include <winml/onnxruntime_cxx_api.h>

extern "C" __declspec(dllexport) char* GetOrtVersionString()
{
    try {
        // Get ONNX Runtime version from the actual ONNX Runtime API
        const char* version = OrtGetApiBase()->GetVersionString();
        
        size_t len = strlen(version) + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result) {
            strcpy_s(result, len, version);
        }
        return result;
    }
    catch (...) {
        // Fallback in case of error
        const char* fallback = "ONNX Runtime (Error getting version)";
        size_t len = strlen(fallback) + 1;
        char* result = static_cast<char*>(CoTaskMemAlloc(len));
        if (result) {
            strcpy_s(result, len, fallback);
        }
        return result;
    }
}

extern "C" __declspec(dllexport) void FreeString(char* str)
{
    if (str) {
        CoTaskMemFree(str);
    }
}
