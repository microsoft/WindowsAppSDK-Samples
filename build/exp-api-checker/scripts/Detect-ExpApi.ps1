<#
.SYNOPSIS
    Detect experimental API usage in Windows App SDK sample code.

.DESCRIPTION
    Scans C# and C++ source files for experimental-only API usage.
    Reports file locations and specific API references found.

.PARAMETER Path
    File or folder to scan. Defaults to current directory.

.PARAMETER OutputFormat
    Output format: 'text' (default) or 'json'

.EXAMPLE
    ./Detect-ExpApi.ps1 -Path ./Samples/WindowsAIFoundry/cs-winui

.EXAMPLE
    ./Detect-ExpApi.ps1 -Path ./MyFile.cs -OutputFormat json
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$Path = ".",

    [ValidateSet("text", "json")]
    [string]$OutputFormat = "text"
)

$ErrorActionPreference = 'Stop'

# Load exp-only API data
$scriptDir = $PSScriptRoot
$apiListPath = Join-Path $scriptDir "exp-only-apis.json"

if (-not (Test-Path $apiListPath)) {
    Write-Error "Exp-only API list not found: $apiListPath`nRun build/winmd-tools/Compare-WinAppSdkApis.ps1 to generate it."
    exit 1
}

$apiData = Get-Content $apiListPath -Raw | ConvertFrom-Json

# Truly exp-only namespaces (entire assembly is exp-only, namespace match is safe)
$expOnlyNamespaces = @(
    "Microsoft.Windows.AI.Text.Experimental",
    "Microsoft.Windows.AI.Video",
    "Microsoft.Windows.Search.AppContentIndex",
    "Microsoft.UI.Composition.Experimental",
    "Microsoft.UI.Input.Experimental",
    "Microsoft.Windows.VisionInternal",
    "Microsoft.Windows.AI.GenerativeInternal"
)

# NOTE: namespaces like Microsoft.Windows.AI.Imaging and Microsoft.UI.Xaml.Controls
# are MIXED (some types stable, some exp). Do NOT match the namespace - rely on
# the type-name list below for those.

# Extract simple type names (last segment) from the full exp-only type list.
# These are matched as whole-word identifiers in source code.
$expOnlyTypeNames = $apiData.ExpOnlyTypes | ForEach-Object {
    ($_ -split '\.')[-1]
} | Where-Object {
    # Exclude very generic names that would produce too many false positives
    $_ -and $_.Length -ge 4 -and $_ -notmatch '^(Args|Info|Type|State|Status|Result|Options|Contract)$'
} | Sort-Object -Unique

# Build map: simple name -> full name (for reporting)
$typeNameMap = @{}
foreach ($t in $apiData.ExpOnlyTypes) {
    $simple = ($t -split '\.')[-1]
    if (-not $typeNameMap.ContainsKey($simple)) {
        $typeNameMap[$simple] = $t
    }
}

# Resolve path
$Path = Resolve-Path $Path -ErrorAction Stop

# Find files to scan
$files = @()
if (Test-Path $Path -PathType Leaf) {
    $files = @(Get-Item $Path)
} else {
    $files = Get-ChildItem $Path -Recurse -Include "*.cs", "*.cpp", "*.h" -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -notmatch '\\obj\\|\\bin\\|\\Generated Files\\' }
}

Write-Host "Scanning $($files.Count) source files..." -ForegroundColor Cyan
Write-Host ""

$results = [System.Collections.ArrayList]::new()

foreach ($file in $files) {
    $content = Get-Content $file.FullName -Raw -ErrorAction SilentlyContinue
    if (-not $content) { continue }
    
    $lines = Get-Content $file.FullName -ErrorAction SilentlyContinue
    $fileHits = [System.Collections.ArrayList]::new()

    # --- Pass 1: namespace-level matches (truly exp-only assemblies) ---
    foreach ($ns in $expOnlyNamespaces) {
        # C# style (Microsoft.X.Y) or C++ style (Microsoft::X::Y)
        $patterns = @(
            [regex]::Escape($ns),
            [regex]::Escape($ns.Replace('.', '::'))
        )
        foreach ($pat in $patterns) {
            if ($content -match $pat) {
                for ($i = 0; $i -lt $lines.Count; $i++) {
                    if ($lines[$i] -match $pat) {
                        [void]$fileHits.Add(@{
                            Line = $i + 1
                            Content = $lines[$i].Trim()
                            Match = $ns
                            MatchKind = 'namespace'
                        })
                    }
                }
            }
        }
    }

    # --- Pass 2: type-name matches (for mixed namespaces) ---
    # Quick filter: only do per-line scan if the file mentions any candidate at all
    foreach ($typeName in $expOnlyTypeNames) {
        # Word-boundary check on full content first (fast path)
        if ($content -notmatch "\b$typeName\b") { continue }
        # Skip if the line already matched via namespace (avoid duplicates)
        for ($i = 0; $i -lt $lines.Count; $i++) {
            if ($lines[$i] -match "\b$typeName\b") {
                $fullName = $typeNameMap[$typeName]
                [void]$fileHits.Add(@{
                    Line = $i + 1
                    Content = $lines[$i].Trim()
                    Match = $fullName
                    MatchKind = 'type'
                })
            }
        }
    }

    if ($fileHits.Count -gt 0) {
        # Deduplicate by (line, match)
        $uniqueHits = $fileHits |
            Sort-Object { "$($_.Line)|$($_.Match)" } -Unique |
            Sort-Object { [int]$_.Line }

        # Normalize property name to keep backward-compatible output
        $hitsForOutput = $uniqueHits | ForEach-Object {
            @{
                Line = $_.Line
                Content = $_.Content
                Namespace = $_.Match  # kept for compatibility with Build-FixPrompt
                MatchKind = $_.MatchKind
            }
        }

        [void]$results.Add(@{
            File = $file.FullName
            RelativePath = $file.FullName.Replace((Get-Location).Path + "\", "")
            Hits = $hitsForOutput
            Namespaces = ($hitsForOutput | ForEach-Object { $_.Namespace } | Select-Object -Unique)
        })
    }
}

# Output results
if ($OutputFormat -eq "json") {
    $results | ConvertTo-Json -Depth 5
    return
} else {
    if ($results.Count -eq 0) {
        Write-Host "✅ No experimental API usage detected." -ForegroundColor Green
    } else {
        Write-Host "⚠️  Found experimental API usage in $($results.Count) file(s):" -ForegroundColor Yellow
        Write-Host ""
        
        foreach ($r in $results) {
            Write-Host "[EXP] $($r.RelativePath)" -ForegroundColor Yellow
            
            foreach ($hit in $r.Hits) {
                Write-Host "      Line $($hit.Line): " -NoNewline -ForegroundColor Gray
                Write-Host $hit.Content -ForegroundColor White
            }
            
            Write-Host "      Namespaces: " -NoNewline -ForegroundColor Gray
            Write-Host ($r.Namespaces -join ", ") -ForegroundColor Magenta
            Write-Host ""
        }
        
        # Summary and suggestions
        Write-Host "─────────────────────────────────────────────────────" -ForegroundColor DarkGray
        Write-Host ""
        Write-Host "📋 Suggested Actions:" -ForegroundColor Cyan
        Write-Host ""
        
        # Determine if hybrid or exp-only
        $allNamespaces = $results | ForEach-Object { $_.Namespaces } | ForEach-Object { $_ } | Select-Object -Unique
        
        Write-Host "   1. For HYBRID samples (some features are exp-only):" -ForegroundColor White
        Write-Host "      Wrap exp code with: #if WASDK_EXPERIMENTAL ... #endif" -ForegroundColor Gray
        Write-Host "      For public methods with callers, use #if/#else with a NotSupportedException stub" -ForegroundColor Gray
        Write-Host ""
        Write-Host "   2. For EXP-ONLY samples (entire sample needs exp):" -ForegroundColor White
        Write-Host "      Set <WindowsAppSDKExperimental>true</WindowsAppSDKExperimental> in Directory.Build.props" -ForegroundColor Gray
        Write-Host ""
        Write-Host "   Run with LLM analysis for specific code suggestions." -ForegroundColor DarkGray
    }
}

# Return results for pipeline use (text mode only; JSON mode already returned)
return $results
