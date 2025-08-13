// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.Windows.AI.MachineLearning;
using System.Reflection;
using System.Text;
using Windows.Storage;
using WindowsML.Shared;

namespace WindowsMLSample
{
    public sealed partial class MainWindow : Window, IDisposable
    {
        private InferenceSession? _session;
        private OrtEnv? _ortEnv;
        private List<string> _labels = new();
        private bool _disposed = false;

        public MainWindow()
        {
            this.InitializeComponent();
            this.Activated += MainWindow_Activated;
        }

        private bool _hasInitialized = false;

        private async void MainWindow_Activated(object sender, WindowActivatedEventArgs e)
        {
            if (!_hasInitialized && e.WindowActivationState != WindowActivationState.Deactivated)
            {
                _hasInitialized = true;
                try
                {
                    await InitializeAndRunDemoAsync();
                }
                catch (Exception ex)
                {
                    ResultsText.Text = $"Error: {ex.Message}";
                }
            }
        }

        private async Task InitializeAndRunDemoAsync()
        {
            try
            {
                // Display execution provider information first
                ResultsText.Text = "Getting available providers...\n";

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
                ResultsText.Text = providerInfo.ToString();

                await LoadModelAsync();
                await LoadAndRunDemoAsync();
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Initialization failed: {ex.Message}";
            }
        }

        private async Task LoadModelAsync()
        {
            // Use checkbox value for provider downloads
            bool allowDownload = AllowProviderDownloadCheckBox.IsChecked ?? false;

            // Create ONNX Runtime environment using shared helper
            _ortEnv = ModelManager.CreateEnvironment("WindowsMLWinUISample");

            // Initialize execution providers using shared helper  
            await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

            // Get application directory and resolve paths using shared helper
            var appDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            if (string.IsNullOrEmpty(appDirectory))
            {
                throw new InvalidOperationException("Could not determine application directory");
            }

            // Create options for path resolution
            var options = new Options {
                ModelPath = "SqueezeNet.onnx"
            };

            // Resolve file paths using shared helper
            var (modelPath, compiledModelPath, labelsPath) = ModelManager.ResolvePaths(options);

            // Override paths to use app directory
            modelPath = Path.Combine(appDirectory, "SqueezeNet.onnx");
            labelsPath = Path.Combine(appDirectory, "SqueezeNet.Labels.txt");

            if (!File.Exists(modelPath))
            {
                throw new FileNotFoundException($"Model file not found: {modelPath}");
            }
            if (!File.Exists(labelsPath))
            {
                throw new FileNotFoundException($"Labels file not found: {labelsPath}");
            }

            // Create inference session using shared helper
            _session = ModelManager.CreateSession(modelPath, options, _ortEnv);

            // Load labels using shared helper
            _labels = ResultProcessor.LoadLabels(labelsPath).ToList();
        }

        private async Task LoadAndRunDemoAsync()
        {
            // Get application directory
            var appDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            var imagePath = Path.Combine(appDirectory!, "image.jpg");

            if (!File.Exists(imagePath))
            {
                throw new FileNotFoundException($"Image file not found: {imagePath}");
            }

            // Load and display the image
            await LoadAndDisplayImageAsync(imagePath);

            // Run inference automatically  
            await RunInferenceAsync(imagePath);
        }

        private async void ReloadModelButton_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                ResultsText.Text = "Reloading model with new settings...";
                await LoadModelAsync();
                await LoadAndRunDemoAsync();
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Reload failed: {ex.Message}";
            }
        }

        private async Task LoadAndDisplayImageAsync(string imagePath)
        {
            var bitmap = new BitmapImage();
            var imageFile = await StorageFile.GetFileFromPathAsync(imagePath);
            using (var stream = await imageFile.OpenAsync(FileAccessMode.Read))
            {
                await bitmap.SetSourceAsync(stream);
            }
            SelectedImage.Source = bitmap;
        }

        private async Task RunInferenceAsync(string imagePath)
        {
            if (_session == null)
            {
                ResultsText.Text = "Model not loaded.";
                return;
            }

            try
            {
                ResultsText.Text = "Running inference...";

                // Load and preprocess image using shared helper
                using var videoFrame = await ImageProcessor.LoadImageFileAsync(imagePath);
                var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

                // Run inference using shared helper
                using var results = InferenceEngine.RunInference(_session, inputTensor);
                var output = InferenceEngine.ExtractResults(_session, results);

                // Process and display results using shared helper
                var topResults = ResultProcessor.GetTopPredictions(output, _labels, 5);

                var resultText = new StringBuilder();
                resultText.AppendLine("Top 5 Predictions:");
                resultText.AppendLine();

                for (int i = 0; i < topResults.Count; i++)
                {
                    var (label, confidence) = topResults[i];
                    resultText.AppendLine($"{i + 1}. {label}: {confidence:P2}");
                }

                ResultsText.Text = resultText.ToString();
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Inference failed: {ex.Message}";
            }
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
    }
}
