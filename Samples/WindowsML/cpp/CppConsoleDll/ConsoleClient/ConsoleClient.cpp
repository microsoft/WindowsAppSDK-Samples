// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

#include <windows.h>
#include <iostream>

// Function pointer type for the DLL exports
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
    GetOrtVersionStringFunc getOrtVersionString = (GetOrtVersionStringFunc)GetProcAddress(hDll, "GetOrtVersionString");
    if (getOrtVersionString == nullptr)
    {
        std::wcout << L"Failed to get GetOrtVersionString function from DLL" << std::endl;
        FreeLibrary(hDll);
        return 1;
    }

    try
    {
        // Call ONNX Runtime version function
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

    FreeLibrary(hDll);
    return 0;
}
