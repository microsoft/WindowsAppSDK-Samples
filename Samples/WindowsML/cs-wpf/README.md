---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: WindowsML WPF Sample
description: Shows how to use Windows ML APIs for image classification with SqueezeNet in a WPF application (packaged)
urlFragment: cs-wpf-windowsml
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

# Windows ML WPF Sample (Packaged)

This sample demonstrates how to use Windows ML APIs to perform image classification using the SqueezeNet model in a WPF application with MSIX packaging.

## Prerequisites

- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Visual Studio 2022 with the following workloads:
  - .NET desktop development
  - MSIX Packaging Tools

## Build and Run the Sample

1. Clone the repository.
2. Open the solution file `WindowsMLSampleForWPF.sln` in Visual Studio 2022.
3. Change the Solution Platform to match your system architecture (x64 or ARM64).
4. Right-click on the **WindowsMLSampleForWPFPkg** packaging project in Solution Explorer and select "Set as Startup Project".
5. Right-click on the solution in Solution Explorer and select "Build" to build the solution.
6. Once the build is successful, press F5 or select "Start Debugging" from the Debug menu to run the sample.

Note: The sample can also be run without debugging by selecting "Start Without Debugging" from the Debug menu or Ctrl+F5.

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
- **MSIX Packaging**: Demonstrates proper packaging for distribution

### Key Methods

- **LoadModelAndLabelsAsync**: Initializes the ONNX Runtime session and loads classification labels
- **PreprocessImageAsync**: Converts and resizes images to the required 224x224 format with proper normalization
- **ProcessResults**: Applies softmax to model outputs and formats the top predictions

## Model Information

This sample uses the SqueezeNet model, a lightweight convolutional neural network designed for image classification. The model:

- Input: 224x224 RGB images
- Output: 1000-class ImageNet classification
- Preprocessing: Normalization with mean=[0.485, 0.456, 0.406] and std=[0.229, 0.224, 0.225]

## Related Links

- [Windows ML APIs Documentation](https://docs.microsoft.com/windows/ai/windows-ml/)
- [ONNX Runtime Documentation](https://onnxruntime.ai/)
- [SqueezeNet Paper](https://arxiv.org/abs/1602.07360)
- [Package and deploy your WPF app with MSIX](https://docs.microsoft.com/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/wpf-plus-winappsdk#package-and-deploy-your-wpf-app-with-msix)
