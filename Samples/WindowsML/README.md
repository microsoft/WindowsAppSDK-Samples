# Windows ML Samples

This folder contains samples demonstrating how to use Windows ML APIs for machine learning inference on Windows.

## Samples Overview

All samples use the SqueezeNet model for image classification, demonstrating the same core functionality in different UI frameworks:

### Console Samples
- **[cpp/CppConsoleDesktop](cpp/CppConsoleDesktop/)** - C++ console application
- **[cs/CSharpConsoleDesktop](cs/CSharpConsoleDesktop/)** - C# console application  
- **[python](python/)** - Python console application

### UI Samples
- **[cs-winui](cs-winui/)** - C# WinUI 3 application with modern UI
- **[cs-winforms](cs-winforms/)** - C# Windows Forms application
- **[cs-wpf](cs-wpf/)** - C# WPF application (packaged)
- **[cs-wpf-sparse](cs-wpf-sparse/)** - C# WPF application (sparse/unpackaged)
- **[cpp-winui](cpp-winui/)** - C++ WinUI 3 application

## Common Resources

The **[Resources](Resources/)** folder contains shared files used by all samples:
- `SqueezeNet.onnx` - The SqueezeNet ONNX model file
- `SqueezeNet.Labels.txt` - ImageNet class labels
- `image.jpg` - Sample image for testing

## Model Information

All samples use SqueezeNet, a lightweight convolutional neural network for image classification:
- **Input**: 224x224 RGB images
- **Output**: 1000-class ImageNet classification probabilities
- **Preprocessing**: Normalization with mean=[0.485, 0.456, 0.406] and std=[0.229, 0.224, 0.225]

## Prerequisites

- Visual Studio 2022
- Windows 10 version 1809 or later / Windows 11
- .NET 9.0 SDK (for C# samples)
- Windows App SDK 1.8 (experimental)

## Getting Started

1. Choose a sample that matches your preferred UI framework
2. Open the solution file in Visual Studio 2022
3. Set the platform to match your system (x64 or ARM64)
4. Build and run the sample

Each sample provides the same core functionality:
- Select an image file (JPG, PNG)
- Run SqueezeNet inference on the image
- Display the top 5 classification results with confidence scores

## Related Documentation

- [Windows ML APIs](https://docs.microsoft.com/windows/ai/windows-ml/)
- [ONNX Runtime](https://onnxruntime.ai/)
- [SqueezeNet Paper](https://arxiv.org/abs/1602.07360)
