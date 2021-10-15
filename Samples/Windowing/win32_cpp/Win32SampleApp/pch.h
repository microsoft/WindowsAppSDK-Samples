#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <strsafe.h>

#include "winrt/Windows.Foundation.h"
#include "winrt/Windows.Foundation.Collections.h"
#include "winrt/Microsoft.UI.h"
#include "winrt/Microsoft.UI.Interop.h"
#include "winrt/Microsoft.UI.Windowing.h"


namespace winrt
{
	using namespace winrt::Windows::Foundation;
	using namespace winrt::Microsoft::UI;
	using namespace winrt::Microsoft::UI::Windowing;
}
