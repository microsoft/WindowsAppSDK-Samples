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
* Ensure that your Python installation is not from the Microsoft Store (you can install an unpackaged version from python.org or via winget). The sample depends on using the Windows App SDK dynamic dependency API, which is only valid for unpackaged apps.
* Install the latest WinML python packages
```PowerShell
.\Install-Requirements.ps1
```
The pinned Windows ML 2.3 package includes the compiled-model compatibility
APIs used by this sample.
### Install WindowsAppRuntime
Please install the WindowsAppRuntime that matches the version of the python package `wasdk-Microsoft.Windows.ApplicationModel.DynamicDependency.Bootstrap`
> For experimental or preview WASDK. The version tags `-xxxN` are changed to `.devN` to fit Python's version requirements.
### Run the example
```PowerShell
python main.py
```

### Compiled model compatibility

The sample uses the `PREFER_NPU` execution provider device policy and caches the
compiled model as `Model\SqueezeNet_ctx.onnx`. Before reusing that file, it:

1. Calls `get_ep_devices` and groups the returned devices by execution provider.
   Devices from different execution providers are never passed together.
2. For the `PREFER_NPU` policy, checks same-execution-provider groups containing
   NPU and CPU devices because policy selection can fall back to CPU.
3. Reads each relevant execution provider's metadata with
   `get_compatibility_info_from_model` and validates it with
   `get_model_compatibility_for_ep_devices`.
4. Requires at least one candidate execution provider group to have matching
   metadata, and reuses the cached model only when every metadata-bearing group
   reports `OrtCompiledModelCompatibility.EP_SUPPORTED_OPTIMAL`. A non-optimal
   result from any applicable group rejects the cache.

If the cached model is missing, has no matching metadata, or is not optimal, the
sample compiles the original model in a temporary directory with EP context
embedded in the ONNX file. It verifies that no external sidecars were produced,
and the temporary model must also report `EP_SUPPORTED_OPTIMAL` before it
replaces the cache. If compilation or validation fails, temporary outputs are
removed and inference uses the original `SqueezeNet.onnx`; an existing stale
compiled model is never used for that run.
