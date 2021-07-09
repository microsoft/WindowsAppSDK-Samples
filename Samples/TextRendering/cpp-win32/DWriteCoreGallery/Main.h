// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

// Windows Header Files
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// DWriteCore header file.
#include <dwrite_core.h>

// Windows implementation library headers.
#include <wil/com.h>
#include <wil/result.h>
#include <wil/resource.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <stdint.h>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <algorithm>
#include <span>

// Global variables.
extern HINSTANCE g_hInstance;
extern wil::com_ptr<IDWriteFactory7> g_factory;

_Ret_z_ WCHAR const* GetAppTitle();

