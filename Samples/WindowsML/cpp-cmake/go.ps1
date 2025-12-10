# Launch-WithWinDbgX-LoaderSnaps.ps1
param(
    [Parameter(Mandatory=$true)]
    [string]$ExecutablePath,
    
    [string]$WinDbgXPath = "",
    
    [string[]]$Arguments = @(),
    
    [string]$SymbolPath = "",
    
    [string]$SourcePath = "",
    
    [switch]$AttachToExisting,
    
    [int]$ProcessId = 0,
    
    [switch]$VerboseLoaderSnaps
)

# Function to find WinDbgX if not specified
function Find-WinDbgX {
    $commonPaths = @(
        "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x64\windbgx.exe",
        "${env:ProgramFiles}\Windows Kits\10\Debuggers\x64\windbgx.exe",
        "${env:ProgramFiles(x86)}\Windows Kits\10\Debuggers\x86\windbgx.exe",
        "${env:ProgramFiles}\Microsoft Windows SDK\Debuggers\x64\windbgx.exe",
        "windbgx.exe"  # Try PATH
    )
    
    foreach ($path in $commonPaths) {
        if (Test-Path $path -ErrorAction SilentlyContinue) {
            return $path
        }
    }
    
    # Try to find via Get-Command
    try {
        $cmd = Get-Command windbgx.exe -ErrorAction Stop
        return $cmd.Source
    }
    catch {
        throw "WinDbgX not found. Please specify the path using -WinDbgXPath parameter or ensure it's in your PATH."
    }
}

try {
    # Find WinDbgX executable
    if ([string]::IsNullOrEmpty($WinDbgXPath)) {
        $WinDbgXPath = Find-WinDbgX
    }
    
    if (-not (Test-Path $WinDbgXPath)) {
        throw "WinDbgX not found at: $WinDbgXPath"
    }
    
    Write-Host "Using WinDbgX at: $WinDbgXPath" -ForegroundColor Green
    
    # Build WinDbgX arguments
    $windbgxArgs = @()
    
    if ($AttachToExisting -and $ProcessId -gt 0) {
        $windbgxArgs += "-p"
        $windbgxArgs += $ProcessId.ToString()
    }
    elseif (-not $AttachToExisting) {
        # Check if executable exists
        if (-not (Test-Path $ExecutablePath)) {
            throw "Executable not found: $ExecutablePath"
        }
        
        # Add executable path
        $windbgxArgs += $ExecutablePath
        
        # Add executable arguments if provided
        if ($Arguments.Count -gt 0) {
            $windbgxArgs += $Arguments
        }
    }
    
    # Add symbol path if specified
    if (-not [string]::IsNullOrEmpty($SymbolPath)) {
        $windbgxArgs = @("-y", $SymbolPath) + $windbgxArgs
    }
    
    # Add source path if specified
    if (-not [string]::IsNullOrEmpty($SourcePath)) {
        $windbgxArgs = @("-srcpath", $SourcePath) + $windbgxArgs
    }
    
    # Create the initial debugger commands to enable loader snaps
    $debuggerCommands = @()
    
    # Enable loader snaps - shows DLL loads/unloads
    $debuggerCommands += "sxe ld"   # Enable break on module load
    $debuggerCommands += "sxe ud"   # Enable break on module unload
    
    if ($VerboseLoaderSnaps) {
        # Enable more verbose loader output
        $debuggerCommands += "!gflag +sls"  # Enable show loader snaps
        $debuggerCommands += "sxi ld"       # Ignore (don't break) on module load but still show output
        $debuggerCommands += "sxi ud"       # Ignore (don't break) on module unload but still show output
    }
    
    # Continue execution after setup
    $debuggerCommands += "g"
    
    # Create a temporary script file with the commands
    $tempScript = [System.IO.Path]::GetTempFileName() + ".wds"
    $debuggerCommands | Out-File -FilePath $tempScript -Encoding ASCII
    
    # Add the script to WinDbgX arguments
    $windbgxArgs = @("-c", "`$<$tempScript") + $windbgxArgs
    
    # Display the command being executed
    $commandLine = "`"$WinDbgXPath`" " + ($windbgxArgs -join " ")
    Write-Host "Executing: $commandLine" -ForegroundColor Yellow
    Write-Host "Loader snaps enabled - you'll see DLL load/unload information" -ForegroundColor Cyan
    
    if ($VerboseLoaderSnaps) {
        Write-Host "Verbose loader snaps enabled - process will run continuously with loader output" -ForegroundColor Cyan
    } else {
        Write-Host "Process will break on each DLL load/unload - use 'g' to continue" -ForegroundColor Cyan
    }
    
    # Launch WinDbgX
    $process = Start-Process -FilePath $WinDbgXPath -ArgumentList $windbgxArgs -PassThru
    
    # Clean up the temporary script file after a short delay
    Start-Job -ScriptBlock {
        param($scriptPath)
        Start-Sleep -Seconds 10
        if (Test-Path $scriptPath) {
            Remove-Item $scriptPath -Force
        }
    } -ArgumentList $tempScript | Out-Null
    
    Write-Host "WinDbgX launched successfully with loader snaps enabled!" -ForegroundColor Green
    Write-Host "Temp script: $tempScript (will be cleaned up automatically)" -ForegroundColor Gray
}
catch {
    Write-Error "Failed to launch WinDbgX: $($_.Exception.Message)"
    exit 1
}