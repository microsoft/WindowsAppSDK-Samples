// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "framework.h"
#include "CppWinMainInstancing.h"

using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Collections;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Microsoft::Windows::AppLifecycle;
using namespace winrt::Windows::Storage;
using namespace std::chrono_literals;

// Windows App SDK version.
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"" };
const PACKAGE_VERSION minVersion{};

#define MAX_LOADSTRING 100
WCHAR windowTitle[MAX_LOADSTRING];
WCHAR windowClass[MAX_LOADSTRING];
ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
WCHAR exePath[MAX_PATH];
WCHAR exePathAndIconIndex[MAX_PATH + 8];
int activationCount = 1;
HWND g_hWndListbox = NULL;
HINSTANCE g_hInst;
HWND g_hWnd;
std::vector<std::wstring> g_wsOutputStack;

HWND CreateListbox();
bool DecideRedirection();
void ReportLaunchArgs(hstring callLocation, AppActivationArguments args);
void ReportFileArgs(hstring callLocation, AppActivationArguments args);
void RegisterForFileActivation();
void UnregisterForFileActivation();
void GetActivationInfo();
void OnActivated(
    const winrt::Windows::Foundation::IInspectable&, 
    const AppActivationArguments& args);

// Helpers ////////////////////////////////////////////////////////////////////

void OutputMessage(const WCHAR* message)
{
    // If we've already created the listbox, just add the string to it.
    // Otherwise, add the string to a collection which we can flush to the 
    // listbox after it has been created.
    if (g_hWndListbox != NULL)
    {    
        SendMessage(g_hWndListbox, LB_ADDSTRING, 0, (LPARAM)message);
    }
    else
    {
        g_wsOutputStack.push_back(message);
    }
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

void WINAPIV OutputFormattedDebugString(const WCHAR* fmt, ...) 
{
    WCHAR s[1025];
    va_list args;
    va_start(args, fmt);
    wvsprintf(s, fmt, args);
    va_end(args);
    OutputDebugString(s);
}

void OutputErrorString(const WCHAR* message)
{
    WCHAR err[256];
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
    OutputFormattedDebugString(L"%s: %s", message, err);
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

ATOM RegisterWindowClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINICON));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CLASSNAME);
    wcex.lpszClassName = windowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInst = hInstance;
    HWND hWnd = CreateWindowW(windowClass, windowTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);
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

        // Create the listbox, and if there's anything waiting in the output
        // collection, add it to the listbox now.
        g_hWndListbox = CreateListbox();
        if (!g_wsOutputStack.empty())
        {
            for (int i = 0; i < g_wsOutputStack.size(); i++)
            {
                SendMessage(g_hWndListbox, LB_ADDSTRING, 0, (LPARAM)g_wsOutputStack.at(i).c_str());
            }
            g_wsOutputStack.clear();
        }
        break;
    case WM_SIZE:
        width = LOWORD(lParam);
        height = HIWORD(lParam);

        // Move/resize the listbox whenever the user moves/resizes the main window.
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
                RegisterForFileActivation();
                break;
            case IDM_UNREGISTER:
                UnregisterForFileActivation();
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

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize Windows App SDK unpackaged apps.
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        OutputFormattedDebugString(
            L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, 
            minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    if (DecideRedirection())
    {
        return 1;
    }

    // Connect the Activated event, to allow for this instance of the app
    // getting reactivated as a result of multi-instance redirection.
    AppInstance thisInstance = AppInstance::GetCurrent();
    auto activationToken = thisInstance.Activated(
        auto_revoke, [&thisInstance](
            const auto& sender, const AppActivationArguments& args)
        { OnActivated(sender, args); }
    );

    // Carry on with regular Windows initialization.
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

    MddBootstrapShutdown();
    return (int)msg.wParam;
}

///////////////////////////////////////////////////////////////////////////////


// Rich activation ////////////////////////////////////////////////////////////

bool DecideRedirection()
{
    // Get the current executable filesystem path, so we can
    // use it later in registering for activation kinds.
    GetModuleFileName(NULL, exePath, MAX_PATH);
    wcscpy_s(exePathAndIconIndex, exePath);
    wcscat_s(exePathAndIconIndex, L",2");

    // Find out what kind of activation this is.
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        ReportLaunchArgs(L"WinMain", args);
    }
    else if (kind == ExtendedActivationKind::File)
    {
        ReportFileArgs(L"WinMain", args);

        try
        {
            // This is a file activation: here we'll get the file information,
            // and register the file name as our instance key.
            IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
            if (fileArgs)
            {
                IStorageItem file = fileArgs.Files().GetAt(0);
                AppInstance keyInstance = AppInstance::FindOrRegisterForKey(file.Name());
                OutputFormattedMessage(
                    L"Registered key = %ls", keyInstance.Key().c_str());

                // If we successfully registered the file name, we must be the
                // only instance running that was activated for this file.
                if (keyInstance.IsCurrent())
                {
                    // Report successful file name key registration.
                    OutputFormattedMessage(
                        L"IsCurrent=true; registered this instance for %ls",
                        file.Name().c_str());
                }
                else
                {
                    keyInstance.RedirectActivationToAsync(args).get();
                    return true;
                }
            }
        }
        catch (...)
        {
            OutputErrorString(L"Error getting instance information");
        }
    }
    return false;
}

void RegisterForFileActivation()
{
    OutputMessage(L"Registering for file activation");

    // Register one or more supported image filetypes, 
    // an icon (specified by binary file path plus resource index),
    // a display name to use in Shell and Settings,
    // zero or more verbs for the File Explorer context menu,
    // and the path to the EXE to register for activation.
    hstring imageFileTypes[1] = { L".moo" };
    hstring verbs[2] = { L"view", L"edit" };
    ActivationRegistrationManager::RegisterForFileTypeActivation(
        imageFileTypes,
        exePathAndIconIndex,
        L"Montoso File Types",
        verbs,
        exePath
    );
}

void UnregisterForFileActivation()
{
    OutputMessage(L"Unregistering for file activation");

    // Unregister one or more registered filetypes.
    try
    {
        hstring imageFileTypes[1] = { L".moo" };
        ActivationRegistrationManager::UnregisterForFileTypeActivation(
            imageFileTypes,
            exePath
        );
    }
    catch (...)
    {
        OutputMessage(L"Error unregistering file types");
    }
}

void GetActivationInfo()
{
    AppActivationArguments args = AppInstance::GetCurrent().GetActivatedEventArgs();
    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        ILaunchActivatedEventArgs launchArgs = args.Data().as<ILaunchActivatedEventArgs>();
        if (launchArgs)
        {
            OutputMessage(L"Launch activation");
            winrt::hstring argString = launchArgs.Arguments();
            std::vector<std::wstring> argStrings = SplitStrings(argString);
            for (std::wstring const& s : argStrings)
            {
                OutputMessage(s.c_str());
            }
        }
    }
    else if (kind == ExtendedActivationKind::File)
    {
        IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
        if (fileArgs)
        {
            IStorageItem file = fileArgs.Files().GetAt(0);
            OutputFormattedMessage(
                L"File activation: %s", file.Name().c_str());
        }
    }
}

void OnActivated(const IInspectable&, const AppActivationArguments& args)
{
    int const arraysize = 4096;
    WCHAR szTmp[arraysize];
    size_t cbTmp = arraysize * sizeof(WCHAR);
    StringCbPrintf(szTmp, cbTmp, L"OnActivated (%d)", activationCount++);

    ExtendedActivationKind kind = args.Kind();
    if (kind == ExtendedActivationKind::Launch)
    {
        ReportLaunchArgs(szTmp, args);
    }
    else if (kind == ExtendedActivationKind::File)
    {
        ReportFileArgs(szTmp, args);
    }
}

void ReportLaunchArgs(hstring callLocation, AppActivationArguments args)
{
    ILaunchActivatedEventArgs launchArgs = args.Data().as<ILaunchActivatedEventArgs>();
    if (launchArgs)
    {
        winrt::hstring argString = launchArgs.Arguments();
        std::vector<std::wstring> argStrings = SplitStrings(argString);
        OutputFormattedMessage(L"Launch activation: %s", callLocation.c_str());
        for (std::wstring s : argStrings)
        {
            OutputMessage(s.c_str());
        }
    }
}

void ReportFileArgs(hstring callLocation, AppActivationArguments args)
{
    IFileActivatedEventArgs fileArgs = args.Data().as<IFileActivatedEventArgs>();
    if (fileArgs)
    {
        IStorageItem file = fileArgs.Files().GetAt(0);
        OutputFormattedMessage(
            L"File activation, %s, %s", callLocation.c_str(), file.Name().c_str());
    }
}

///////////////////////////////////////////////////////////////////////////////
