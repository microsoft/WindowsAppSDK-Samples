// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "framework.h"
#include "CppWinMainEnv.h"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Microsoft::Windows::System;

#define MAX_LOADSTRING 256
WCHAR windowTitle[MAX_LOADSTRING];
WCHAR windowClass[MAX_LOADSTRING];
HINSTANCE g_hInst;
HWND g_hWnd;
HWND g_hWndListbox;
BOOL g_bWorkInProgress;

void GetEnvironmentVariables();
void AddEnvironmentVariables();
void RemoveEnvironmentVariables();
void AddToPath();
void RemoveFromPath();
void AddToPathExt();
void RemoveFromPathExt();

ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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

void OutputErrorString(const WCHAR* message)
{
    WCHAR err[256];
    DWORD errorCode = GetLastError();
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), err, 255, NULL);
    OutputFormattedMessage(L"%s: %s", message, err);
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
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int) msg.wParam;
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
            case IDM_GETVARS:
                GetEnvironmentVariables();
                break;
			case IDM_ADDVARS:
				AddEnvironmentVariables();
				break;
			case IDM_REMOVEVARS:
				RemoveEnvironmentVariables();
				break;
			case IDM_ADDTOPATH:
				AddToPath();
				break;
			case IDM_REMOVEFROMPATH:
				RemoveFromPath();
				break;
			case IDM_ADDTOPATHEXT:
				AddToPathExt();
				break;
			case IDM_REMOVEFROMPATHEXT:
				RemoveFromPathExt();
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


// Manipulate environment variables ///////////////////////////////////////////

void GetEnvironmentVariables()
{
	OutputMessage(L"\nGetEnvironmentVariables.......");

	// The EnvironmentManager APIs are only supported from OS builds 19044 onwards.
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// Get environment variables for the current user.
			auto userManager = EnvironmentManager::GetForUser();
			auto TEMP = userManager.GetEnvironmentVariable(L"TEMP");
			OutputFormattedMessage(L"TEMP=%s", TEMP.c_str());

			// Get system-wide environment variables.
			auto systemManager = EnvironmentManager::GetForMachine();
			auto ComSpec = systemManager.GetEnvironmentVariable(L"ComSpec");
			OutputFormattedMessage(L"ComSpec=%s", ComSpec.c_str());

			// Get all environment variables at a particular level.
			auto processManager = EnvironmentManager::GetForProcess();
			auto envVars = processManager.GetEnvironmentVariables();
			for (auto envVar : envVars)
			{
				OutputFormattedMessage(
					L"%s=%s", envVar.Key().c_str(), envVar.Value().c_str());
			}
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error getting environment variables: %s", message.c_str());
		}
	}
}

void AddEnvironmentVariables()
{
	OutputMessage(L"\nAddEnvironmentVariables.......");
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// Environment variables set at the process level will persist
			// only until the process terminates.
			auto processManager = EnvironmentManager::GetForProcess();
			processManager.SetEnvironmentVariable(L"MAXITEMS", L"1024");
			auto maxItemsValue = processManager.GetEnvironmentVariable(L"MAXITEMS");
			OutputFormattedMessage(L"MAXITEMS=%s", maxItemsValue.c_str());

			// For packaged apps, environment variables set at the user 
			// or machine level will persist until the app is uninstalled.
			// For unpackaged apps, they are not removed on app uninstall.
			auto userManager = EnvironmentManager::GetForUser();
			userManager.SetEnvironmentVariable(L"HOMEDIR", L"D:\\Foo");
			auto homeDirValue = userManager.GetEnvironmentVariable(L"HOMEDIR");
			OutputFormattedMessage(L"HOMEDIR=%s", homeDirValue.c_str());

			// To set environment variables at machine level, the app must
			// be running elevated, otherwise this will result in Access Denied.
			auto systemManager = EnvironmentManager::GetForMachine();
			systemManager.SetEnvironmentVariable(L"Fruit", L"Banana");
			auto fruitValue = systemManager.GetEnvironmentVariable(L"Fruit");
			OutputFormattedMessage(L"Fruit=%s", fruitValue.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error setting environment variables: %s", message.c_str());
		}
	}
}

void RemoveEnvironmentVariables()
{
	OutputMessage(L"\nRemoveEnvironmentVariables....");
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// Environment variables set at the process level will persist
			// only until the process terminates.
			auto processManager = EnvironmentManager::GetForProcess();
			processManager.SetEnvironmentVariable(L"MAXITEMS", L"");
			auto maxItemsValue = processManager.GetEnvironmentVariable(L"MAXITEMS");
			if (maxItemsValue == L"")
			{
				OutputMessage(L"MAXITEMS not found");
			}
			else
			{
				OutputFormattedMessage(L"MAXITEMS=%s", maxItemsValue.c_str());
			}

			// For packaged apps, environment variables set at the user 
			// or machine level will persist until the app is uninstalled.
			// For unpackaged apps, they are not removed on app uninstall.
			auto userManager = EnvironmentManager::GetForUser();
			userManager.SetEnvironmentVariable(L"HOMEDIR", L"");
			auto homeDirValue = userManager.GetEnvironmentVariable(L"HOMEDIR");
			if (maxItemsValue == L"")
			{
				OutputMessage(L"HOMEDIR not found");
			}
			else
			{
				OutputFormattedMessage(L"HOMEDIR=%s", homeDirValue.c_str());
			}

			// To remove environment variables at machine level, the app must
			// be running elevated, otherwise this will result in Access Denied.
			auto systemManager = EnvironmentManager::GetForMachine();
			systemManager.SetEnvironmentVariable(L"Fruit", L"");
			auto fruitValue = systemManager.GetEnvironmentVariable(L"Fruit");
			if (maxItemsValue == L"")
			{
				OutputMessage(L"Fruit not found");
			}
			else
			{
				OutputFormattedMessage(L"Fruit=%s", fruitValue.c_str());
			}
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error removing environment variables: %s", message.c_str());
		}
	}
}

void AddToPath()
{
	OutputMessage(L"\nAddToPath.....................");
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// You can append paths to the end of the PATH variable, at
			// process, user or machine scope.
			auto userManager = EnvironmentManager::GetForUser();
			userManager.AppendToPath(L"%USERPROFILE%\\ContosoBin");
			auto path = userManager.GetEnvironmentVariable(L"PATH");
			OutputFormattedMessage(L"PATH=%s", path.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error adding to PATH: %s", message.c_str());
		}
	}
}

void RemoveFromPath()
{
	OutputMessage(L"\nRemoveFromPath................");
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// You can remove paths from the PATH variable that you
			// previously appended.
			// Removal is done with case-insensitive ordinal string comparison,
			// without expanding the %USERPROFILE% environment variable.
			auto userManager = EnvironmentManager::GetForUser();
			userManager.RemoveFromPath(L"%USERPROFILE%\\ContosoBin");
			auto path = userManager.GetEnvironmentVariable(L"PATH");
			OutputFormattedMessage(L"PATH=%s", path.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error removing from PATH: %s", message.c_str());
		}
	}
}

void AddToPathExt()
{
	OutputMessage(L"\nAddToPathExt..................");
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// You can append extensions to the PATHEXT variable
			auto userManager = EnvironmentManager::GetForUser();
			userManager.AddExecutableFileExtension(L".XYZ");
			auto pathExt = userManager.GetEnvironmentVariable(L"PATHEXT");
			OutputFormattedMessage(L"PATHEXT=%s", pathExt.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error adding to PATHEXT: %s", message.c_str());
		}
	}
}

void RemoveFromPathExt()
{
	OutputMessage(L"\nRemoveFromPathExt.............");
	if (EnvironmentManager::IsSupported())
	{
		try
		{
			// You can remove any extensions that you previously 
			// added to the PATHEXT variable.
			auto userManager = EnvironmentManager::GetForUser();
			userManager.RemoveExecutableFileExtension(L".XYZ");
			auto pathExt = userManager.GetEnvironmentVariable(L"PATHEXT");
			OutputFormattedMessage(L"PATHEXT=%s", pathExt.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(
				L"Error removing from PATHEXT: %s", message.c_str());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////



