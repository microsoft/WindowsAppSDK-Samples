// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using Microsoft.Win32;
using Microsoft.Windows.AI.MachineLearning;
using System.IO;
using System.Reflection;
using System.Text;
using System.Windows;
using System.Windows.Media.Imaging;
using WindowsML.Shared;

namespace WindowsMLSampleForWPF;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window, IDisposable
{
    private string? _selectedImagePath;
    private InferenceSession? _session;
    private OrtEnv? _ortEnv;
    private List<string> _labels = new();
    private bool _disposed = false;

    public MainWindow()
    {
        InitializeComponent();
        Loaded += MainWindow_Loaded;
        Closed += MainWindow_Closed;
    }

    private void MainWindow_Closed(object? sender, EventArgs e)
    {
        Dispose();
    }

    public void Dispose()
    {
        if (!_disposed)
        {
            _session?.Dispose();
            _ortEnv?.Dispose();
            _disposed = true;
        }
    }

    private async void MainWindow_Loaded(object sender, RoutedEventArgs e)
    {
        await InitializeAsync();
    }

    private async Task InitializeAsync()
    {
        try
        {
            // Display execution provider information first
            ResultsTextBox.Text = "Getting available providers...\n";

            // Get provider information directly from ExecutionProviderCatalog
            var catalog = ExecutionProviderCatalog.GetDefault();
            var providers = catalog.FindAllProviders();
            var providerInfo = new StringBuilder();
            providerInfo.AppendLine("Available Execution Providers:");
            providerInfo.AppendLine("========================================");

            foreach (var provider in providers)
            {
                providerInfo.AppendLine($"Provider: {provider.Name}");
                providerInfo.AppendLine($"  Ready state: {provider.ReadyState}");
                providerInfo.AppendLine();
            }

            providerInfo.AppendLine("========================================");
            providerInfo.AppendLine("Loading model...");
            ResultsTextBox.Text = providerInfo.ToString();

            await LoadModelAndLabelsAsync();
            ResultsTextBox.Text += "\nModel loaded successfully. Select an image and click 'Run Inference' to see classification results.";
        }
        catch (Exception ex)
        {
            ResultsTextBox.Text = $"Failed to initialize: {ex.Message}";
        }
    }

    private async Task LoadModelAndLabelsAsync()
    {
        // Use checkbox value for provider downloads
        bool allowDownload = AllowProviderDownloadCheckBox.IsChecked ?? false;

        // Create ONNX Runtime environment using shared helper
        _ortEnv = ModelManager.CreateEnvironment("WindowsMLSampleForWPF");

        // Initialize execution providers using shared helper  
        await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

        // Get model and labels paths using shared helper
        var options = new Options { ModelPath = "SqueezeNet.onnx" };
        var (modelPath, compiledModelPath, labelsPath) = ModelManager.ResolvePaths(options);

        // Create inference session using shared helper
        string actualModelPath = ModelManager.ResolveActualModelPath(options, modelPath, compiledModelPath, _ortEnv);
        _session = ModelManager.CreateSession(actualModelPath, options, _ortEnv);

        // Load labels using shared helper
        _labels = ResultProcessor.LoadLabels(labelsPath).ToList();
    }

    private void SelectImageButton_Click(object sender, RoutedEventArgs e)
    {
        var openFileDialog = new OpenFileDialog {
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

            // Load and preprocess the image using shared helper
            var videoFrame = await ImageProcessor.LoadImageFileAsync(_selectedImagePath);
            var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

            // Run inference using shared helper
            using var results = InferenceEngine.RunInference(_session, inputTensor);
            var resultTensor = InferenceEngine.ExtractResults(_session, results);

            // Process and display results using shared helper
            var topPredictions = ResultProcessor.GetTopPredictions(resultTensor, _labels, 5);
            var formattedResults = FormatResultsForUI(topPredictions);
            ResultsTextBox.Text = formattedResults;
        }
        catch (Exception ex)
        {
            ResultsTextBox.Text = $"Error during inference: {ex.Message}";
        }
    }

    private string FormatResultsForUI(List<(string Label, float Confidence)> topPredictions)
    {
        var sb = new StringBuilder();
        sb.AppendLine("Top 5 Predictions:");
        sb.AppendLine("-------------------------------------------");
        sb.AppendLine($"{"Label",-32} {"Confidence",10}");
        sb.AppendLine("-------------------------------------------");

        foreach (var (label, confidence) in topPredictions)
        {
            sb.AppendLine($"{label,-32} {confidence:P2}");
        }

        sb.AppendLine("-------------------------------------------");
        return sb.ToString();
    }
}
