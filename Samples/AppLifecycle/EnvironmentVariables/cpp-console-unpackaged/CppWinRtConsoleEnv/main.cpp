// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"

using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Microsoft::Windows::System;

PCWSTR contosoPath{ L"%USERPROFILE%\\ContosoBin" };
PCWSTR xyzExtension{ L".XYZ" };
PCWSTR maxItemsName{ L"MAXITEMS" };
PCWSTR homeDirName{ L"HOMEDIR" };
PCWSTR fruitName{ L"Fruit" };

void GetEnvironmentVariables();
void AddEnvironmentVariables();
void RemoveEnvironmentVariables();
void AddToPath();
void RemoveFromPath();
void AddToPathExt();
void RemoveFromPathExt();

// Helpers ////////////////////////////////////////////////////////////////////

void OutputMessage(const WCHAR* message)
{
	_putws(message);
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


// main ///////////////////////////////////////////////////////////////////////

int main()
{
    init_apartment();

	GetEnvironmentVariables();
	AddEnvironmentVariables();
	RemoveEnvironmentVariables();
	AddToPath();
	RemoveFromPath();
	AddToPathExt();
	RemoveFromPathExt();

	_putws(L"...press any key to exit...");
	getwchar();

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
			OutputFormattedMessage(L"Error getting environment variables: %s", message.c_str());
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
			processManager.SetEnvironmentVariable(maxItemsName, L"1024");
			auto maxItemsValue = processManager.GetEnvironmentVariable(maxItemsName);
			OutputFormattedMessage(L"MAXITEMS=%s", maxItemsValue.c_str());

			// For packaged apps, environment variables set at the user 
			// or machine level will persist until the app is uninstalled.
			// For unpackaged apps, they are not removed on app uninstall.
			auto userManager = EnvironmentManager::GetForUser();
			userManager.SetEnvironmentVariable(homeDirName, L"D:\\Foo");
			auto homeDirValue = userManager.GetEnvironmentVariable(homeDirName);
			OutputFormattedMessage(L"HOMEDIR=%s", homeDirValue.c_str());

			// To set environment variables at machine level, the app must
			// be running elevated, otherwise this will result in Access Denied.
			auto systemManager = EnvironmentManager::GetForMachine();
			systemManager.SetEnvironmentVariable(fruitName, L"Banana");
			auto fruitValue = systemManager.GetEnvironmentVariable(fruitName);
			OutputFormattedMessage(L"Fruit=%s", fruitValue.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(L"Error setting environment variables: %s", message.c_str());
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
			processManager.SetEnvironmentVariable(maxItemsName, L"");
			auto maxItemsValue = processManager.GetEnvironmentVariable(maxItemsName);
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
			userManager.SetEnvironmentVariable(homeDirName, L"");
			auto homeDirValue = userManager.GetEnvironmentVariable(homeDirName);
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
			systemManager.SetEnvironmentVariable(fruitName, L"");
			auto fruitValue = systemManager.GetEnvironmentVariable(fruitName);
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
			OutputFormattedMessage(L"Error removing environment variables: %s", message.c_str());
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
			userManager.AppendToPath(contosoPath);
			auto path = userManager.GetEnvironmentVariable(L"PATH");
			OutputFormattedMessage(L"PATH=%s", path.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(L"Error adding to PATH: %s", message.c_str());
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
			userManager.RemoveFromPath(contosoPath);
			auto path = userManager.GetEnvironmentVariable(L"PATH");
			OutputFormattedMessage(L"PATH=%s", path.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(L"Error removing from PATH: %s", message.c_str());
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
			userManager.AddExecutableFileExtension(xyzExtension);

			auto pathExt = userManager.GetEnvironmentVariable(L"PATHEXT");
			OutputFormattedMessage(L"PATHEXT=%s", pathExt.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(L"Error adding to PATHEXT: %s", message.c_str());
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
			userManager.RemoveExecutableFileExtension(xyzExtension);
			auto pathExt = userManager.GetEnvironmentVariable(L"PATHEXT");
			OutputFormattedMessage(L"PATHEXT=%s", pathExt.c_str());
		}
		catch (winrt::hresult_error const& ex)
		{
			winrt::hresult hr = ex.code();
			winrt::hstring message = ex.message();
			OutputFormattedMessage(L"Error removing from PATHEXT: %s", message.c_str());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
