# Windows ML WPF Image Classification Sample

This sample demonstrates how to use Windows ML in a **WPF (Windows Presentation Foundation)** desktop application for real-time image classification. The application provides a user-friendly interface for selecting images, configuring execution providers, and viewing classification results.

## Features

- **Image File Selection** - Browse and select images for classification using standard file picker
- **Execution Provider Configuration** - View available providers (NPU, GPU, CPU) and configure download settings
- **Real-time Inference** - Classify images using the SqueezeNet model with immediate results
- **Results Display** - Show top 5 predictions with confidence percentages in formatted output
- **Provider Information** - Display detailed execution provider status and capabilities

## Key Technologies

- **WPF** - Windows Presentation Foundation for desktop UI
- **Windows ML** - ONNX Runtime integration via Windows App SDK
- **Shared Helpers** - Uses `WindowsML.Shared` library for consistent ML operations

## Building and Running

1. Open the main solution: `WindowsML-Samples.sln`
2. Set the `WindowsMLSampleForWPF` project as the startup project
3. Build and run (F5)

## Usage

1. **Launch Application** - The app loads and displays available execution providers
2. **Configure Providers** - Optionally check "Allow Provider Download" to enable automatic EP downloads
3. **Select Image** - Click "Select Image" to browse for an image file (.jpg, .jpeg, .png)
4. **Run Classification** - Click "Run Inference" to classify the selected image
5. **View Results** - See the top 5 predictions with confidence percentages

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
var videoFrame = await ImageProcessor.LoadImageFileAsync(_selectedImagePath);
var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

// Run inference
using var results = InferenceEngine.RunInference(_session, inputTensor);
var resultTensor = InferenceEngine.ExtractResults(_session, results);

// Process results
var topPredictions = ResultProcessor.GetTopPredictions(resultTensor, _labels, 5);
```

## Related Samples

- **WinForms Version**: [cs-winforms](../cs-winforms/) - Same functionality with Windows Forms UI
- **WinUI 3 Version**: [cs-winui](../cs-winui/) - Modern WinUI 3 packaged application
- **Console Version**: [cs/CSharpConsoleDesktop](../cs/CSharpConsoleDesktop/) - Command-line interface

## Model Information

This sample uses the **SqueezeNet** model:

- **Purpose**: Image classification with 1000 categories
- **Input**: 224x224 RGB images
- **Output**: Probability distribution over ImageNet classes
- **Size**: Lightweight model optimized for performance
