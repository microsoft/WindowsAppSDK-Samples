# Run elevated

param(
  [Parameter(Mandatory=$true)] [string] $RegFile,
  [Parameter(Mandatory=$true)] [string] $OutDat
)

$Scratch = 'HKLM\Software\_TempForHive'

# 1) Prepare scratch
reg.exe add "$Scratch" /f | Out-Null

# 2) Import .reg (must contain HKLM\Software\... paths)
reg.exe import "$RegFile"
if ($LASTEXITCODE -ne 0) { throw "reg import failed: $LASTEXITCODE" }

# (Optional) If your .reg wrote *outside* the scratch subtree,
# move the imported keys beneath $Scratch before saving.

# 3) Save the hive
reg.exe save "$Scratch" "$OutDat" /y
if ($LASTEXITCODE -ne 0) { throw "reg save failed: $LASTEXITCODE" }

# 4) Cleanup
reg.exe delete "$Scratch" /f | Out-Null

