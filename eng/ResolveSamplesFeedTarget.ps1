<#
.SYNOPSIS
    Validates and resolves the Samples feed hydration target.
.DESCRIPTION
    Normalizes branch names and refs, with empty targets using the triggering
    branch. Version mode requires an explicit NuGet version. Emits job variables
    for later steps.
.PARAMETER HydrationMode
    Branch, ValidateOnly, or WindowsAppSDKVersion.
.PARAMETER Target
    BranchOrVersion parameter value, supplied through HYDRATION_TARGET.
.PARAMETER SourceBranch
    Triggering Git ref, normally supplied by BUILD_SOURCEBRANCH.
.EXAMPLE
    .\ResolveSamplesFeedTarget.ps1 -HydrationMode Branch -Target '' `
        -SourceBranch 'refs/heads/main'
.EXAMPLE
    .\ResolveSamplesFeedTarget.ps1 -HydrationMode WindowsAppSDKVersion `
        -Target '2.7.0'
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory)]
    [ValidateSet('Branch', 'ValidateOnly', 'WindowsAppSDKVersion')]
    [string]$HydrationMode,

    [AllowEmptyString()]
    [string]$Target = $env:HYDRATION_TARGET,

    [string]$SourceBranch = $env:BUILD_SOURCEBRANCH
)

Set-StrictMode -Version 3.0
$ErrorActionPreference = 'Stop'

if ($HydrationMode -eq 'WindowsAppSDKVersion')
{
    if ([string]::IsNullOrWhiteSpace($Target) -or
        $Target.Trim() -notmatch '^[0-9]+(\.[0-9]+){0,3}(-[0-9A-Za-z-]+(\.[0-9A-Za-z-]+)*)?(\+[0-9A-Za-z-]+(\.[0-9A-Za-z-]+)*)?$')
    {
        throw 'BranchOrVersion must contain an explicit NuGet version in WindowsAppSDKVersion mode.'
    }

    $version = $Target.Trim()
    Write-Output "##vso[task.setvariable variable=EffectiveWindowsAppSDKVersion]$version"
}
else
{
    $branch = if ([string]::IsNullOrWhiteSpace($Target))
    {
        $SourceBranch
    }
    else
    {
        $Target.Trim()
    }

    $branch = $branch -replace '^refs/heads/', ''

    if ($branch -notmatch '^(main|release/[A-Za-z0-9][A-Za-z0-9._-]*)$')
    {
        throw "The selected branch must be 'main' or a protected 'release/*' branch; received '$branch'."
    }

    Write-Output "##vso[task.setvariable variable=EffectiveSamplesBranch]$branch"
}
