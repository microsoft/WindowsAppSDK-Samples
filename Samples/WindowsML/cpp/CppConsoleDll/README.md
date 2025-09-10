# CppConsoleDll Sample

This sample demonstrates how to use WindowsML APIs from within a DLL that is consumed by a separate executable. This pattern is particularly useful when you need to share WindowsML functionality across multiple applications or isolate ML functionality in a separate module.

## Overview

The sample consists of two projects:

1. **WindowsMLWrapper.dll** - A Dynamic Link Library that:
   - Uses the WindowsAppSDK with ML support
   - Exports C-style functions that can be easily consumed by other applications
   - Demonstrates proper string memory management using CoTaskMemAlloc
   - Shows how to use ONNX Runtime APIs from WindowsAppSDK

2. **ConsoleClient.exe** - A minimal console application that:
   - Dynamically loads the WindowsMLWrapper.dll
   - Calls the exported functions
   - Properly manages memory allocated by the DLL
   - Demonstrates error handling for DLL loading and function calls

## Key Features

- **WindowsAppSdkBootstrapInitialize**: The DLL project uses `WindowsAppSdkBootstrapInitialize=true` to ensure proper initialization of the WindowsAppSDK runtime when loaded
- **Memory Management**: Exported functions use CoTaskMemAlloc to allocate strings, which are freed by the client using CoTaskMemFree
- **Error Handling**: Both the DLL and client demonstrate proper error handling patterns
- **Minimal Dependencies**: The client application has minimal dependencies and only links to what it needs

## Building

Both projects are included in the main WindowsML-Samples.sln solution. When you build the solution:

1. WindowsMLWrapper.dll is built first
2. ConsoleClient.exe is built with a dependency on the DLL project
3. The DLL is automatically copied to the client's output directory

## Running

After building, you can run ConsoleClient.exe directly. It will:

1. Load WindowsMLWrapper.dll from the same directory
2. Call the exported functions
3. Display the results in the console
4. Clean up and exit

Expected output:
```
Calling GetTestMessage from DLL...
Test message: WindowsML DLL is working correctly!
Calling GetOrtVersionString from DLL...
ONNX Runtime version: [ONNX Runtime version string]
DLL functions called successfully!
```

## Architecture Notes

This pattern is useful in scenarios such as:
- Creating reusable ML components across multiple applications
- Isolating ML functionality in a separate process space
- Building plugin architectures where ML capabilities are loaded dynamically
- Separating concerns between UI and ML processing components

The DLL handles all WindowsAppSDK initialization and cleanup, while the client application can be kept lightweight and focused on its primary functionality.
