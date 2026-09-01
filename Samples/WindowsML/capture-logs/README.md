# Capturing Windows ML Logs

This folder contains the scripts and profiles needed to capture Windows ML
diagnostic logs. For full instructions, see
[Capture Windows ML logs][logs] on Microsoft Learn.

[logs]: https://learn.microsoft.com/windows/ai/new-windows-ml/logs

Install Windows Performance Toolkit before running the script. The script does
not install or modify system components.

## Files

- [Get-WinMLRundown.ps1](Get-WinMLRundown.ps1): Generates a rundown log from
  an ETL file.
- [WindowsMLProfile.wpaProfile](WindowsMLProfile.wpaProfile): Defines the WPA
  tables exported from the ETL file.
- [WinML.wprp](WinML.wprp): Captures Windows ML diagnostic data with WPR.

Temporary CSV files are written to a unique folder beneath `OutputFolder`.
Only that temporary folder is removed after processing.