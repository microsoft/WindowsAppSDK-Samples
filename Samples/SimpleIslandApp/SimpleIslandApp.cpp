// SimpleIslandApp.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "SimpleIslandApp.h"

#include <App.xaml.h>
#include <MainPage.h>

namespace winrt
{
    using namespace winrt::Microsoft::UI;
    using namespace winrt::Microsoft::UI::Dispatching;
    using namespace winrt::Microsoft::UI::Xaml;
    using namespace winrt::Microsoft::UI::Xaml::Hosting;
    using namespace winrt::Microsoft::UI::Xaml::Markup;
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

HWND g_button1{ NULL };
HWND g_button2{ NULL };
winrt::DesktopWindowXamlSource g_desktopWindowXamlSource {nullptr};

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    winrt::init_apartment(winrt::apartment_type::single_threaded);

    // We must start a DispatcherQueueController before we can create an island or use Xaml.
    auto dispatcherQueueController {winrt::DispatcherQueueController::CreateOnCurrentThread()};

    // Create our custom Xaml App object.
    auto simpleIslandApp {winrt::make<winrt::SimpleIslandApp::implementation::App>()};

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SIMPLEISLANDAPP, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SIMPLEISLANDAPP));
    // TODO: What should we do with hAccelTable?

    // This takes the place of the message loop, it does the message loop for us.
    dispatcherQueueController.DispatcherQueue().RunEventLoop();

    dispatcherQueueController.ShutdownQueue();

    return 0;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
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

    return RegisterClassExW(&wcex);
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
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
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
    switch (message)
    {
    case WM_CREATE:
        {
            g_button1 = ::CreateWindow(L"BUTTON", L"Win32 Button 1", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 10, 10, 150, 40, hWnd, (HMENU)101, hInst, NULL);
            g_button2 = ::CreateWindow(L"BUTTON", L"Win32 Button 2", WS_TABSTOP | WS_VISIBLE | WS_CHILD, 10, 400, 150, 40, hWnd, (HMENU)102, hInst, NULL);

            // Create our DesktopWindowXamlSource and attach it to our hwnd.  This is our "island".
            g_desktopWindowXamlSource = winrt::DesktopWindowXamlSource{};
            g_desktopWindowXamlSource.Initialize(winrt::GetWindowIdFromWindow(hWnd));

            // Put a new instance of our Xaml "MainPage" into our island.  This is our UI content.
            g_desktopWindowXamlSource.Content(winrt::make<winrt::SimpleIslandApp::implementation::MainPage>());
        }
        break;
    case WM_SIZE:
        {
            const int width = LOWORD(lParam);
            const int height = HIWORD(lParam);

            ::SetWindowPos(g_button1, NULL, 10, 10, 150, 40, SWP_NOZORDER);
            ::SetWindowPos(g_button2, NULL, 10, height - 50, 150, 40, SWP_NOZORDER);

            if (g_desktopWindowXamlSource)
            {
                g_desktopWindowXamlSource.SiteBridge().MoveAndResize({ 10, 60, width - 20, height - 120 });
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
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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
