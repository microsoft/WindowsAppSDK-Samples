# Format all C++ and C# code in the WindowsML directory
param([switch]$WhatIf, [switch]$Verbose)

$WindowsMLDir = $PSScriptRoot
Write-Host "Formatting code in WindowsML directory..." -ForegroundColor Green

# Test tools availability
function Test-Tool($name, $cmd) {
    try { Invoke-Expression $cmd | Out-Null; return $true }
    catch { Write-Host "$name not available" -ForegroundColor Red; return $false }
}

if (-not ((Test-Tool "clang-format" "clang-format --version") -and (Test-Tool "dotnet format" "dotnet format --version"))) {
    exit 1
}

# Format C++ files
Write-Host "Formatting C++ files..." -ForegroundColor Yellow
$cppFiles = Get-ChildItem -Path $WindowsMLDir -Recurse -Include "*.cpp", "*.h", "*.hpp", "*.c" |
            Where-Object { $_.FullName -notlike "*\packages\*" -and $_.FullName -notlike "*\.vs\*" -and $_.FullName -notlike "*\x64\*" -and $_.FullName -notlike "*\Debug\*" -and $_.FullName -notlike "*\Release\*" -and $_.FullName -notlike "*Generated Files*" }

Write-Host "Found $($cppFiles.Count) C++ files" -ForegroundColor Cyan
foreach ($file in $cppFiles) {
    $rel = $file.FullName.Substring($WindowsMLDir.Length + 1)
    if ($WhatIf) {
        Write-Host "  Would format: $rel" -ForegroundColor Gray
    } else {
        if ($Verbose) { Write-Host "  Formatting: $rel" -ForegroundColor Gray }
        clang-format -i "$($file.FullName)"
    }
}

# Format C# files
Write-Host "Formatting C# files..." -ForegroundColor Yellow
$csProjects = Get-ChildItem -Path $WindowsMLDir -Recurse -Include "*.csproj", "*.sln" |
              Where-Object { $_.FullName -notlike "*\packages\*" -and $_.FullName -notlike "*\.vs\*" -and $_.FullName -notlike "*\x64\*" -and $_.FullName -notlike "*\Debug\*" -and $_.FullName -notlike "*\Release\*" }

Write-Host "Found $($csProjects.Count) C# projects" -ForegroundColor Cyan
foreach ($project in $csProjects) {
    $rel = $project.FullName.Substring($WindowsMLDir.Length + 1)
    if ($WhatIf) {
        Write-Host "  Would format: $rel" -ForegroundColor Gray
    } else {
        if ($Verbose) { Write-Host "  Formatting: $rel" -ForegroundColor Gray }
        Push-Location (Split-Path $project.FullName)
        dotnet format "$($project.Name)" --verbosity quiet
        Pop-Location
    }
}

Write-Host "Formatting complete!" -ForegroundColor Green
