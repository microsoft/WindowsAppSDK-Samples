
param (
    [Parameter(Mandatory = $true)]
    [string]$EtlFilePath,

    [Parameter(Mandatory = $true)]
    [string]$WpaProfilePath,

    [Parameter(Mandatory = $true)]
    [string]$OutputFolder
)

# Ensure output folder exists
if (-not (Test-Path $OutputFolder)) {
    New-Item -ItemType Directory -Path $OutputFolder | Out-Null
}

# Define log file path inside OutputFolder
$LogFilePath = Join-Path -Path $OutputFolder -ChildPath "WinmlRundown.log"

# Function to install Windows Performance Toolkit (minimal installation)
function Install-WindowsPerformanceToolkit {
    Write-Host "Windows Performance Toolkit not found. Attempting to install minimal components..."
    
    try {
        # Check if running as administrator
        $currentPrincipal = New-Object Security.Principal.WindowsPrincipal([Security.Principal.WindowsIdentity]::GetCurrent())
        $isAdmin = $currentPrincipal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
        
        if (-not $isAdmin) {
            Write-Host "Administrator privileges required to install Windows Performance Toolkit."
            Write-Host "Please run this script as Administrator or manually install just the Performance Toolkit."
            return $null
        }
        
        # Primary method: Install Windows Performance Toolkit via Windows ADK
        Write-Host "Attempting to install Windows Performance Toolkit via Windows ADK..."
        
        try {
            # Detect system architecture
            $arch = $env:PROCESSOR_ARCHITECTURE
            Write-Host "Detected architecture: $arch"
            
            # Select ADK download URL based on architecture
            # ARM64: Use the ARM64-specific ADK (November 2025)
            # x64/x86: Use the standard Windows 11 ADK (December 2024)
            if ($arch -eq 'ARM64') {
                $adkUrl = "https://go.microsoft.com/fwlink/?linkid=2337875"  # Windows 11 ADK for ARM64 (November 2025)
                $adkVersion = "10.1.28000.1 (ARM64, November 2025)"
            } else {
                $adkUrl = "https://go.microsoft.com/fwlink/?linkid=2289980"  # Windows 11 ADK (December 2024)
                $adkVersion = "10.1.26100.2454 (December 2024)"
            }
            
            Write-Host "Selected ADK version: $adkVersion"
            
            $installerPath = "$env:TEMP\adksetup.exe"
            
            Write-Host "Downloading Windows ADK installer..."
            Write-Host "  URL: $adkUrl"
            Write-Host "  Destination: $installerPath"
            
            try {
                Invoke-WebRequest -Uri $adkUrl -OutFile $installerPath -UseBasicParsing
                Write-Host "  [OK] Download complete" -ForegroundColor Green
            } catch {
                Write-Host "Failed to download Windows ADK installer: $_" -ForegroundColor Red
                throw
            }
            
            Write-Host "Installing Windows Performance Toolkit feature..."
            Write-Host "  This may take a few minutes..."
            
            $installArgs = "/quiet /norestart /features OptionId.WindowsPerformanceToolkit"
            Write-Host "  Running: $installerPath $installArgs"
            
            $process = Start-Process -FilePath $installerPath -ArgumentList $installArgs -Wait -PassThru
            
            if ($process.ExitCode -eq 0 -or $process.ExitCode -eq 3010) {
                Write-Host "  [OK] Installation complete (Exit code: $($process.ExitCode))" -ForegroundColor Green
            } else {
                Write-Host "Installation failed with exit code: $($process.ExitCode)" -ForegroundColor Red
                throw "ADK installation failed"
            }
            
            Write-Host "Verifying wpaexporter.exe installation..."
            
            # Check multiple possible installation paths for wpaexporter.exe
            $possiblePaths = @(
                "$env:ProgramFiles\Windows Kits\10\Windows Performance Toolkit\wpaexporter.exe",
                "${env:ProgramFiles(x86)}\Windows Kits\10\Windows Performance Toolkit\wpaexporter.exe"
            )
            
            $foundPath = $null
            foreach ($path in $possiblePaths) {
                Write-Host "  Checking: $path"
                if (Test-Path $path) {
                    $foundPath = $path
                    break
                }
            }
            
            if ($foundPath) {
                Write-Host "===================================================="
                Write-Host "SUCCESS: Windows Performance Toolkit Installed" -ForegroundColor Green
                Write-Host "===================================================="
                Write-Host "wpaexporter.exe found at: $foundPath"
                
                # Clean up installer
                Remove-Item $installerPath -Force -ErrorAction SilentlyContinue
                return $foundPath  # Return the actual path found
            } else {
                Write-Host "wpaexporter.exe not found after installation" -ForegroundColor Red
                throw "wpaexporter.exe verification failed"
            }
        }
        catch {
            Write-Host "ADK installation failed: $_" -ForegroundColor Red
        }
        
        Write-Host "Automatic installation failed. Manual installation options:"
        Write-Host "1. Download Windows ADK:"
        Write-Host "   - x64/x86: https://go.microsoft.com/fwlink/?linkid=2289980 (December 2024)"
        Write-Host "   - ARM64:   https://go.microsoft.com/fwlink/?linkid=2337875 (November 2025)"
        Write-Host "   - Run: adksetup.exe /quiet /norestart /features OptionId.WindowsPerformanceToolkit"

        return $null
    }
    catch {
        Write-Host "Error during installation: $_"
        return $null
    }
}

# Build wpaexporter path, checking both Program Files locations
$wpaExporterPath = Join-Path "${env:ProgramFiles(x86)}\Windows Kits\10\Windows Performance Toolkit" "wpaexporter.exe"

if (-not (Test-Path $wpaExporterPath)) {
    # Try the regular Program Files directory as fallback
    $wpaExporterPath = Join-Path "${env:ProgramFiles}\Windows Kits\10\Windows Performance Toolkit" "wpaexporter.exe"
    
    if (-not (Test-Path $wpaExporterPath)) {
        Write-Host "wpaexporter.exe not found in either Program Files location."
        Write-Host "Checked paths:"
        Write-Host "  - ${env:ProgramFiles(x86)}\Windows Kits\10\Windows Performance Toolkit\wpaexporter.exe"
        Write-Host "  - ${env:ProgramFiles}\Windows Kits\10\Windows Performance Toolkit\wpaexporter.exe"
        
        # Attempt automatic installation
        $wpaExporterPath = Install-WindowsPerformanceToolkit
        if (-not $wpaExporterPath) {
            Write-Host "Unable to install Windows Performance Toolkit automatically."
            exit 1
        }
    }
}

Write-Host "Exporting ETL to CSV files using $wpaExporterPath..."
& $wpaExporterPath -i $EtlFilePath -profile $WpaProfilePath -outputFolder $OutputFolder

# Step 2: Get all CSV files
$csvFiles = Get-ChildItem -Path $OutputFolder -Filter *.csv

if ($csvFiles.Count -eq 0) {
    Set-Content -Path $LogFilePath -Value "No CSV files found."
    exit
}

# Step 3: Merge CSV files with prefix and column renaming
$allData = foreach ($file in $csvFiles) {
    $prefix = ""
    $columnMap = @{}

    if ($file.Name -like "*onnxversion*") {
        $prefix = "Onnx Version"
        $columnMap = @{
            "Field 4" = "Runtime Version"
            "Field 6" = "Is Redist"
            "Field 7" = "Framework Name"
        }
    } elseif ($file.Name -like "*driverinfo*") {
        $prefix = "Driver Info"
        $columnMap = @{
            "Field 4" = "Device Class"
            "Field 5" = "Driver Name"
            "Field 6" = "Driver Version"
        }
    } elseif ($file.Name -like "*registeredproviders*") {
        $prefix = "Registered Providers"
        $columnMap = @{
            "Field 1" = "PackageFamilyName"
        }
    } elseif ($file.Name -like "*sessioncreation*") {
        $prefix = "Session Creation"
        $columnMap = @{
            "Field 3"  = "Schema Version"
            "Field 4"  = "Session ID"
            "Field 5"  = "IR Version"
            "Field 6"  = "ORT Programming Projection"
            "Field 10" = "Using FP16"
            "Field 14" = "Model Weight Type"
            "Field 15" = "Model Graph Hash"
            "Field 16" = "Model Weight Hash"
            "Field 19" = "EP ID"
        }
    } elseif ($file.Name -like "*epautoselection*") {
        $prefix = "EP Auto Selection"
        $columnMap = @{
            "Field 3"  = "Schema Version"
            "Field 4"  = "Session ID"
            "Field 5"  = "Selection Policy"
            "Field 6"  = "Requested EP"
            "Field 7"  = "Available EP"
        }
    } elseif ($file.Name -like "*winmlonnxerror*") {
        $prefix = "====================WINML ONNX ERROR==================="
        $columnMap = @{
            "Field 3"  = "Schema Version"
            "Field 4"  = "HRESULT"
            "Field 5"  = "Session ID"
            "Field 6"  = "Error Code"
            "Field 7"  = "Error Category"
            "Field 8"  = "Error Message"
            "Field 9"  = "File"
            "Field 10"  = "function"
            "Field 11"  = "Line"
        }
    } elseif ($file.Name -like "*onnxerror*") {
        $prefix = "====================ONNX ERROR==================="
        $columnMap = @{
            "Field 3"  = "Location"
            "Field 4"  = "Message"
        }
    } elseif ($file.Name -like "*winmlVersion*") {
        $prefix = "WindowsAppSDK.ML Version"
        $columnMap = @{
            "Field 5" = "Version"
        }
    } elseif ($file.Name -like "*providerCertification*") {
        $prefix = "Provider Certification"
        $columnMap = @{
            "Field 1"  = "Provider"
            "Field 2"  = "Ready State"
            "Field 3"  = "Certification State"
        }
    }

    Import-Csv $file.FullName | ForEach-Object {
        foreach ($key in $columnMap.Keys) {
            if ($_.PSObject.Properties.Name -contains $key) {
                $_ | Add-Member -NotePropertyName $columnMap[$key] -NotePropertyValue $_.$key
                $_.PSObject.Properties.Remove($key)
            }
        }
        $_ | Add-Member -NotePropertyName "Prefix" -NotePropertyValue $prefix
        $_
    }
}

if (-not $allData -or $allData.Count -eq 0) {
    Set-Content -Path $LogFilePath -Value "No data available."
    exit
}

if (-not ($allData | Get-Member -Name "Process")) {
    Set-Content -Path $LogFilePath -Value "No 'Process' column found."
    exit
}

# Step 4: Group by Process
$groupedData = $allData | Group-Object -Property "Process"

# Step 5: Write log file
Remove-Item -Path $LogFilePath -ErrorAction SilentlyContinue

foreach ($group in $groupedData) {
    Add-Content -Path $LogFilePath -Value "Process: $($group.Name)"
    Add-Content -Path $LogFilePath -Value "----------------------------------------"

    # Sort items within each process group by Time (Local) if the column exists
    $sortedItems = $group.Group
    if ($group.Group | Get-Member -Name "Time (Local)" -MemberType Properties) {
        $sortedItems = $group.Group | Sort-Object "Time (Local)"
    }

    $emittedWasdkVersion = $null
    foreach ($item in $sortedItems) {
        # Extract time information for the prefix line
        $timeValue = ""
        if ($item.PSObject.Properties.Name -contains "Time (Local)" -and $item."Time (Local)") {
            $timeValue = " ($($item.'Time (Local)'))"
        }

        $properties = $item.PSObject.Properties |
                      Where-Object { $_.Name -notin @("Process","Prefix","Time (Local)") } |
                      Where-Object { $_.Value -and $_.Value.Trim() -ne "" }

        if ($properties.Count -gt 0) {
            if ($item.Prefix -eq "WindowsAppSDK.ML Version") {
                $version = ($properties | Where-Object { $_.Name -eq "Version" }).Value
                if ($version -eq $emittedWasdkVersion) { continue }
                $emittedWasdkVersion = $version
            }

            Add-Content -Path $LogFilePath -Value "`t$($item.Prefix)$timeValue :"
            foreach ($prop in $properties) {
                Add-Content -Path $LogFilePath -Value "`t`t$($prop.Name) = $($prop.Value)"
            }
            Add-Content -Path $LogFilePath -Value ""
        }
    }

    Add-Content -Path $LogFilePath -Value "`n"
}

Write-Host "Log file created successfully."

# Delete intermediate CSV files after success
$csvFiles | Remove-Item -Force
Write-Host "Intermediate CSV files deleted."

