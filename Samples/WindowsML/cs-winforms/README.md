# Windows ML WinForms Image Classification Sample

This sample demonstrates how to use Windows ML in a **Windows Forms** desktop application for real-time image classification. The application provides a familiar Windows Forms interface for selecting images, configuring execution providers, and viewing classification results.

## Features

- **Image File Selection** - Browse and select images for classification using standard OpenFileDialog
- **Execution Provider Configuration** - View available providers (NPU, GPU, CPU) and configure download settings
- **Real-time Inference** - Classify images using the SqueezeNet model with immediate results
- **Results Display** - Show top 5 predictions with confidence percentages in formatted output
- **Image Preview** - Display selected image in PictureBox control
- **Provider Information** - Display detailed execution provider status and capabilities

## Key Technologies

- **Windows Forms** - Windows .NET-based desktop UI framework
- **Windows ML** - ONNX Runtime integration via Windows App SDK
- **Shared Helpers** - Uses `WindowsML.Shared` library for consistent ML operations

## Building and Running

1. Open the main solution: `WindowsML-Samples.sln`
2. Set the `WindowsMLWinFormsSample` project as the startup project
3. Build and run (F5)

## Usage

1. **Launch Application** - The app loads and displays available execution providers
2. **Configure Providers** - Optionally check "Allow Provider Download" to enable automatic EP downloads
3. **Select Image** - Click "Select Image" to browse for an image file (.jpg, .jpeg, .png)
4. **Preview Image** - Selected image appears in the picture box
5. **Run Classification** - Click "Run Inference" to classify the selected image
6. **View Results** - See the top 5 predictions with confidence percentages in the text area

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
using var videoFrame = await ImageProcessor.LoadImageFileAsync(_selectedImagePath);
var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

// Run inference
using var results = InferenceEngine.RunInference(_session, inputTensor);
var resultTensor = InferenceEngine.ExtractResults(_session, results);

// Process results
var topPredictions = ResultProcessor.GetTopPredictions(resultTensor, _labels, 5);
```

## Related Samples

- **WPF Version**: [cs-wpf](../cs-wpf/) - Same functionality with WPF UI framework
- **WinUI 3 Version**: [cs-winui](../cs-winui/) - WinUI 3 packaged application
- **Console Version**: [cs/CSharpConsoleDesktop](../cs/CSharpConsoleDesktop/) - Command-line interface

## Model Information

This sample uses the **SqueezeNet** model:

- **Purpose**: Image classification with 1000 categories
- **Input**: 224x224 RGB images
- **Output**: Probability distribution over ImageNet classes
- **Size**: Lightweight model optimized for performance

