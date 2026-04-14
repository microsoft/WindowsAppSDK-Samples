// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

#include <windows.h>
#include <iostream>

// Function pointer types for the DLL exports
typedef char*(__stdcall* GetOrtVersionStringFunc)();
typedef char*(__stdcall* GetTestMessageFunc)();

int main()
{
    HMODULE hDll = LoadLibraryW(L"WindowsMLWrapper.dll");
    if (hDll == nullptr)
    {
        std::wcout << L"Failed to load WindowsMLWrapper.dll" << std::endl;
        return 1;
    }

    // Get function pointers
    GetTestMessageFunc getTestMessage = (GetTestMessageFunc)GetProcAddress(hDll, "GetTestMessage");
    GetOrtVersionStringFunc getOrtVersionString = (GetOrtVersionStringFunc)GetProcAddress(hDll, "GetOrtVersionString");

    if (getTestMessage == nullptr || getOrtVersionString == nullptr)
    {
        std::wcout << L"Failed to get function pointers from DLL" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    try
    {
        // Call GetTestMessage - verifies the ORT runtime initializes correctly
        std::wcout << L"Calling GetTestMessage from DLL..." << std::endl;
        char* testMsg = getTestMessage();
        if (testMsg != nullptr)
        {
            std::wcout << L"Test message: " << testMsg << std::endl;
            CoTaskMemFree(testMsg); // Clean up string allocated by DLL
        }
        else
        {
            std::wcout << L"GetTestMessage returned null" << std::endl;
        }

        // Call GetOrtVersionString - demonstrates ORT C API via DLL
        std::wcout << L"Calling GetOrtVersionString from DLL..." << std::endl;
        char* versionString = getOrtVersionString();
        if (versionString != nullptr)
        {
            std::wcout << L"ONNX Runtime version: " << versionString << std::endl;
            CoTaskMemFree(versionString); // Clean up string allocated by DLL
        }
        else
        {
            std::wcout << L"GetOrtVersionString returned null" << std::endl;
        }
    }
    catch (...)
    {
        std::wcout << L"Exception occurred while calling DLL function" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    std::wcout << L"DLL functions called successfully!" << std::endl;
    FreeLibrary(hDll);
    return 0;
}
