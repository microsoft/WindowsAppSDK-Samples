#include "pch.h"
#include "WindowsAppSdkLoader.h"


static bool s_loaded = false;


HRESULT LoadWindowsAppSdk(UINT16 majorVersion, UINT16 minorVersion, PCWSTR versionTag)
{
    HRESULT hr = S_OK;

    if (!s_loaded)
    {
        const UINT32 majorMinorVersion { (UINT32)majorVersion << 16 | ((UINT32)minorVersion) };
        const PACKAGE_VERSION minVersion {};

        hr = MddBootstrapInitialize(majorMinorVersion, versionTag, minVersion);

        if (FAILED(hr))
        {
            TCHAR errorMsg[200];
            StringCbPrintf(errorMsg, sizeof(errorMsg), L"Error 0x%X in MddBootstrapInitialize(0x%08X, %s, %hu.%hu.%hu.%hu)\n",
                hr, majorMinorVersion, versionTag, minVersion.Major, minVersion.Minor, minVersion.Build, minVersion.Revision);
            OutputDebugString(errorMsg);
        }
        else
        {
            s_loaded = true;
        }
    }

    return hr;
}


void UnloadWindowsAppSdk()
{
    if (s_loaded)
    {
        MddBootstrapShutdown();
        s_loaded = false;
    }
}
