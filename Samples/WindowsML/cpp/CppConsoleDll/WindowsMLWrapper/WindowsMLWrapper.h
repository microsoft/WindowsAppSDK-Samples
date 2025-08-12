#pragma once

#include <windows.h>

#ifdef WINDOWSMLWRAPPER_EXPORTS
#define WINDOWSMLWRAPPER_API __declspec(dllexport)
#else
#define WINDOWSMLWRAPPER_API __declspec(dllimport)
#endif

extern "C" {
    // Returns the ONNX Runtime version string
    // Caller is responsible for freeing the returned string using CoTaskMemFree
    WINDOWSMLWRAPPER_API char* GetOrtVersionString();
}
