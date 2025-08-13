---
page_type: sample
languages:
- python
products:
- windows-app-sdk
name: "WindowsML python"
urlFragment: WindowsMLPython
description: "Shows how to use WindowsML and ONNX Runtime to write python machine learning projects that runs across Windows AI hardware."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
* Python 3.10 - 3.13 on Windows
* Download model file
```PowerShell
..\Download-Model.ps1
```
* Install dependencies
```PowerShell
pip install -r requirements.txt
```
Please also install the provided wheel files. As those are not published to a public source.
* Install WindowsAppRuntime

Please install the correct WindowsAppRuntime with the provided installer.
