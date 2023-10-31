// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// NOTES. This app is cloned from the unpackaged version. The key differences are as follows:
// 1. A packaged app cannot use the Register/Unregister APIs for rich activation; instead it declares activation kinds in its manifest.
// 2. A packaged app does not need to initialize the Windows App SDK for unpackaged support.
// 3. The Package project must include a reference to the Windows App SDK NuGet in addition to the app project itself.
// 4. A packaged app can declare rich activation extensions in the manifest, and retrieve activation arguments via the Windows App SDK GetActivatedEventArgs API.
// 5. A packaged app can use the UWP GetActivatedEventArgs API instead of the Windows App SDK version.

#include "framework.h"
#include "CppWinMainActivation.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::Storage;

#define MAX_LOADSTRING 100
WCHAR windowTitle[MAX_LOADSTRING];
WCHAR windowClass[MAX_LOADSTRING];
ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND g_hWndListbox;
HINSTANCE g_hInst;
HWND g_hWnd;

HWND CreateListbox();
void GetActivationInfoWas();
void GetActivationInfoUwp();


// Helpers ////////////////////////////////////////////////////////////////////

void OutputMessage(const WCHAR* message)
{
    SendMessage(g_hWndListbox, LB_ADDSTRING, 0, (LPARAM)message);
}

void OutputFormattedMessage(const WCHAR* fmt, ...)
{
    WCHAR message[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(message, fmt, args);
    va_end(args);
    OutputMessage(message);
}

std::vector<std::wstring> SplitStrings(hstring argString)
{
    std::vector<std::wstring> argStrings;
    std::wistringstream iss(argString.c_str());
    for (std::wstring s; iss >> s; )
    {
        argStrings.push_back(s);
    }
    return argStrings;
}

///////////////////////////////////////////////////////////////////////////////


// WinMain/WndProc ////////////////////////////////////////////////////////////

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, windowTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLASSNAME, windowClass, MAX_LOADSTRING);
    RegisterWindowClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

ATOM RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CLASSNAME);
    wcex.lpszClassName  = windowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance; 
   HWND hWnd = CreateWindowW(windowClass, windowTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 640,480, nullptr, nullptr, hInstance, nullptr);
   if (!hWnd)
   {
      return FALSE;
   }
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   return TRUE;
}

HWND CreateListbox()
{
    DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_VSCROLL | LBS_DISABLENOSCROLL | LBS_NOTIFY;
    return CreateWindowEx(WS_EX_CLIENTEDGE, L"listbox",
        nullptr,
        dwStyle,
        0, 0, 0, 0,
        g_hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_LISTBOX)),
        g_hInst, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    UINT width = 0;
    UINT height = 0;
    switch (message)
    {
    case WM_CREATE:
        g_hWnd = hWnd;
        g_hWndListbox = CreateListbox();
        break;
    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);
        MoveWindow(g_hWndListbox, 10, 10, width - 20, height - 20, true);
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_GETARGSWAS:
                GetActivationInfoWas();
                break;
            case IDM_GETARGSUWP:
                GetActivationInfoUwp();
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

///////////////////////////////////////////////////////////////////////////////


// Use the Windows App SDK version of AppInstance::GetActivatedEventArgs.
void GetActivationInfoWas()
{
    namespace WindowsAppSdk = winrt::Microsoft::Windows::AppLifecycle;

    // The Windows App SDK GetActivatedEventArgs returns an AppActivationArguments
    // object, which exposes the specific activation type via its Data property.
    WindowsAppSdk::AppActivationArguments args = 
        WindowsAppSdk::AppInstance::GetCurrent().GetActivatedEventArgs();

    // The Windows App SDK includes the ExtendedActivationKind enum, which
    // includes and extends the UWP ActivationKind enum.
    WindowsAppSdk::ExtendedActivationKind kind = args.Kind();
    if (kind == WindowsAppSdk::ExtendedActivationKind::Launch)
    {
        ILaunchActivatedEventArgs launchArgs =
            args.Data().as<ILaunchActivatedEventArgs>();
        if (launchArgs)
        {
            winrt::hstring argString = launchArgs.Arguments();
            std::vector<std::wstring> argStrings = SplitStrings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring const& s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == WindowsAppSdk::ExtendedActivationKind::File)
    {
        IFileActivatedEventArgs fileArgs =
            args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
    else if (kind == WindowsAppSdk::ExtendedActivationKind::Protocol)
    {
        IProtocolActivatedEventArgs protocolArgs =
            args.Data().as<IProtocolActivatedEventArgs>();
        if (protocolArgs)
        {
            Uri uri = protocolArgs.Uri();
            OutputFormattedMessage(
                L"Protocol activation: %s", uri.RawUri().c_str());
        }
    }
    else if (kind == WindowsAppSdk::ExtendedActivationKind::StartupTask)
    {
        IStartupTaskActivatedEventArgs startupArgs =
            args.Data().as<IStartupTaskActivatedEventArgs>();
        if (startupArgs)
        {
            OutputFormattedMessage(
                L"Startup activation: %s", startupArgs.TaskId().c_str());
        }
    }
}

// Use the UWP version of AppInstance::GetActivatedEventArgs.
void GetActivationInfoUwp()
{
    // The UWP GetActivatedEventArgs returns an IActivatedEventArgs,
    // which can be directly cast to the specific activation type it represents.
    IActivatedEventArgs args = 
        winrt::Windows::ApplicationModel::AppInstance::GetActivatedEventArgs();
    ActivationKind kind = args.Kind();
    if (kind == ActivationKind::Launch)
    {
        ILaunchActivatedEventArgs launchArgs = 
            args.as<ILaunchActivatedEventArgs>();
        if (launchArgs)
        {
            winrt::hstring argString = launchArgs.Arguments();
            std::vector<std::wstring> argStrings = SplitStrings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring const& s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == ActivationKind::File)
    {
        IFileActivatedEventArgs fileArgs =
            args.as<IFileActivatedEventArgs>();
        if (fileArgs)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
    else if (kind == ActivationKind::Protocol)
    {
        IProtocolActivatedEventArgs protocolArgs =
            args.as<IProtocolActivatedEventArgs>();
        if (protocolArgs)
        {
            Uri uri = protocolArgs.Uri();
            OutputFormattedMessage(
                L"Protocol activation: %s", uri.RawUri().c_str());
        }
    }
    else if (kind == ActivationKind::StartupTask)
    {
        IStartupTaskActivatedEventArgs startupArgs =
            args.as<IStartupTaskActivatedEventArgs>();
        if (startupArgs)
        {
            OutputFormattedMessage(
                L"Startup activation: %s", startupArgs.TaskId().c_str());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
