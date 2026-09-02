# Install dependencies for SqueezeNetPython sample

Set-StrictMode -Version 2.0
$ErrorActionPreference = 'Stop'

# The Windows ML package brings the matching stable ONNX Runtime dependency.
& python -m pip install --upgrade -r (Join-Path $PSScriptRoot 'requirements.txt')
if ($LASTEXITCODE -ne 0)
{
    throw "Dependency installation failed with exit code $LASTEXITCODE."
}

exit 0
