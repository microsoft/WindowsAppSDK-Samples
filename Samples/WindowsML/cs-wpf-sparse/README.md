---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: WindowsML WPF Sparse Sample
description: Shows how to use Windows ML APIs for image classification with SqueezeNet in a WPF application (sparse/unpackaged)
urlFragment: cs-wpf-sparse-windowsml
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Windows ML WPF Sample (Sparse/Unpackaged)

This sample demonstrates how to use Windows ML APIs to perform image classification using the SqueezeNet model in a WPF application using sparse packaging (external location).

## Prerequisites

- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Visual Studio 2022 with the following workloads:
  - .NET desktop development
- MakeAppx.exe (part of Windows SDK)

## Build and Run the Sample

### Option 1: Build with Scripts
1. Clone the repository.
2. Open a Developer Command Prompt for Visual Studio 2022.
3. Navigate to the `cs-wpf-sparse` folder.
4. Run the build script:
   ```cmd
   BuildSparsePackage.cmd
   ```
   Or with PowerShell:
   ```powershell
   .\BuildSparsePackage.ps1
   ```
5. Install the generated `.msix` package.

### Option 2: Build with Visual Studio
1. Clone the repository.
2. Open the solution file `WindowsMLSampleForWPF.sln` in Visual Studio 2022.
3. Change the Solution Platform to match your system architecture (x64 or ARM64).
4. Right-click on the solution in Solution Explorer and select "Build" to build the solution.
5. Once the build is successful, press F5 or select "Start Debugging" from the Debug menu to run the sample.

Note: The sparse version demonstrates unpackaged deployment but still requires package identity for Windows ML APIs.

## Sample Overview

The `MainWindow` class demonstrates how to use Windows ML APIs to:

- Load and display images selected by the user in a WPF Image control
- Load the SqueezeNet ONNX model for image classification
- Preprocess images to the required input format (224x224, normalized)
- Run inference using the model
- Display the top 5 classification results with confidence scores in a formatted TextBox

### Key Features

- **Select Image**: Opens a WPF file dialog to select an image file (JPG, JPEG, PNG)
- **Run Inference**: Processes the selected image through the SqueezeNet model and displays classification results
- **WPF Native Controls**: Uses WPF Image, TextBox, and Button controls for a native Windows experience
- **Sparse Packaging**: Demonstrates external location deployment while maintaining package identity

### Key Methods

- **LoadModelAndLabelsAsync**: Initializes the ONNX Runtime session and loads classification labels
- **PreprocessImageAsync**: Converts and resizes images to the required 224x224 format with proper normalization
- **ProcessResults**: Applies softmax to model outputs and formats the top predictions

## Special Considerations for Sparse Packaging

- The application uses sparse packaging to maintain package identity while being deployed from an external location
- The `AppxManifest.xml` file provides the necessary package identity
- Build scripts automate the creation of the sparse MSIX package

## Model Information

This sample uses the SqueezeNet model, a lightweight convolutional neural network designed for image classification. The model:

- Input: 224x224 RGB images
- Output: 1000-class ImageNet classification
- Preprocessing: Normalization with mean=[0.485, 0.456, 0.406] and std=[0.229, 0.224, 0.225]

## Related Links

- [Windows ML APIs Documentation](https://docs.microsoft.com/windows/ai/windows-ml/)
- [ONNX Runtime Documentation](https://onnxruntime.ai/)
- [SqueezeNet Paper](https://arxiv.org/abs/1602.07360)
- [Grant package identity to unpackaged apps](https://docs.microsoft.com/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps)
