function Get-UserPath
{
    $root = Join-Path (Get-Item $PSScriptRoot ).FullName "WCRforWPF"
    $user = Join-Path $root '.user'
    if (-not(Test-Path -Path $user -PathType Container))
    {
        Write-Host "Creating $user..."
        $null = New-Item -Path $user -ItemType Directory -Force
    }
    return $user
}

$user_path = Get-UserPath
$pwd_file = Join-Path $user_path 'wcrforwpf.certificate.test.pwd'
$cert_thumbprint = Join-Path $user_path 'wcrforwpf.certificate.test.thumbprint'
$cer = Join-Path $user_path 'wcrforwpf.certificate.test.cer'
$pfx = Join-Path $user_path 'wcrforwpf.certificate.test.pfx'

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
    $subject = 'CN=Windows App SDK Samples'
    $friendly_name = "Microsoft.WindowsAppSDK Test Certificate Create=$now"
    $key_friendly_name = "Microsoft.WindowsAppSDK Test PrivateKey Create=$now"
    $key_description = "Microsoft.WindowsAppSDK Test PrivateKey Create=$now"
    $eku_oid = '2.5.29.37'
    $eku_value = '1.3.6.1.5.5.7.3.3,1.3.6.1.4.1.311.10.3.13'
    $eku = "$eku_oid={text}$eku_value"
    $cert = New-SelfSignedCertificate -CertStoreLocation $cert_path -NotAfter $expiration -Subject $subject -FriendlyName $friendly_name -KeyFriendlyName $key_friendly_name -KeyDescription $key_description -TextExtension $eku
    
    # Save the thumbprint
    $thumbprint = $cert.Thumbprint
    Set-Content -Path $cert_thumbprint -Value $thumbprint -Force
    
    # Export the certificate
    $cer = Join-Path $user_path 'wcrforwpf.certificate.test.cer'
    $export_cer = Export-Certificate -Cert $cert -FilePath $cer -Force
    $cert_personal = "cert:\CurrentUser\My\$thumbprint"
    $pfx = Join-Path $user_path 'wcrforwpf.certificate.test.pfx'
    $export_pfx = Export-PfxCertificate -Cert $cert_personal -FilePath $pfx -Password $password    
}

dotnet build /p:platform=x64