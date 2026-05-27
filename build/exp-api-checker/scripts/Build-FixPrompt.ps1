<#
.SYNOPSIS
    Build a complete LLM prompt for fixing experimental API usage in a sample file.

.DESCRIPTION
    Combines detection results, file content, and the suggest-fix.md template into
    a ready-to-send prompt payload. The output can be piped into an LLM (Copilot,
    Claude, GPT, etc.) to get specific fix suggestions.

    Workflow:
      1. Detection (Detect-ExpApi.ps1) finds files with exp API usage
      2. This script builds a full LLM prompt for each file
      3. Paste the prompt into an LLM and apply the suggested diff

.PARAMETER Path
    Folder or single file to analyze. Defaults to current directory.

.PARAMETER OutputDir
    Directory to write per-file prompt files. If omitted, prompts are printed
    to stdout separated by '=== PROMPT FOR <file> ===' headers.

.PARAMETER PromptTemplate
    Path to the prompt template. Defaults to ../prompts/suggest-fix.md

.EXAMPLE
    .\Build-FixPrompt.ps1 -Path ..\..\..\Samples\WindowsAIFoundry\cs-winui\Models\VideoScalerModel.cs

.EXAMPLE
    .\Build-FixPrompt.ps1 -Path ..\..\..\Samples\WindowsAIFoundry\cs-winui -OutputDir .\prompts-out
#>
[CmdletBinding()]
param(
    [Parameter(Position = 0)]
    [string]$Path = ".",

    [string]$OutputDir,

    [string]$PromptTemplate = (Join-Path $PSScriptRoot "..\prompts\suggest-fix.md")
)

$ErrorActionPreference = 'Stop'

# Load template
if (-not (Test-Path $PromptTemplate)) {
    Write-Error "Prompt template not found: $PromptTemplate"
    exit 1
}
$template = Get-Content $PromptTemplate -Raw

# Call detection script and capture clean JSON output
$detectScript = Join-Path $PSScriptRoot "Detect-ExpApi.ps1"
if (-not (Test-Path $detectScript)) {
    Write-Error "Detection script not found: $detectScript"
    exit 1
}

Write-Host "Running detection on: $Path" -ForegroundColor Cyan
# Redirect host (6) and warning (3) streams to $null so only JSON reaches stdout
$jsonText = & $detectScript -Path $Path -OutputFormat json 6>$null 3>$null | Out-String

if ([string]::IsNullOrWhiteSpace($jsonText)) {
    Write-Host "No experimental API usage detected." -ForegroundColor Green
    return
}

try {
    $results = $jsonText | ConvertFrom-Json
} catch {
    Write-Error "Failed to parse detection results: $_"
    Write-Host "Raw output was:" -ForegroundColor Red
    Write-Host $jsonText
    exit 1
}

# Normalize to array (ConvertFrom-Json returns single object when only 1 result)
if ($null -eq $results) {
    Write-Host "No experimental API usage detected." -ForegroundColor Green
    return
}
if ($results -isnot [System.Array]) {
    $results = @($results)
}

Write-Host "Building prompts for $($results.Count) file(s)..." -ForegroundColor Cyan
Write-Host ""

if ($OutputDir) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

foreach ($r in $results) {
    $filePath = $r.File
    if (-not (Test-Path $filePath)) {
        Write-Warning "Skipping (not found): $filePath"
        continue
    }

    $fileContent = Get-Content $filePath -Raw -ErrorAction Stop

    # Language hint for code fence
    $ext = [System.IO.Path]::GetExtension($filePath).ToLower()
    $language = switch ($ext) {
        '.cs'  { 'csharp' }
        '.cpp' { 'cpp' }
        '.h'   { 'cpp' }
        default { 'text' }
    }

    # Normalize Hits to array
    $hits = $r.Hits
    if ($hits -isnot [System.Array]) { $hits = @($hits) }

    # Build detection results section
    $sb = New-Object System.Text.StringBuilder
    foreach ($hit in $hits) {
        [void]$sb.AppendLine("- Line $($hit.Line): ``$($hit.Content)`` (namespace: ``$($hit.Namespace)``)")
    }

    # Normalize Namespaces to array
    $namespaces = $r.Namespaces
    if ($namespaces -isnot [System.Array]) { $namespaces = @($namespaces) }
    $namespacesStr = ($namespaces -join ', ')

    # Fill template using literal string replacement (avoid regex pitfalls)
    $prompt = $template
    $prompt = $prompt.Replace('{{FILE_PATH}}', $filePath)
    $prompt = $prompt.Replace('{{NAMESPACES}}', $namespacesStr)
    $prompt = $prompt.Replace('{{DETECTION_RESULTS}}', $sb.ToString().TrimEnd())
    $prompt = $prompt.Replace('{{LANGUAGE}}', $language)
    $prompt = $prompt.Replace('{{FILE_CONTENT}}', $fileContent)

    if ($OutputDir) {
        $safeName = ($r.RelativePath -replace '[\\/:]', '_') + '.prompt.md'
        $outPath = Join-Path $OutputDir $safeName
        Set-Content -Path $outPath -Value $prompt -Encoding UTF8
        Write-Host "  Wrote: $outPath" -ForegroundColor Gray
    } else {
        Write-Host "=== PROMPT FOR $($r.RelativePath) ===" -ForegroundColor Yellow
        Write-Output $prompt
        Write-Host "=== END PROMPT ===" -ForegroundColor Yellow
        Write-Host ""
    }
}

if ($OutputDir) {
    Write-Host ""
    Write-Host "Done. $($results.Count) prompt(s) written to: $OutputDir" -ForegroundColor Green
    Write-Host ""
    Write-Host "Next: paste a prompt into your LLM (Copilot Chat, Claude, GPT)" -ForegroundColor Cyan
    Write-Host "      and apply the returned diff to the source file." -ForegroundColor Cyan
}
