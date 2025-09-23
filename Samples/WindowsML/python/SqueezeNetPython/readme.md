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

## Steps
### Download test model
```PowerShell
..\Download-Model.ps1
```
### Prepare the python environment
* Create a python environment with Python 3.10 - 3.13
* Install the latest WinML python packages
```PowerShell
pip install --upgrade --pre -r requirements.txt 
```
### Install WindowsAppRuntime
Please install the WindowsAppRuntime that matches the version of the python package `wasdk-Microsoft.Windows.ApplicationModel.DynamicDependency.Bootstrap`
> For experimental or preview WASDK. The version tags `-xxxN` are changed to `.devN` to fit Python's version requirements.
### Run the example
```PowerShell
python main.py
```
