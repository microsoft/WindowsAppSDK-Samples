param(
    [string]$Platform = "x64",
    [string]$Configuration = "Release"
)

Write-Host "Building WindowsML WPF Sparse Sample..."

# Build the application
Write-Host "Building solution..."
& dotnet build "WindowsMLSampleForWPF.sln" -c $Configuration

if ($LASTEXITCODE -ne 0) {
    Write-Error "Build failed!"
    exit $LASTEXITCODE
}

# Create sparse package
Write-Host "Creating sparse package..."
$outputDir = "WindowsMLSampleForWPF\bin\$Configuration\net9.0-windows10.0.26100.0\win-$Platform"
$packageName = "WindowsMLSampleForWPF_$Platform.msix"

& MakeAppx.exe pack /d $outputDir /p $packageName /o

if ($LASTEXITCODE -ne 0) {
    Write-Error "Package creation failed!"
    exit $LASTEXITCODE
}

Write-Host "Build and packaging completed successfully!"
Write-Host "Package created: $packageName"
