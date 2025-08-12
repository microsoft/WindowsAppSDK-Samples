using System.IO;
using System.Reflection;
using System.Text;
using System.Windows;
using System.Windows.Media.Imaging;
using Microsoft.Win32;
using Microsoft.Windows.AI.MachineLearning;
using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Storage;
using Windows.Storage.Streams;

namespace WindowsMLSampleForWPF;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    private string? _selectedImagePath;
    private InferenceSession? _session;
    private List<string> _labels = new();

    public MainWindow()
    {
        InitializeComponent();
        InitializeAsync();
    }

    private async void InitializeAsync()
    {
        try
        {
            await LoadModelAndLabelsAsync();
            ResultsTextBox.Text = "Model loaded successfully. Select an image and click 'Run Inference' to see classification results.";
        }
        catch (Exception ex)
        {
            ResultsTextBox.Text = $"Failed to initialize model: {ex.Message}";
        }
    }

    private async Task LoadModelAndLabelsAsync()
    {
        // Register execution providers
        var catalog = ExecutionProviderCatalog.GetDefault();
        var registeredProviders = await catalog.EnsureAndRegisterAllAsync();

        // Load the ONNX model
        string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;
        string modelPath = Path.Combine(executableFolder, "SqueezeNet.onnx");
        string labelsPath = Path.Combine(executableFolder, "SqueezeNet.Labels.txt");

        if (!File.Exists(modelPath))
        {
            throw new FileNotFoundException($"Model file not found: {modelPath}");
        }

        if (!File.Exists(labelsPath))
        {
            throw new FileNotFoundException($"Labels file not found: {labelsPath}");
        }

        // Create session options
        SessionOptions sessionOptions = new();

        // Configure execution providers (simplified for this sample)
        var ortEnv = OrtEnv.CreateInstanceWithOptions(new EnvironmentCreationOptions
        {
            logId = "WindowsMLWPFSample",
            logLevel = OrtLoggingLevel.ORT_LOGGING_LEVEL_WARNING
        });

        // Create inference session
        _session = new InferenceSession(modelPath, sessionOptions);

        // Load labels
        _labels = LoadLabels(labelsPath);
    }

    private List<string> LoadLabels(string labelsPath)
    {
        return File.ReadAllLines(labelsPath)
            .Select(line => line.Split(',', 2)[1])
            .ToList();
    }

    private void SelectImageButton_Click(object sender, RoutedEventArgs e)
    {
        var openFileDialog = new OpenFileDialog
        {
            Title = "Select Image File",
            Filter = "Image files (*.jpg, *.jpeg, *.png)|*.jpg;*.jpeg;*.png",
            FilterIndex = 1
        };

        if (openFileDialog.ShowDialog() == true)
        {
            _selectedImagePath = openFileDialog.FileName;
            ImagePathTextBox.Text = Path.GetFileName(_selectedImagePath);

            // Display the selected image
            try
            {
                var bitmap = new BitmapImage();
                bitmap.BeginInit();
                bitmap.UriSource = new Uri(_selectedImagePath);
                bitmap.EndInit();
                SelectedImage.Source = bitmap;
                
                RunInferenceButton.IsEnabled = true;
                ResultsTextBox.Text = "Image selected. Click 'Run Inference' to classify the image.";
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error loading image: {ex.Message}", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }

    private async void RunInferenceButton_Click(object sender, RoutedEventArgs e)
    {
        if (string.IsNullOrEmpty(_selectedImagePath) || _session == null)
        {
            ResultsTextBox.Text = "Please select an image first.";
            return;
        }

        try
        {
            ResultsTextBox.Text = "Running inference...";
            
            // Force UI update
            Dispatcher.Invoke(() => { }, System.Windows.Threading.DispatcherPriority.Render);

            // Load and preprocess the image
            var videoFrame = await LoadImageFileAsync(_selectedImagePath);
            var inputTensor = await PreprocessImageAsync(videoFrame);

            // Run inference
            var inputName = _session.InputMetadata.First().Key;
            var inputs = new List<NamedOnnxValue>
            {
                NamedOnnxValue.CreateFromTensor(inputName, inputTensor)
            };

            using var results = _session.Run(inputs);
            var outputName = _session.OutputMetadata.First().Key;
            var resultTensor = results.First(r => r.Name == outputName).AsEnumerable<float>().ToArray();

            // Process and display results
            var formattedResults = ProcessResults(_labels, resultTensor);
            ResultsTextBox.Text = formattedResults;
        }
        catch (Exception ex)
        {
            ResultsTextBox.Text = $"Error during inference: {ex.Message}";
        }
    }

    private async Task<VideoFrame> LoadImageFileAsync(string filePath)
    {
        var file = await StorageFile.GetFileFromPathAsync(filePath);
        using var stream = await file.OpenAsync(FileAccessMode.Read);
        var decoder = await BitmapDecoder.CreateAsync(stream);
        var softwareBitmap = await decoder.GetSoftwareBitmapAsync();
        return VideoFrame.CreateWithSoftwareBitmap(softwareBitmap);
    }

    private async Task<DenseTensor<float>> PreprocessImageAsync(VideoFrame videoFrame)
    {
        var softwareBitmap = videoFrame.SoftwareBitmap;
        const int targetWidth = 224;
        const int targetHeight = 224;

        float[] mean = [0.485f, 0.456f, 0.406f];
        float[] std = [0.229f, 0.224f, 0.225f];

        // Convert to BGRA8
        if (softwareBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8 ||
            softwareBitmap.BitmapAlphaMode != BitmapAlphaMode.Premultiplied)
        {
            softwareBitmap = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
        }

        // Resize
        softwareBitmap = await ResizeSoftwareBitmapAsync(softwareBitmap, targetWidth, targetHeight);

        // Get pixel data
        uint bufferSize = (uint)(targetWidth * targetHeight * 4);
        var buffer = new Windows.Storage.Streams.Buffer(bufferSize);
        softwareBitmap.CopyToBuffer(buffer);
        byte[] pixels = buffer.ToArray();

        // Output Tensor shape: [1, 3, 224, 224]
        var tensorData = new DenseTensor<float>(new[] { 1, 3, targetHeight, targetWidth });

        for (int y = 0; y < targetHeight; y++)
        {
            for (int x = 0; x < targetWidth; x++)
            {
                int pixelIndex = (y * targetWidth + x) * 4;
                float r = pixels[pixelIndex + 2] / 255f;
                float g = pixels[pixelIndex + 1] / 255f;
                float b = pixels[pixelIndex + 0] / 255f;

                // Normalize using mean/stddev
                r = (r - mean[0]) / std[0];
                g = (g - mean[1]) / std[1];
                b = (b - mean[2]) / std[2];

                tensorData[0, 0, y, x] = r; // R
                tensorData[0, 1, y, x] = g; // G
                tensorData[0, 2, y, x] = b; // B
            }
        }

        return tensorData;
    }

    private async Task<SoftwareBitmap> ResizeSoftwareBitmapAsync(SoftwareBitmap bitmap, int width, int height)
    {
        using var stream = new InMemoryRandomAccessStream();
        var encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
        encoder.SetSoftwareBitmap(bitmap);
        encoder.IsThumbnailGenerated = false;
        await encoder.FlushAsync();
        stream.Seek(0);

        var decoder = await BitmapDecoder.CreateAsync(stream);
        var transform = new BitmapTransform
        {
            ScaledWidth = (uint)width,
            ScaledHeight = (uint)height,
            InterpolationMode = BitmapInterpolationMode.Fant
        };

        var resized = await decoder.GetSoftwareBitmapAsync(
            BitmapPixelFormat.Bgra8,
            BitmapAlphaMode.Premultiplied,
            transform,
            ExifOrientationMode.IgnoreExifOrientation,
            ColorManagementMode.DoNotColorManage);

        return resized;
    }

    private string ProcessResults(List<string> labels, float[] results)
    {
        // Apply softmax to the results
        float maxLogit = results.Max();
        var expScores = results.Select(r => MathF.Exp(r - maxLogit)).ToArray();
        float sumExp = expScores.Sum();
        var softmaxResults = expScores.Select(e => e / sumExp).ToArray();

        // Get top 5 results
        var topResults = softmaxResults
            .Select((value, index) => new { Index = index, Confidence = value })
            .OrderByDescending(x => x.Confidence)
            .Take(5);

        var sb = new StringBuilder();
        sb.AppendLine("Top 5 Predictions:");
        sb.AppendLine("-------------------------------------------");
        sb.AppendLine($"{"Label",-32} {"Confidence",10}");
        sb.AppendLine("-------------------------------------------");

        foreach (var result in topResults)
        {
            sb.AppendLine($"{labels[result.Index],-32} {result.Confidence:P2}");
        }

        sb.AppendLine("-------------------------------------------");
        return sb.ToString();
    }
}
