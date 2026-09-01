---
page_type: sample
languages:
- python
products:
- windows-app-sdk
name: "WindowsML python"
urlFragment: WindowsMLPython
description: >-
  Shows how to use Windows ML and ONNX Runtime to write Python machine learning
  applications that run across Windows AI hardware.
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Steps

### Download the test model

```powershell
..\Download-Model.ps1
```

### Prepare the Python environment

- Create a Python environment with Python 3.10 through 3.13.
- Ensure that Python is not installed from the Microsoft Store. Install an
  unpackaged version from [python.org](https://www.python.org/) or with
  `winget`. The Windows App SDK dynamic dependency API used by this sample is
  only valid for unpackaged apps.
- Install the stable Windows ML 2.1.3 Python packages:

```powershell
.\Install-Requirements.ps1
```

### Install Windows App Runtime

Install Windows App Runtime 2.1.3 to match
`wasdk-Microsoft.Windows.ApplicationModel.DynamicDependency.Bootstrap`.

### Run the example

```powershell
python main.py
```
