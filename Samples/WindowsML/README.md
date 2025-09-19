# Windows ML Samples

This repository contains comprehensive samples demonstrating how to use [Windows ML](https://learn.microsoft.com/en-us/windows/ai/new-windows-ml/overview) and [ONNX Runtime](https://onnxruntime.ai/) for machine learning inference on Windows. These samples show integration across different programming languages, UI frameworks, and Windows App SDK deployment scenarios.

## Overview

Windows ML enables high-performance, reliable inferencing of machine learning models on Windows devices. These samples demonstrate key concepts including:

- **Execution Provider Selection** - Automatic discovery and acquisition of execution providers for hardware-accelerated inference
- **Model Compilation** - Optimize models for specific hardware during first run
- **Windows App SDK Deployment Types** - Use models in a variety of different Windows App SDK deployment modes (e.g., self-contained, framework-based deployment)

## Prerequisites

- **Windows 11 PC** running version 24H2 (build 26100) or greater
- **Visual Studio 2022** with C++ and .NET workloads
- **Windows App SDK 1.8** or later
- **Python 3.10-3.13** for Python samples on x64 and ARM64 devices

## Sample Categories

### C++ Samples

| Sample | Description | Key Features |
|--------|-------------|--------------|
| [CppConsoleDesktop](cpp/CppConsoleDesktop/) | Basic C++ console application | EP discovery, command-line options, model compilation |
| [CppConsoleDesktop.FrameworkDependent](cpp/CppConsoleDesktop.FrameworkDependent/) | Framework-dependent deployment variant | Shared runtime, smaller deployment footprint |
| [CppConsoleDesktop.SelfContained](cpp/CppConsoleDesktop.SelfContained/) | Self-contained deployment variant | Standalone deployment, no runtime dependencies |
| [CppConsoleDll](cpp/CppConsoleDll/) | DLL usage pattern | WindowsML in shared library, memory management |
| [CppResnetBuildDemo](cpp/CppResnetBuildDemo/) | ResNet model demo from the [Windows ML session](https://www.youtube.com/watch?v=AQjOq8qSsbE) at Build 2025 | Model conversion, EP compilation, detailed tutorial |

### C# Samples

#### Console Applications
| Sample | Description | Key Features |
|--------|-------------|--------------|
| [CSharpConsoleDesktop](cs/CSharpConsoleDesktop/) | Basic C# console application | Shared helper usage, command-line interface |
| [ResnetBuildDemoCS](cs/ResnetBuildDemoCS/) | ResNet model demo from the [Windows ML session](https://www.youtube.com/watch?v=AQjOq8qSsbE) at Build 2025 | Model conversion, EP compilation, detailed tutorial |

#### GUI Applications
| Sample | UI Framework | Description |
|--------|-------------|-------------|
| [cs-wpf](cs-wpf/) | WPF | Image classification example |
| [cs-winforms](cs-winforms/) | Windows Forms | Image classification example |
| [cs-winui](cs-winui/) | WinUI 3 | Image classification example |

### Python Samples

| Sample | Description | Key Features |
|--------|-------------|--------------|
| [SqueezeNetPython](python/SqueezeNetPython/) | Python image classification | WinML Python bindings, batch image processing |

## Common Workflow

Most samples follow this pattern:

1. **Initialize Environment** - Create ONNX Runtime environment
2. **Register Execution Providers** - Discover and register available hardware accelerators
3. **Load Model** - Load ONNX model, optionally compile for target hardware
4. **Preprocess Input** - Convert images to model input format
5. **Run Inference** - Execute model and get predictions
6. **Process Results** - Apply softmax and display top predictions

## Model Files

Samples use these pre-trained models:

- **SqueezeNet** - Lightweight image classification (included)
- **ResNet-50** - High-accuracy image classification (requires [AI Toolkit](https://code.visualstudio.com/docs/intelligentapps/modelconversion) conversion)

## Hardware Support

These samples detect and utilize the CPU, GPU, and NPU.

## Getting Help

- [Windows ML Documentation](https://learn.microsoft.com/en-us/windows/ai/new-windows-ml/overview)
- [ONNX Runtime Documentation](https://onnxruntime.ai/docs/)
- [Windows App SDK Documentation](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
- [Report Issues](https://github.com/microsoft/WindowsAppSDK-Samples/issues)