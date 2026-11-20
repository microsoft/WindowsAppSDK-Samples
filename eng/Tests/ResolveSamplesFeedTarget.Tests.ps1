<#
.SYNOPSIS
    Tests mode-specific Samples feed target resolution.
.DESCRIPTION
    Checks blank branch fallback, explicit targets, and invalid input without
    accessing package feeds or changing upstream settings.
.EXAMPLE
    powershell -NoProfile -Command `
        "Invoke-Pester .\eng\Tests\ResolveSamplesFeedTarget.Tests.ps1"
#>
Describe 'Samples feed target resolution' {
    $resolver = Join-Path (Split-Path $PSScriptRoot -Parent) 'ResolveSamplesFeedTarget.ps1'

    It 'uses the triggering branch for blank <Mode> input' -TestCases @(
        @{ Mode = 'Branch'; Target = ''; Source = 'refs/heads/main'; Branch = 'main' }
        @{ Mode = 'ValidateOnly'; Target = ''; Source = 'refs/heads/release/1.8'; Branch = 'release/1.8' }
        @{ Mode = 'Branch'; Target = '  '; Source = 'refs/heads/release/2.0'; Branch = 'release/2.0' }
    ) {
        param($Mode, $Target, $Source, $Branch)
        & $resolver -HydrationMode $Mode -Target $Target -SourceBranch $Source |
            Should Be "##vso[task.setvariable variable=EffectiveSamplesBranch]$Branch"
    }

    It 'uses an explicit protected branch instead of the triggering branch' {
        & $resolver -HydrationMode Branch -Target 'release/1.8' -SourceBranch 'refs/heads/main' |
            Should Be '##vso[task.setvariable variable=EffectiveSamplesBranch]release/1.8'
    }

    It 'rejects unprotected branch input <Target>' -TestCases @(
        @{ Target = '.' }
        @{ Target = 'feature/test' }
        @{ Target = '2.7.0' }
    ) {
        param($Target)
        { & $resolver -HydrationMode Branch -Target $Target } | Should Throw
    }

    It 'rejects an unprotected triggering ref when the branch is blank' {
        { & $resolver -HydrationMode ValidateOnly -Target '' -SourceBranch 'refs/pull/42/merge' } |
            Should Throw
    }

    It 'requires an explicit valid version instead of <Target>' -TestCases @(
        @{ Target = '' }
        @{ Target = '  ' }
        @{ Target = '.' }
        @{ Target = 'main' }
        @{ Target = '2..7' }
    ) {
        param($Target)
        { & $resolver -HydrationMode WindowsAppSDKVersion -Target $Target } | Should Throw
    }

    It 'accepts NuGet version <Version>' -TestCases @(
        @{ Version = '2.7.0' }
        @{ Version = '1.8.260921001' }
        @{ Version = '2.6.6-experimental' }
        @{ Version = '2.7.0-preview.1+build.2' }
    ) {
        param($Version)
        & $resolver -HydrationMode WindowsAppSDKVersion -Target $Version |
            Should Be "##vso[task.setvariable variable=EffectiveWindowsAppSDKVersion]$Version"
    }
}
