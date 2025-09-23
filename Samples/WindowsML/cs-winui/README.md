# Windows ML WinUI 3 Image Classification Sample

This sample demonstrates how to use Windows ML in a **WinUI 3** packaged or unpackaged application for image classification. The application showcases Windows UI design with automatic demo functionality and execution provider configuration.

## Features

- **Automatic Demo Mode** - Loads and classifies included sample image on startup
- **Execution Provider Configuration** - View available providers (NPU, GPU, CPU) and configure download settings
- **Real-time Inference** - Classify images using the SqueezeNet model with immediate results
- **Results Display** - Show top 5 predictions with confidence percentages
- **Model Reloading** - Reload model with different provider settings
- **Modern UI** - Clean WinUI 3 interface with contemporary design elements

## Key Technologies

- **WinUI 3** - Windows UI framework for packaged applications
- **Windows ML** - ONNX Runtime integration via Windows App SDK
- **Shared Helpers** - Uses `WindowsML.Shared` library for consistent ML operations
- **Packaged Deployment** - MSIX deployment model with app manifest

## Building and Running

1. Open the main solution: `WindowsML-Samples.sln`
2. Set the `WindowsMLSample` project as the startup project
3. Build and run (F5)

## Usage

1. **Launch Application** - The app automatically loads and displays execution provider information
2. **Auto-Demo** - Sample image (`image.jpg`) is automatically loaded and classified on startup
3. **Configure Providers** - Use "Allow Provider Download" checkbox to enable automatic EP downloads
4. **Reload Model** - Click "Reload Model" to reinitialize with new provider settings
5. **View Results** - See the top 5 predictions displayed automatically

## Expected Output

```
Top 5 Predictions:
-------------------------------------------
Label                            Confidence
-------------------------------------------
French bulldog                       45.07%
bull mastiff                         35.05%
boxer                                 2.85%
pug                                   1.01%
American Staffordshire terrier        0.57%
-------------------------------------------
```

## Code Structure

The sample uses the shared `WindowsML.Shared` library for consistency across samples:

```csharp
// Initialize execution providers
var catalog = ExecutionProviderCatalog.GetDefault();
await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

// Load and preprocess image
using var videoFrame = await ImageProcessor.LoadImageFileAsync(imagePath);
var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

// Run inference
using var results = InferenceEngine.RunInference(_session, inputTensor);
var output = InferenceEngine.ExtractResults(_session, results);

// Process results
var topResults = ResultProcessor.GetTopPredictions(output, _labels, 5);
```

## Related Samples

- **WPF Version**: [cs-wpf](../cs-wpf/) - Same functionality with WPF UI framework
- **WinForms Version**: [cs-winforms](../cs-winforms/) - Windows Forms application
- **Console Version**: [cs/CSharpConsoleDesktop](../cs/CSharpConsoleDesktop/) - Command-line interface

## Model Information

This sample uses the **SqueezeNet** model:

- **Purpose**: Image classification with 1000 categories
- **Input**: 224x224 RGB images
- **Output**: Probability distribution over ImageNet classes
- **Size**: Lightweight model optimized for performance

## Deployment

The application can be deployed as:

- **Development**: Direct run from Visual Studio
- **Sideloading**: MSIX package for deployment to other devices
