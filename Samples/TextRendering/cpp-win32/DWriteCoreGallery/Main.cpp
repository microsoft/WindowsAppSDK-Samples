// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "Main.h"
#include "MainWindow.h"
#include "Resource.h"

//#define USE_INBOX_DWRITE

HINSTANCE g_hInstance;
wil::com_ptr<IDWriteFactory7> g_factory;

_Field_z_ WCHAR g_appTitle[80];
_Ret_z_ WCHAR const* GetAppTitle()
{
    return g_appTitle;
}

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
)
{
    // Initialize the global instance variable and app title.
    g_hInstance = hInstance;
    LoadStringW(hInstance, IDS_APP_NAME, g_appTitle, ARRAYSIZE(g_appTitle));

#ifndef USE_INBOX_DWRITE
    // Initialize the global factory object.
    THROW_IF_FAILED(DWriteCoreCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown**>(&g_factory)));
#else
    auto moduleHandle = LoadLibraryW(L"DWrite.dll");
    THROW_LAST_ERROR_IF_NULL(moduleHandle);
    auto createFactoryProc = reinterpret_cast<decltype(DWriteCreateFactory)*>(GetProcAddress(moduleHandle, "DWriteCreateFactory"));
    THROW_LAST_ERROR_IF_NULL(createFactoryProc);
    THROW_IF_FAILED(createFactoryProc(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown**>(&g_factory)));
#endif

    // Create the main window.
    MainWindow mainWindow{ nCmdShow };

    // Process messages until the main window is destroyed.
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

