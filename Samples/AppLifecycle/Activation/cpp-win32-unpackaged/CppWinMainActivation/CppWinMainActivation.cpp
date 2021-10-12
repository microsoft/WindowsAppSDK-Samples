// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "framework.h"
#include "CppWinMainActivation.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::Storage;

// Windows App SDK version.
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"" };
const PACKAGE_VERSION minVersion{};

#define MAX_LOADSTRING 100
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
WCHAR szExePath[MAX_PATH];
WCHAR szExePathAndIconIndex[MAX_PATH + 8];
HWND g_hWndListbox;
HINSTANCE g_hInst;
HWND g_hWnd;

HWND CreateListbox();

void RegisterForActivation();
void UnregisterForActivation();
void GetActivationInfo();


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

std::vector<std::wstring> split_strings(hstring argString)
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

    // Initialize Windows App SDK for unpackaged apps.
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, 
            minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    // Get the current executable filesystem path, so we can
    // use it later in registering for activation kinds.
    GetModuleFileName(NULL, szExePath, MAX_PATH);
    wcscpy_s(szExePathAndIconIndex, szExePath);
    wcscat_s(szExePathAndIconIndex, L",1");

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLASSNAME, szWindowClass, MAX_LOADSTRING);
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

    // Uninitialize Windows App SDK.
    MddBootstrapShutdown();
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
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   g_hInst = hInstance; 
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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
        NULL,
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
            case IDM_GETARGS:
                GetActivationInfo();
                break;
            case IDM_REGISTER:
                RegisterForActivation();
                break;
            case IDM_UNREGISTER:
                UnregisterForActivation();
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


// Rich activation ////////////////////////////////////////////////////////////

void RegisterForActivation()
{
    OutputMessage(L"Registering for rich activation");

    // Register one or more supported filetypes, specifying 
    // an icon (specified by binary file path plus resource index),
    // a display name to use in Shell and Settings,
    // zero or more verbs for the File Explorer context menu,
    // and the path to the EXE to register for activation.
    hstring myFileTypes[3] = { L".foo", L".foo2", L".foo3" };
    hstring verbs[2] = { L"view", L"edit" };
    ActivationRegistrationManager::RegisterForFileTypeActivation(
        myFileTypes,
        szExePathAndIconIndex,
        L"Contoso File Types",
        verbs,
        szExePath
    );

    // Register a URI scheme for protocol activation,
    // specifying the scheme name, icon, display name and EXE path.
    ActivationRegistrationManager::RegisterForProtocolActivation(
        L"foo",
        szExePathAndIconIndex,
        L"Contoso Foo Protocol",
        szExePath
    );

    // Register for startup activation.
    // As we're registering for startup activation multiple times,
    // and this is a multi-instance app, we'll get multiple instances
    // activated at startup.
    ActivationRegistrationManager::RegisterForStartupActivation(
        L"ContosoStartupId",
        szExePath
    );

    // If we don't specify the EXE, it will default to this EXE.
    ActivationRegistrationManager::RegisterForStartupActivation(
        L"ContosoStartupId2",
        L""
    );
}

void UnregisterForActivation()
{
    OutputMessage(L"Unregistering for rich activation");
    
    // Unregister one or more registered filetypes.
    try
    {
        hstring myFileTypes[3] = { L".foo", L".foo2", L".foo3" };
        ActivationRegistrationManager::UnregisterForFileTypeActivation(
            myFileTypes,
            szExePath
        );
    }
    catch (...)
    {
        OutputMessage(L"Error unregistering file types");
    }

    // Unregister a protocol scheme.
    ActivationRegistrationManager::UnregisterForProtocolActivation(
        L"foo",
        L"");

    // Unregister for startup activation.
    ActivationRegistrationManager::UnregisterForStartupActivation(
        L"ContosoStartupId");
    ActivationRegistrationManager::UnregisterForStartupActivation(
        L"ContosoStartupId2");
}

void GetActivationInfo()
{
    AppActivationArguments args = 
        AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        ILaunchActivatedEventArgs launchArgs = 
            args.Data().as<ILaunchActivatedEventArgs>();
        if (launchArgs != NULL)
        {
            winrt::hstring argString = launchArgs.Arguments().c_str();
            std::vector<std::wstring> argStrings = split_strings(argString);
            OutputMessage(L"Launch activation");
            for (std::wstring s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == ExtendedActivationKind::File)
    {
        IFileActivatedEventArgs fileArgs = 
            args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs != NULL)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
    else if (kind == ExtendedActivationKind::Protocol)
    {
        IProtocolActivatedEventArgs protocolArgs = 
            args.Data().as<IProtocolActivatedEventArgs>();
        if (protocolArgs != NULL)
        {
            Uri uri = protocolArgs.Uri();
            OutputFormattedMessage(
                L"Protocol activation: %s", uri.RawUri().c_str());
        }
    }
    else if (kind == ExtendedActivationKind::StartupTask)
    {
        IStartupTaskActivatedEventArgs startupArgs = 
            args.Data().as<IStartupTaskActivatedEventArgs>();
        if (startupArgs != NULL)
        {
            OutputFormattedMessage(
                L"Startup activation: %s", startupArgs.TaskId().c_str());
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
