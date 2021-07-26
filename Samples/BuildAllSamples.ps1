[CmdLetBinding()]
Param(
    [string]$windowsAppSDKVersion
)
$msbuild = "msbuild.exe"
# $testroot = "$PSScriptRoot\HelloWorld\cpp-winui\ReunionCppDesktopSampleApp.sln"
# Write-Host $testroot ": " $windowsAppSDKVersion

# nuget update $testroot -id Microsoft.WindowsAppSDK.Foundation -version $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK.DWrite -version $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK -version $windowsAppSDKVersion

# & $msbuild $testroot /restore /p:platform=x86 /p:configuration=release


# $testroot = "$PSScriptRoot\ResourceManagement\cs-winui\winui_desktop_packaged_app.sln"
# Write-Host $testroot ": " $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK.WinUI -version $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK.InteractiveExperiences -version $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK.Foundation -version $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK.DWrite -version $windowsAppSDKVersion
# nuget update $testroot -id Microsoft.WindowsAppSDK -version $windowsAppSDKVersion

# & $msbuild $testroot /restore /p:platform=x86 /p:configuration=debug
# $winuicppsln = "$PSScriptRoot\ResourceManagement\cpp-winui\winui_desktop_packaged_app_cpp.sln"
# $winuicppwapproj= "$PSScriptRoot\ResourceManagement\cpp-winui\winui_desktop_packaged_app_cpp (Package)\winui_desktop_packaged_app_cpp (Package).wapproj"
# Write-Host $winuicppsln ": " $windowsAppSDKVersion
# nuget update $winuicppsln -id Microsoft.WindowsAppSDK.WinUI -version $windowsAppSDKVersion
# nuget update $winuicppsln -id Microsoft.WindowsAppSDK.InteractiveExperiences -version $windowsAppSDKVersion
# nuget update $winuicppsln -id Microsoft.WindowsAppSDK.Foundation -version $windowsAppSDKVersion
# nuget update $winuicppsln -id Microsoft.WindowsAppSDK.DWrite -version $windowsAppSDKVersion
# nuget update $winuicppsln -id Microsoft.WindowsAppSDK -version $windowsAppSDKVersion


# dotnet remove $winuicppsln package Microsoft.WindowsAppSDK
# dotnet add $winuicppsln package Microsoft.WindowsAppSDK.WinUI -v $windowsAppSDKVersion

# # & $msbuild $testroot /restore /p:platform=x86 /p:configuration=debug


$winuicppsln = "$PSScriptRoot\ResourceManagement\cpp-console-unpackaged\console_unpackaged_app.sln"
Write-Host $winuicppsln ": " $windowsAppSDKVersion
nuget update $winuicppsln -id Microsoft.WindowsAppSDK.WinUI -version $windowsAppSDKVersion
nuget update $winuicppsln -id Microsoft.WindowsAppSDK.InteractiveExperiences -version $windowsAppSDKVersion
nuget update $winuicppsln -id Microsoft.WindowsAppSDK.Foundation -version $windowsAppSDKVersion
nuget update $winuicppsln -id Microsoft.WindowsAppSDK.DWrite -version $windowsAppSDKVersion
nuget update $winuicppsln -id Microsoft.WindowsAppSDK -version $windowsAppSDKVersion
nuget restore $winuicppsln
& $msbuild $winuicppsln /restore /p:platform=x86 /p:configuration=debug