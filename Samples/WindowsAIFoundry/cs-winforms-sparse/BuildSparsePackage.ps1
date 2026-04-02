Param(
    [Parameter(Mandatory=$true)]
    [string]$Platform = "arm64",
    [Parameter(Mandatory=$true)]
    [string]$Configuration = "Release",
    [switch]$Clean
)

if ($Clean) {
    $CleanTargets = @(
      'bin'
      'obj'
    )

    $ProjectRoot = (Join-Path $PSScriptRoot "WinForms")
    foreach ($CleanTarget in $CleanTargets)
    {
      $CleanTargetPath = (Join-Path $ProjectRoot $CleanTarget)
      if (Test-Path ($CleanTargetPath)) {
        Remove-Item $CleanTargetPath -recurse
      }
    }
    Get-AppxPackage -Name "WindowsAISampleForWinFormsSparse" | Remove-AppxPackage
}

function Get-UserPath
{
    $root = Join-Path (Get-Item $PSScriptRoot ).FullName "WinForms"
    $user = Join-Path $root '.user'
    if (-not(Test-Path -Path $user -PathType Container))
    {
        Write-Host "Creating $user..."
        $null = New-Item -Path $user -ItemType Directory -Force
    }
    return $user
}

$user_path = Get-UserPath
$pwd_file = Join-Path $user_path 'WindowsAISampleForWinForms.certificate.sample.pwd'
$cert_thumbprint = Join-Path $user_path 'WindowsAISampleForWinForms.certificate.sample.thumbprint'
$cer = Join-Path $user_path 'WindowsAISampleForWinForms.certificate.sample.cer'
$pfx = Join-Path $user_path 'WindowsAISampleForWinForms.certificate.sample.pfx'

if (-not (Test-Path $pfx))
{
    # clear the files    
    $files = Get-ChildItem -Path $user_path
    foreach ($file in $files) {
        if ($file.PSIsContainer) {
            Remove-Item -Path $file.FullName -Recurse -Force
        } else {
            Remove-Item -Path $file.FullName -Force
        }
    }
    
    $charSet = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@'
    $passwordLength = 20
    $password = New-Object -TypeName System.Security.SecureString
    # Generate random characters and append to SecureString
    for ($i = 0; $i -lt $passwordLength; $i++)
    {
        $randomChar = $charSet[(Get-Random -Maximum $charSet.Length)]
        $password.AppendChar($randomChar)
    }
    
    # Convert back to plaintext. This is not secure but this cert is only use for development purposes.
    # Do not use for production purposes.
    $BSTR = [System.Runtime.InteropServices.Marshal]::SecureStringToBSTR($password)
    $passwordPlainText = [System.Runtime.InteropServices.Marshal]::PtrToStringAuto($BSTR)
    [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($BSTR)
    
    Set-Content -Path $pwd_file -Value $passwordPlainText -Force
    
    # Create the certificate
    $cert_path = "cert:\CurrentUser\My"
    $now = Get-Date
    $expiration = $now.AddMonths(12)
    $subject = "CN=Fabrikam Corporation, O=Fabrikam Corporation, L=Redmond, S=Washington, C=US"
    $friendly_name = "Microsoft.WindowsAppSDK Samples Certificate Create=$now"
    $key_friendly_name = "Microsoft.WindowsAppSDK Samples PrivateKey Create=$now"
    $key_description = "Microsoft.WindowsAppSDK Samples PrivateKey Create=$now"
    $eku_oid = '2.5.29.37'
    $eku_value = '1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13'
    $eku = "$eku_oid={text}$eku_value"
    $cert = New-SelfSignedCertificate -CertStoreLocation $cert_path -NotAfter $expiration -Subject $subject -FriendlyName $friendly_name -KeyFriendlyName $key_friendly_name -KeyDescription $key_description -TextExtension $eku
    
    # Save the thumbprint
    $thumbprint = $cert.Thumbprint
    Set-Content -Path $cert_thumbprint -Value $thumbprint -Force
    
    # Export the certificate
    $cer = Join-Path $user_path 'WindowsAISampleForWinForms.certificate.sample.cer'
    $export_cer = Export-Certificate -Cert $cert -FilePath $cer -Force
    $cert_personal = "cert:\CurrentUser\My\$thumbprint"
    $pfx = Join-Path $user_path 'WindowsAISampleForWinForms.certificate.sample.pfx'
    $export_pfx = Export-PfxCertificate -Cert $cert_personal -FilePath $pfx -Password $password    
}

msbuild /restore /p:platform=$Platform /p:configuration=$Configuration
msbuild /p:platform=$Platform /p:configuration=$Configuration

MakeAppx.exe pack /d "$PSScriptRoot\WinForms" /p "$PSScriptRoot\WinForms\bin\$Platform\$Configuration\net8.0-windows10.0.22621.0\WindowsAISampleForWinFormsSparse.msix" /nv /o

$pwd_content = Get-Content -Path $pwd_file
SignTool.exe sign /fd SHA256 /a /f "$PSScriptRoot\WinForms\.user\WindowsAISampleForWinForms.certificate.sample.pfx" /p $pwd_content "$PSScriptRoot\WinForms\bin\$Platform\$Configuration\net8.0-windows10.0.22621.0\WindowsAISampleForWinFormsSparse.msix"

Write-Host "`nPlease Install '$PSScriptRoot\WinForms\.user\WindowsAISampleForWinForms.certificate.sample.cer' to Local Machine/Trusted People Store before running"
Write-Host "`nTo run the sample, run: $PSScriptRoot\WinForms\bin\$Platform\$Configuration\net8.0-windows10.0.22621.0\WindowsAISampleForWinFormsSparse.exe"