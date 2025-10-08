// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

//
// ABI Utilities Header
//
// Common utilities for working with Windows Runtime ABI interfaces.
// This header provides essential helper classes for ABI development.
//

#include <windows.h>
#include <combaseapi.h>
#include <string>

namespace CppAbiEPEnumerationSample
{

    /// <summary>
    /// Simple RAII wrapper for HSTRING
    /// Provides automatic memory management and easy conversion to std::wstring
    /// </summary>
    class HStringWrapper {
    public:
        HStringWrapper() : m_hstr(nullptr) {}
        explicit HStringWrapper(HSTRING hstr) : m_hstr(hstr) {}

        ~HStringWrapper() {
            if (m_hstr) {
                WindowsDeleteString(m_hstr);
            }
        }

        // Move constructor
        HStringWrapper(HStringWrapper&& other) noexcept : m_hstr(other.m_hstr) {
            other.m_hstr = nullptr;
        }

        // Move assignment
        HStringWrapper& operator=(HStringWrapper&& other) noexcept {
            if (this != &other) {
                if (m_hstr) {
                    WindowsDeleteString(m_hstr);
                }
                m_hstr = other.m_hstr;
                other.m_hstr = nullptr;
            }
            return *this;
        }

        // Disable copy
        HStringWrapper(const HStringWrapper&) = delete;
        HStringWrapper& operator=(const HStringWrapper&) = delete;

        HSTRING get() const { return m_hstr; }
        HSTRING* put() {
            if (m_hstr) {
                WindowsDeleteString(m_hstr);
                m_hstr = nullptr;
            }
            return &m_hstr;
        }

        std::wstring ToString() const {
            if (!m_hstr) return L"";
            UINT32 length = 0;
            const wchar_t* buffer = WindowsGetStringRawBuffer(m_hstr, &length);
            return std::wstring(buffer, length);
        }

        static HRESULT Create(const std::wstring& str, HSTRING* result) {
            return WindowsCreateString(str.c_str(), static_cast<UINT32>(str.length()), result);
        }

    private:
        HSTRING m_hstr;
    };

} // namespace CppAbiEPEnumerationSample