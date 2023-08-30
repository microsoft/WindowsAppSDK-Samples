// SimpleIslandApp.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "SimpleIslandApp.h"

#include <App.xaml.h>
#include <MainPage.h>
#include <Microsoft.UI.Dispatching.Interop.h> // For ContentPreTranslateMessage

namespace winrt
{
    using namespace winrt::Microsoft::UI;
    using namespace winrt::Microsoft::UI::Dispatching;
    using namespace winrt::Microsoft::UI::Xaml;
    using namespace winrt::Microsoft::UI::Xaml::Hosting;
    using namespace winrt::Microsoft::UI::Xaml::Markup;
}

// Forward declarations of functions included in this code module:
void                MyRegisterClass(HINSTANCE hInstance, const wchar_t* szWindowClass);
void                InitInstance(HINSTANCE, int, const wchar_t* szTitle, const wchar_t* szWindowClass);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    try
    {
        winrt::init_apartment(winrt::apartment_type::single_threaded);

        // We must start a DispatcherQueueController before we can create an island or use Xaml.
        auto dispatcherQueueController{ winrt::DispatcherQueueController::CreateOnCurrentThread() };

        // Create our custom Xaml App object. This is needed to properly use the controls and metadata in Microsoft.ui.xaml.controls.dll.
        auto simpleIslandApp{ winrt::make<winrt::SimpleIslandApp::implementation::App>() };

        // The title bar text
        WCHAR szTitle[100];
        winrt::check_bool(LoadStringW(hInstance, IDS_APP_TITLE, szTitle, ARRAYSIZE(szTitle)) != 0);

        // The main window class name
        WCHAR szWindowClass[100];
        winrt::check_bool(LoadStringW(hInstance, IDC_SIMPLEISLANDAPP, szWindowClass, ARRAYSIZE(szWindowClass)) != 0);

        MyRegisterClass(hInstance, szWindowClass);

        // Perform application initialization:
        InitInstance(hInstance, nCmdShow, szTitle, szWindowClass);

        HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEISLANDAPP));

        MSG msg{};

        // Main message loop:
        while (GetMessage(&msg, nullptr, 0, 0))
        {
            // It's important to call ContentPreTranslateMessage in the event loop so that WinAppSDK can be aware of
            // the messages.  If you don't need to use an accelerator table, you could just call DispatcherQueue.RunEventLoop
            // to do the message pump for you (it will call ContentPreTranslateMessage automatically).
            if (!::ContentPreTranslateMessage(&msg) && !TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        dispatcherQueueController.ShutdownQueue();
    }
    catch (const winrt::hresult_error& exception)
    {
        // An exception was thrown, let's make the exit code the HR value of the exception.
        return exception.code().value;
    }

    return 0;
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

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SIMPLEISLANDAPP));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_SIMPLEISLANDAPP);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

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
void InitInstance(HINSTANCE hInstance, int nCmdShow, const wchar_t* szTitle, const wchar_t* szWindowClass)
{
   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, ::GetModuleHandle(NULL), nullptr);
   winrt::check_bool(hWnd != NULL);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
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
    // Extra state for our window, to keep in GWLP_USERDATA
    struct WindowInfo
    {
        winrt::DesktopWindowXamlSource DesktopWindowXamlSource{ nullptr };
    };

    WindowInfo* windowInfo = reinterpret_cast<WindowInfo*>(::GetWindowLongPtr(hWnd, GWLP_USERDATA));

    switch (message)
    {
    case WM_CREATE:
        {
            windowInfo = new WindowInfo();
            ::SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(windowInfo));

            const HINSTANCE hInst = ::GetModuleHandle(NULL);
            ::CreateWindow(L"BUTTON", L"Win32 Button 1", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 10, 10, 150, 40, hWnd, (HMENU)101, hInst, NULL);
            ::CreateWindow(L"BUTTON", L"Win32 Button 2", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 10, 400, 150, 40, hWnd, (HMENU)102, hInst, NULL);

            // Create our DesktopWindowXamlSource and attach it to our hwnd.  This is our "island".
            windowInfo->DesktopWindowXamlSource = winrt::DesktopWindowXamlSource{};
            windowInfo->DesktopWindowXamlSource.Initialize(winrt::GetWindowIdFromWindow(hWnd));

            // Put a new instance of our Xaml "MainPage" into our island.  This is our UI content.
            windowInfo->DesktopWindowXamlSource.Content(winrt::make<winrt::SimpleIslandApp::implementation::MainPage>());
        }
        break;
    case WM_SIZE:
        {
            const int width = LOWORD(lParam);
            const int height = HIWORD(lParam);

            ::SetWindowPos(::GetDlgItem(hWnd,101), NULL, 10, 10, 150, 40, SWP_NOZORDER);
            ::SetWindowPos(::GetDlgItem(hWnd, 102), NULL, 10, height - 50, 150, 40, SWP_NOZORDER);

            if (windowInfo->DesktopWindowXamlSource)
            {
                windowInfo->DesktopWindowXamlSource.SiteBridge().MoveAndResize({ 10, 60, width - 20, height - 120 });
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
