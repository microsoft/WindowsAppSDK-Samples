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
using Microsoft.UI.Xaml.Controls;

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
                ResultsText.Text = "Enumerating execution providers...\n";

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

                bool allowDownload = AllowProviderDownloadCheckBox.IsChecked ?? false;
                await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

                // Create the single OrtEnv instance for this application
                _ortEnv ??= ModelManager.CreateEnvironment("WindowsMLWinUISample");
                var devices = _ortEnv.GetEpDevices();
                PopulateEpCombo(devices);
                PopulateDeviceCombo(devices);

                EpCombo.SelectionChanged += EpCombo_SelectionChanged;

                providerInfo.AppendLine("========================================");
                providerInfo.AppendLine("Select an execution provider (and device if required) then click 'Load / Reload Model'.");
                ResultsText.Text = providerInfo.ToString();
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Initialization failed: {ex.Message}";
            }
        }

        private async Task LoadModelAsync()
        {
            if (EpCombo.SelectedItem == null)
            {
                ResultsText.Text = "Select an execution provider first.";
                return;
            }

            bool allowDownload = AllowProviderDownloadCheckBox.IsChecked ?? false;

            _session?.Dispose();
            _session = null;

            // Create OrtEnv only once per application instance
            _ortEnv ??= ModelManager.CreateEnvironment("WindowsMLWinUISample");
            await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

            var selectedEp = EpCombo.SelectedItem?.ToString();
            var selectedDeviceType = (DeviceCombo.IsEnabled ? DeviceCombo.SelectedItem?.ToString() : null);

            if (DeviceCombo.IsEnabled && selectedDeviceType == null)
            {
                ResultsText.Text = "Select a device type for the selected execution provider.";
                return;
            }

            var appDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)
                               ?? throw new InvalidOperationException("Could not determine application directory");

            var options = new Options
            {
                ModelPath = "SqueezeNet.onnx",
                EpName = selectedEp,
                DeviceType = selectedDeviceType
            };

            var (modelPath, compiledModelPath, labelsPath) = ModelManager.ResolvePaths(options, _ortEnv);

            if (!File.Exists(modelPath)) throw new FileNotFoundException($"Model file not found: {modelPath}");
            if (!File.Exists(labelsPath)) throw new FileNotFoundException($"Labels file not found: {labelsPath}");

            _session = ModelManager.CreateSession(modelPath, options, _ortEnv);
            _labels = ResultProcessor.LoadLabels(labelsPath).ToList();
        }

        private async Task LoadAndRunDemoAsync()
        {
            // Get application directory
            var appDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            var imagePath = Path.Combine(appDirectory, "image.png");

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
                ResultsText.Text = "Loading / reloading model...";
                await LoadModelAsync();
                if (_session != null)
                {
                    ResultsText.Text += "\nRunning demo inference...";
                    await LoadAndRunDemoAsync();
                }
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

        private void PopulateEpCombo(IEnumerable<OrtEpDevice> devices)
        {
            EpCombo.Items.Clear();
            var epGroups = devices
                .GroupBy(d => d.EpName)
                .OrderBy(g => g.Key, StringComparer.OrdinalIgnoreCase);

            foreach (var g in epGroups)
            {
                EpCombo.Items.Add(g.Key);
            }

            if (EpCombo.Items.Count > 0)
            {
                EpCombo.SelectedIndex = 0;
            }
        }

        private void PopulateDeviceCombo(IEnumerable<OrtEpDevice> devices)
        {
            DeviceCombo.Items.Clear();
            DeviceCombo.IsEnabled = false;

            var selectedEp = EpCombo.SelectedItem?.ToString();
            if (string.IsNullOrEmpty(selectedEp))
            {
                return;
            }

            var epDevices = devices.Where(d => d.EpName == selectedEp).ToList();

            // Generalized: any EP with multiple distinct hardware device types
            var types = epDevices
                .Select(d => d.HardwareDevice.Type.ToString())
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .OrderBy(s => s, StringComparer.OrdinalIgnoreCase)
                .ToList();

            if (types.Count == 1)
            {
                DeviceCombo.Items.Add(types[0]);
                DeviceCombo.SelectedIndex = 0;
            }
            else if (types.Count > 1)
            {
                DeviceCombo.IsEnabled = true;
                foreach (var t in types)
                {
                    DeviceCombo.Items.Add(t);
                }
                DeviceCombo.SelectedIndex = 0;
            }
            else
            {
                // Log an error – no devices for selected EP
                ResultsText.Text = $"No devices found for selected execution provider: {selectedEp}";
            }
        }

        private void EpCombo_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            try
            {
                // Use the shared OrtEnv instance for device refresh
                if (_ortEnv != null)
                {
                    var devices = _ortEnv.GetEpDevices();
                    PopulateDeviceCombo(devices);
                }
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Device refresh failed: {ex.Message}";
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
