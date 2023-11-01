// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

// XamlIslandApp.cpp : Defines the entry point for the application.

#include "pch.h"
#include "XamlIslandApp.h"

#include <App.xaml.h>
#include <MainPage.h>
#include <Microsoft.UI.Dispatching.Interop.h> // For ContentPreTranslateMessage

namespace winrt
{
    using namespace winrt::Microsoft::UI;
    using namespace winrt::Microsoft::UI::Content;
    using namespace winrt::Microsoft::UI::Dispatching;
    using namespace winrt::Microsoft::UI::Input;
    using namespace winrt::Microsoft::UI::Xaml;
    using namespace winrt::Microsoft::UI::Xaml::Controls;
    using namespace winrt::Microsoft::UI::Xaml::Hosting;
    using namespace winrt::Microsoft::UI::Xaml::Markup;
}

// Forward declarations of functions included in this code module:
void                MyRegisterClass(HINSTANCE hInstance, const wchar_t* szWindowClass);
HWND                InitInstance(HINSTANCE, int, const wchar_t* szTitle, const wchar_t* szWindowClass);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
bool                ProcessMessageForTabNavigation(const HWND topLevelWindow, MSG* msg);
void                NavigateFromIsland(const HWND topLevelWindow, winrt::FocusNavigationReason reason);
winrt::StackPanel   CreateXamlContent();

// Extra state for our top-level window, we point to from GWLP_USERDATA.
struct WindowInfo
{
    winrt::XamlIsland XamlIsland{ nullptr };
    winrt::DesktopChildSiteBridge SiteBridge{ nullptr };

    winrt::InputFocusNavigationHost FocusNavigationHost{ nullptr };
    HWND LastFocusedWindow{ NULL };
};

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try
    {
        // Island-support: Call init_apartment to initialize COM and WinRT for the thread.
        winrt::init_apartment(winrt::apartment_type::single_threaded);

        // Island-support: We must start a DispatcherQueueController before we can create an island or use Xaml.
        auto dispatcherQueueController{ winrt::DispatcherQueueController::CreateOnCurrentThread() };

        // Island-support: Create our custom Xaml App object. This is needed to properly use the controls and metadata
        // in Microsoft.ui.xaml.controls.dll.
        auto XamlIslandApp{ winrt::make<winrt::XamlIslandApp::implementation::App>() };

        // The title bar text
        WCHAR szTitle[100];
        winrt::check_bool(LoadStringW(hInstance, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle)) != 0);

        // The main window class name
        WCHAR szWindowClass[100];
        winrt::check_bool(LoadStringW(hInstance, IDC_XamlIslandApp, szWindowClass, ARRAYSIZE(szWindowClass)) != 0);

        MyRegisterClass(hInstance, szWindowClass);

        // Perform application initialization:
        HWND topLevelWindow = InitInstance(hInstance, nCmdShow, szTitle, szWindowClass);

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_XamlIslandApp));

        MSG msg{};

        // Main message loop:
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            // Island-support: It's important to call ContentPreTranslateMessage in the event loop so that WinAppSDK can be aware of
            // the messages.  If you don't need to use an accelerator table, you could just call DispatcherQueue.RunEventLoop
            // to do the message pump for you (it will call ContentPreTranslateMessage automatically).
            if (::ContentPreTranslateMessage(&msg))
            {
                continue;
            }

            if (TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                continue;
            }

            // Island-support: This is needed so that the user can correctly tab and shift+tab into islands.
            if (ProcessMessageForTabNavigation(topLevelWindow, &msg))
            {
                continue;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Island-support: To properly shut down after using a DispatcherQueue, call ShutdownQueue[Aysnc]().
        dispatcherQueueController.ShutdownQueue();
    }
    catch (const winrt::hresult_error& exception)
    {
        // An exception was thrown, let's make the exit code the HR value of the exception.
        return exception.code().value;
    }

    return 0;
}

// Returns "true" if the function handled the message and it shouldn't be processed any further.
// Intended to be called from the main message loop.
bool ProcessMessageForTabNavigation(const HWND topLevelWindow, MSG* msg)
{
    if (msg->message == WM_KEYDOWN && msg->wParam == VK_TAB)
    {
        // The user is pressing the "tab" key.  We want to handle this ourselves so we can pass information into Xaml
        // about the tab navigation.  Specifically, we need to tell Xaml whether this is a forward tab, or a backward
        // shift+tab, so Xaml will know whether to put focus on the first Xaml element in the island or the last
        // Xaml element.  (This is done in the call to FocusNavigationHost.NavigateFocus()).
        const HWND currentFocusedWindow = ::GetFocus();
        if (::GetAncestor(currentFocusedWindow, GA_ROOT) != topLevelWindow)
        {
            // This is a window outside of our top-level window, let the system process it.
            return false;
        }

        const bool isShiftKeyDown = ((HIWORD(::GetKeyState(VK_SHIFT)) & 0x8000) != 0);
        const HWND nextFocusedWindow = ::GetNextDlgTabItem(topLevelWindow, currentFocusedWindow, isShiftKeyDown /*bPrevious*/);

        WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(topLevelWindow, GWLP_USERDATA));
        const HWND xamlIslandWindow = winrt::GetWindowFromWindowId(windowInfo->SiteBridge.WindowId());
        if (xamlIslandWindow == nextFocusedWindow)
        {
            // Focus is moving to our XamlIsland.  Instead of just calling SetFocus on it, we call NavigateFocus(),
            // which allows us to tell Xaml which direction the keyboard focus is moving.
            // If your app has multiple XamlIslands in the window, you'll want to loop over them and check to
            // see if focus is moving to each one.
            winrt::FocusNavigationReason request{
                isShiftKeyDown ?
                    winrt::FocusNavigationReason::Right :
                    winrt::FocusNavigationReason::Left };

            windowInfo->FocusNavigationHost.NavigateFocus(request);
        }

        // Focus isn't moving to our XamlIsland.  IsDialogMessage will automatically do the tab navigation
        // for us for this msg.
        const bool handled = (::IsDialogMessage(topLevelWindow, msg) == TRUE);
        return handled;
    }
    return false;
}

// Focus is leaving the XamlIsland, check which element to shift focus to.
void NavigateFromIsland(const HWND topLevelWindow, winrt::FocusNavigationReason reason)
{
    if (reason == winrt::FocusNavigationReason::First ||
        reason == winrt::FocusNavigationReason::Down ||
        reason == winrt::FocusNavigationReason::Right)
    {
        ::SetFocus(::GetDlgItem(topLevelWindow, 502));
    }
    else
    {
        ::SetFocus(::GetDlgItem(topLevelWindow, 501));
    }
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
void MyRegisterClass(HINSTANCE hInstance, const wchar_t* szWindowClass)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_XamlIslandApp));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_XamlIslandApp);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    winrt::check_bool(RegisterClassExW(&wcex) != 0);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE /*hInstance*/, int nCmdShow, const wchar_t* szTitle, const wchar_t* szWindowClass)
{
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, ::GetModuleHandle(NULL), nullptr);
    winrt::check_bool(hWnd != NULL);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    return hWnd;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
    {
        windowInfo = new WindowInfo();
        ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowInfo));

        const HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
        ::CreateWindow(L"BUTTON", L"Win32 Button 1", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 10, 10, 150, 40, hWnd, (HMENU)501, hInst, NULL);

        // Create our XamlIsland and retrieve its ContentIsland.  Then connect it with a bridge.
        windowInfo->XamlIsland = winrt::XamlIsland{};
        auto contentIsland = windowInfo->XamlIsland.ContentIsland();

        windowInfo->SiteBridge = winrt::DesktopChildSiteBridge::Create(contentIsland.Compositor(), winrt::GetWindowIdFromWindow(hWnd));
        windowInfo->SiteBridge.Connect(contentIsland);

        // Enable the DesktopChildSiteBridge to be a tab stop.
        ::SetWindowLong(
            winrt::GetWindowFromWindowId(windowInfo->SiteBridge.WindowId()),
            GWL_STYLE,
            WS_TABSTOP | WS_CHILD | WS_VISIBLE);

        // Put a new instance of our Xaml Content into our island.  This is our UI content.
        // TODO: Once input and layout work properly with the XamlIsland, put an instance of the MainPage instead.
        // Currently, the layout required by a Page object doesn't work without an HWND.

        windowInfo->XamlIsland.Content(CreateXamlContent());

        ::CreateWindow(L"BUTTON", L"Win32 Button 2", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 10, 400, 150, 40, hWnd, (HMENU)502, hInst, NULL);

        // Save the SiteBridge's InputFocusNavigationHost, and handle focus leaving the island with the
        // DepartFocusRequested event.
        windowInfo->FocusNavigationHost = winrt::InputFocusNavigationHost::GetForSiteBridge(windowInfo->SiteBridge);
        windowInfo->FocusNavigationHost.DepartFocusRequested(
            [hWnd](winrt::InputFocusNavigationHost const&, winrt::FocusNavigationRequestEventArgs const& args) {
                NavigateFromIsland(hWnd, args.Reason());
            });
    }
    break;
    case WM_SIZE:
    {
        const int width = LOWORD(lParam);
        const int height = HIWORD(lParam);

        ::SetWindowPos(::GetDlgItem(hWnd, 501), NULL, 10, 10, 150, 40, SWP_NOZORDER);
        ::SetWindowPos(::GetDlgItem(hWnd, 502), NULL, 10, height - 50, 150, 40, SWP_NOZORDER);

        if (windowInfo->SiteBridge)
        {
            windowInfo->SiteBridge.MoveAndResize({ 10, 60, width - 20, height - 120 });
        }
    }
    break;
    case WM_ACTIVATE:
    {
        // Make focus work nicely when the user presses alt+tab to activate a different window, and then alt+tab
        // again to come back to this window.  We want the focus to go back to the same child HWND that was focused
        // before.
        const bool isGettingDeactivated = (LOWORD(wParam) == WA_INACTIVE);
        if (isGettingDeactivated)
        {
            // Remember the HWND that had focus.
            windowInfo->LastFocusedWindow = ::GetFocus();
        }
        else if (windowInfo->LastFocusedWindow != NULL)
        {
            ::SetFocus(windowInfo->LastFocusedWindow);
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(::GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        UNREFERENCED_PARAMETER(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_NCDESTROY:
        delete windowInfo;
        ::SetWindowLong(hWnd, GWLP_USERDATA, NULL);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

winrt::StackPanel CreateXamlContent()
{
    auto textBox = winrt::TextBox();
    textBox.Text(L"Text Edit");

    auto button1 = winrt::Button();
    button1.Content(winrt::box_value(L"Test Button 1"));

    auto button2 = winrt::Button();
    button2.Content(winrt::box_value(L"Test Button 2"));

    auto stackPanel = winrt::StackPanel();
    stackPanel.Spacing(10);
    stackPanel.Margin(winrt::ThicknessHelper::FromUniformLength(5.0));

    stackPanel.Children().Append(textBox);
    stackPanel.Children().Append(button1);
    stackPanel.Children().Append(button2);

    return stackPanel;
}
