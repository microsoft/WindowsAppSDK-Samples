#pragma once

#include <MddBootstrap.h>

HRESULT LoadWindowsAppSdk(UINT16 majorVersion, UINT16 minorVersion, PCWSTR versionTag);
void UnloadWindowsAppSdk();

