// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "framework.h"
#include "CppWinMainState.h"

#include <stdio.h>
#include <vector>
#include <functional>
#include <iostream>
#include <SDKDDKVer.h>
#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
using namespace std;

using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::System::Diagnostics;
using namespace winrt::Windows::System;
using namespace winrt::Microsoft::Windows::System::Power;

// WASDK version.
const UINT32 majorMinorVersion{ 0x00010000 };
PCWSTR versionTag{ L"preview1" };
const PACKAGE_VERSION minVersion{};

#define MAX_LOADSTRING 256
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HINSTANCE g_hInst; 
HWND g_hWnd;
HWND g_hWndListbox;

BOOL bWorkInProgress;
winrt::event_token batteryToken;
winrt::event_token powerToken;
winrt::event_token powerSourceToken;
winrt::event_token chargeToken;
winrt::event_token dischargeToken;

ATOM RegisterWindowClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void RegisterPowerManagerCallbacks();
void UnregisterPowerManagerCallbacks();
void DetermineWorkloads();
void PauseNonCriticalWork();
void StartPowerIntensiveWork();
void StopUpdatingGraphics();
void StartDoingBackgroundWork();

void OnBatteryStatusChanged();
void OnPowerSupplyStatusChanged();
void OnPowerSourceKindChanged();
void OnDisplayStatusChanged();
void OnRemainingChargePercentChanged();
void OnRemainingDischargeTimeChanged();


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

///////////////////////////////////////////////////////////////////////////////


// WinMain/WndProc ////////////////////////////////////////////////////////////

int APIENTRY wWinMain(
    _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize WASDK for unpackaged apps.
    HRESULT hr{ MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion) };
    if (FAILED(hr))
    {
        wprintf(L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
            hr, majorMinorVersion, versionTag, minVersion.Major, 
            minVersion.Minor, minVersion.Build, minVersion.Revision);
        return hr;
    }

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLASSNAME, szWindowClass, MAX_LOADSTRING);
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

    // Uninitialize WASDK.
    MddBootstrapShutdown();
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
        0,0,0,0,
        g_hWnd,
        reinterpret_cast<HMENU>(static_cast<INT_PTR>(IDC_LISTBOX)),
        g_hInst, 0);
}

LRESULT CALLBACK WndProc(
    HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
            case IDM_REGISTER:
                RegisterPowerManagerCallbacks();
                break;
            case IDM_UNREGISTER:
                UnregisterPowerManagerCallbacks();
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


// Register/Unregister state/power notification callbacks /////////////////////

void RegisterPowerManagerCallbacks()
{
    batteryToken = PowerManager::BatteryStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnBatteryStatusChanged(); });
    powerToken = PowerManager::PowerSupplyStatusChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnPowerSupplyStatusChanged(); });
    powerSourceToken = PowerManager::PowerSourceKindChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnPowerSourceKindChanged(); });
    chargeToken = PowerManager::RemainingChargePercentChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnRemainingChargePercentChanged(); });
    dischargeToken = PowerManager::RemainingDischargeTimeChanged([&](
        const auto&, winrt::Windows::Foundation::IInspectable obj) { OnRemainingDischargeTimeChanged(); });

    if (batteryToken && powerToken && powerSourceToken && chargeToken && dischargeToken)
    {
        OutputMessage(L"Successfully registered for state notifications");
    }
    else
    {
        OutputMessage(L"Failed to register for state notifications");
    }
}

void UnregisterPowerManagerCallbacks()
{
    OutputMessage(L"Unregistering state notifications");
    PowerManager::BatteryStatusChanged(batteryToken);
    PowerManager::PowerSupplyStatusChanged(powerToken);
    PowerManager::PowerSourceKindChanged(powerSourceToken);
    PowerManager::RemainingChargePercentChanged(chargeToken);
    PowerManager::RemainingDischargeTimeChanged(dischargeToken);
}

///////////////////////////////////////////////////////////////////////////////


// State/power notification callbacks /////////////////////////////////////////

void OnBatteryStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    BatteryStatus batteryStatus = PowerManager::BatteryStatus();
    int remainingCharge = PowerManager::RemainingChargePercent();
    switch (batteryStatus)
    {
    case BatteryStatus::Charging:
        wcscpy_s(szStatus, L"Charging");
        break;
    case BatteryStatus::Discharging:
        wcscpy_s(szStatus, L"Discharging");
        break;
    case BatteryStatus::Idle:
        wcscpy_s(szStatus, L"Idle");
        break;
    case BatteryStatus::NotPresent:
        wcscpy_s(szStatus, L"NotPresent");
        break;
    }

    OutputFormattedMessage(
        L"Battery status changed: %s, %d%% remaining", 
        szStatus, remainingCharge);
    DetermineWorkloads();
}

void OnPowerSupplyStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    PowerSupplyStatus powerStatus = PowerManager::PowerSupplyStatus();
    switch (powerStatus)
    {
    case PowerSupplyStatus::Adequate:
        wcscpy_s(szStatus, L"Adequate");
        break;
    case PowerSupplyStatus::Inadequate:
        wcscpy_s(szStatus, L"Inadequate");
        break;
    case PowerSupplyStatus::NotPresent:
        wcscpy_s(szStatus, L"NotPresent");
        break;
    }

    OutputFormattedMessage(
        L"Power supply status changed: %s", szStatus);
    DetermineWorkloads();
}

void OnPowerSourceKindChanged()
{
    const size_t statusSize = 16;
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    PowerSourceKind powerSource = PowerManager::PowerSourceKind();
    switch (powerSource)
    {
    case PowerSourceKind::AC:
        wcscpy_s(szStatus, L"AC");
        break;
    case PowerSourceKind::DC:
        wcscpy_s(szStatus, L"DC");
        break;
    }

    OutputFormattedMessage(
        L"Power source kind changed: %s", szStatus);
    DetermineWorkloads();
}

void OnRemainingChargePercentChanged() 
{
    OutputMessage(L"Remaining charge percent changed");
    DetermineWorkloads();
}

void OnRemainingDischargeTimeChanged() 
{
    OutputMessage(L"Remaining discharge time changed");
    DetermineWorkloads();
}

void OnDisplayStatusChanged()
{
    const size_t statusSize = 16;
    WCHAR szStatus[statusSize];
    wmemset(&(szStatus[0]), 0, statusSize);

    DisplayStatus displayStatus = PowerManager::DisplayStatus();
    switch (displayStatus)
    {
    case DisplayStatus::Dimmed:
        wcscpy_s(szStatus, L"Dimmed");
        break;
    case DisplayStatus::Off:
        wcscpy_s(szStatus, L"Off");
        break;
    case DisplayStatus::On:
        wcscpy_s(szStatus, L"On");
        break;
    }

    OutputFormattedMessage(
        L"Display status changed: %s", szStatus);
    if (displayStatus == DisplayStatus::Off)
    {
        // The screen is off, let's stop rendering foreground graphics,
        // and instead kick off some background work now.
        StopUpdatingGraphics();
        StartDoingBackgroundWork();
    }
}

void DetermineWorkloads()
{
    BatteryStatus batteryStatus = PowerManager::BatteryStatus();
    int remainingCharge = PowerManager::RemainingChargePercent();
    PowerSupplyStatus powerStatus = PowerManager::PowerSupplyStatus();
    PowerSourceKind powerSource = PowerManager::PowerSourceKind();

    if ((powerSource == PowerSourceKind::DC 
        && batteryStatus == BatteryStatus::Discharging 
        && remainingCharge < 25)
        || (powerSource == PowerSourceKind::AC
        && powerStatus == PowerSupplyStatus::Inadequate))
    {
        // The device is not in a good battery/power state, 
        // so we should pause any non-critical work.
        PauseNonCriticalWork();
    }
    else if ((batteryStatus != BatteryStatus::Discharging && remainingCharge > 75)
        && powerStatus != PowerSupplyStatus::Inadequate)
    {
        // The device is in good battery/power state,
        // so let's kick of some high-power work.
        StartPowerIntensiveWork();
    }
}

///////////////////////////////////////////////////////////////////////////////


// Simulate starting/stopping work ////////////////////////////////////////////

void PauseNonCriticalWork() 
{
    bWorkInProgress = false;
    OutputMessage(L"paused non-critical work");
}

void StartPowerIntensiveWork()
{
    if (!bWorkInProgress)
    {
        bWorkInProgress = true;
        OutputMessage(L"starting power-intensive work");
    }
}

void StopUpdatingGraphics() 
{
    OutputMessage(L"stopped updating graphics");
}

void StartDoingBackgroundWork() 
{
    OutputMessage(L"starting background work");
}

///////////////////////////////////////////////////////////////////////////////
