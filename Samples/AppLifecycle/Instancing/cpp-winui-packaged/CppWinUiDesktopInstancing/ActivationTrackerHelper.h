#pragma once
#include <Windows.h>
#include <wil/result.h>
#include <memory>
#include <string>
#include <string_view>

constexpr std::wstring_view c_redirectionKeyName{ L"SOFTWARE\\AppLifecycleExample"};
constexpr std::wstring_view c_redirectionValueName{ L"NumberOfActivations" };

inline wil::unique_hkey GetRegistryKey()
{
    wil::unique_hkey keyToRedirectionTracker{};
    THROW_IF_WIN32_ERROR(RegCreateKeyEx(HKEY_CURRENT_USER, c_redirectionKeyName.data(), 0, nullptr, REG_OPTION_VOLATILE, KEY_ALL_ACCESS, nullptr, keyToRedirectionTracker.put(), nullptr));

    return std::move(keyToRedirectionTracker);
}

inline void SetNumberOfActivations(DWORD newNumberOfActivations)
{
    auto keyToRedirectionTracker{GetRegistryKey()};
    THROW_IF_WIN32_ERROR(RegSetValueEx(keyToRedirectionTracker.get(), c_redirectionValueName.data(), 0, REG_DWORD, reinterpret_cast<const BYTE*>(&newNumberOfActivations), sizeof(newNumberOfActivations)));
}

inline DWORD GetNumberOfActivations()
{
    DWORD numberOfActivations = 0;
    auto instanceTrackerKey = GetRegistryKey();

    DWORD sizeOfEntry{};
    auto getResult{ RegQueryValueEx(instanceTrackerKey.get(), c_redirectionValueName.data(), nullptr, nullptr, reinterpret_cast<byte*>(&numberOfActivations), &sizeOfEntry) };

    if (getResult == ERROR_FILE_NOT_FOUND)
    {
        THROW_IF_WIN32_ERROR(RegSetValueEx(instanceTrackerKey.get(), c_redirectionValueName.data(), 0, REG_DWORD, reinterpret_cast<byte*>(&numberOfActivations), sizeof(numberOfActivations)));
        return numberOfActivations;
    }
    else if (getResult != ERROR_MORE_DATA)
    {
        THROW_WIN32(getResult);
    }

    THROW_IF_WIN32_ERROR(RegQueryValueEx(instanceTrackerKey.get(), c_redirectionValueName.data(), nullptr, nullptr, reinterpret_cast<BYTE*>(&numberOfActivations), &sizeOfEntry));
    return numberOfActivations;
}

inline DWORD IncrementNumberOfRedirections()
{
    auto numberOfActivations{GetNumberOfActivations()};
    numberOfActivations++;
    SetNumberOfActivations(numberOfActivations);
    return numberOfActivations;
}
