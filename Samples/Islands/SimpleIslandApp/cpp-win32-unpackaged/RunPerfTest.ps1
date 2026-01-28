param(
    [string]$Configuration = "Debug",
    [string]$Platform = "x64"
)

$exePath='D:\WindowsAppSDK-Samples\Samples\Islands\SimpleIslandApp\cpp-win32-unpackaged\x64\Release\SimpleIslandApp\SimpleIslandApp.exe'

if (-not $exePath) {
    Write-Error "Could not find SimpleIslandApp.exe in common output directories."
    Write-Host "Searched paths:"
    foreach ($path in $possiblePaths) {
        Write-Host "  $path"
    }
    exit 1
}

Write-Host "Found executable: $exePath"
Write-Host "Starting performance test..."

# Start the process and wait for it to exit
$process = Start-Process -FilePath $exePath -PassThru -Wait

Write-Host "Performance test completed. Exit code: $($process.ExitCode)"
exit $process.ExitCode
