// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media.Imaging;
using Microsoft.ML.OnnxRuntime;
using Microsoft.Win32;
using Microsoft.Windows.AI.MachineLearning;
using WindowsML.Shared;

namespace WindowsMLSampleForWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, IDisposable
    {
        private string? _selectedImagePath;
        private InferenceSession? _session;
        private OrtEnv? _ortEnv;
        private List<string> _labels = new();
        private bool _disposed;

        public MainWindow()
        {
            InitializeComponent(); // Must match x:Class in XAML
            Loaded += MainWindow_Loaded;
            Closed += MainWindow_Closed;
        }

        private void MainWindow_Closed(object? sender, EventArgs e)
        {
            Dispose();
        }

        public void Dispose()
        {
            if (_disposed) return;
            _session?.Dispose();
            _ortEnv?.Dispose();
            _disposed = true;
        }

        private async void MainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            await InitializeAsync();
        }

        private async Task InitializeAsync()
        {
            try
            {
                ResultsTextBox.Text = "Getting available providers...\n";

                var catalog = ExecutionProviderCatalog.GetDefault();
                var providers = catalog.FindAllProviders();
                var providerInfo = new StringBuilder();
                providerInfo.AppendLine("Available Execution Providers:");
                providerInfo.AppendLine("========================================");

                if (providers is not null)
                {
                    foreach (var provider in providers)
                    {
                        providerInfo.AppendLine($"Provider: {provider.Name}");
                        providerInfo.AppendLine($"  Ready state: {provider.ReadyState}");
                        providerInfo.AppendLine();
                    }
                }

                bool allowDownload = AllowProviderDownloadCheckBox.IsChecked ?? false;
                await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

                using (var discoveryEnv = ModelManager.CreateEnvironment("WpfDiscovery"))
                {
                    var devices = discoveryEnv.GetEpDevices();

                    var epGroups = devices
                        .GroupBy(d => d.EpName)
                        .OrderBy(g => g.Key, StringComparer.OrdinalIgnoreCase)
                        .ToList();

                    EpCombo.Items.Clear();
                    foreach (var grp in epGroups)
                    {
                        EpCombo.Items.Add(grp.Key);
                    }

                    if (EpCombo.Items.Count > 0)
                    {
                        EpCombo.SelectedIndex = 0;
                    }

                    PopulateDeviceCombo(discoveryEnv);
                }

                EpCombo.SelectionChanged += EpCombo_SelectionChanged;
                ReloadSessionButton.Click += ReloadSessionButton_Click;

                providerInfo.AppendLine("========================================");
                providerInfo.AppendLine("Select EP/device (if required) then click 'Load / Reload Model'.");
                ResultsTextBox.Text = providerInfo.ToString();
            }
            catch (Exception ex)
            {
                ResultsTextBox.Text = $"Failed to initialize: {ex.Message}";
            }
        }

        private async Task LoadModelAndLabelsAsync()
        {
            if (EpCombo.SelectedItem == null)
            {
                ResultsTextBox.Text = "Select an execution provider first.";
                return;
            }

            _session?.Dispose();
            _session = null;
            _ortEnv?.Dispose();
            _ortEnv = null;

            bool allowDownload = AllowProviderDownloadCheckBox.IsChecked ?? false;

            _ortEnv = ModelManager.CreateEnvironment("WindowsMLSampleForWPF");
            await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

            var options = new Options
            {
                ModelPath = "SqueezeNet.onnx",
                EpName = EpCombo.SelectedItem?.ToString(),
                DeviceType = (DeviceCombo.IsEnabled ? DeviceCombo.SelectedItem?.ToString() : null)
            };

            if (DeviceCombo.IsEnabled && DeviceCombo.SelectedItem == null)
            {
                ResultsTextBox.Text = "Select a device type for the selected execution provider.";
                return;
            }

            var (modelPath, compiledModelPath, labelsPath) = ModelManager.ResolvePaths(options);
            string actualModelPath = ModelManager.ResolveActualModelPath(options, modelPath, compiledModelPath, _ortEnv);
            _session = ModelManager.CreateSession(actualModelPath, options, _ortEnv);
            _labels = ResultProcessor.LoadLabels(labelsPath).ToList();
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

                try
                {
                    var bitmap = new BitmapImage();
                    bitmap.BeginInit();
                    bitmap.UriSource = new Uri(_selectedImagePath);
                    bitmap.CacheOption = BitmapCacheOption.OnLoad;
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
                Dispatcher.Invoke(() => { }, System.Windows.Threading.DispatcherPriority.Render);

                var videoFrame = await ImageProcessor.LoadImageFileAsync(_selectedImagePath);
                var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

                using var results = InferenceEngine.RunInference(_session, inputTensor);
                var resultTensor = InferenceEngine.ExtractResults(_session, results);

                var topPredictions = ResultProcessor.GetTopPredictions(resultTensor, _labels, 5);
                ResultsTextBox.Text = FormatResultsForUI(topPredictions);
            }
            catch (Exception ex)
            {
                ResultsTextBox.Text = $"Error during inference: {ex.Message}";
            }
        }

        private void PopulateDeviceCombo(OrtEnv env)
        {
            DeviceCombo.Items.Clear();
            DeviceCombo.IsEnabled = false;

            var selectedEp = EpCombo.SelectedItem?.ToString();
            if (string.IsNullOrEmpty(selectedEp))
            {
                return;
            }

            var devices = env.GetEpDevices()
                             .Where(d => d.EpName == selectedEp)
                             .ToList();

            // Generalized: any EP with multiple distinct hardware device types
            var types = devices
                .Select(d => d.HardwareDevice.Type.ToString())
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .OrderBy(s => s, StringComparer.OrdinalIgnoreCase)
                .ToList();

            if (types.Count == 1)
            {
                // Single device type – show it but keep combo disabled
                DeviceCombo.IsEnabled = false;
                DeviceCombo.Items.Add(types[0]);
                DeviceCombo.SelectedIndex = 0;
            }
            else if (types.Count > 1)
            {
                // Multiple device types – enable selection
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
                ResultsTextBox.Text = $"No devices found for selected execution provider: {selectedEp}";
            }
        }

        private async void ReloadSessionButton_Click(object sender, RoutedEventArgs e)
        {
            ResultsTextBox.Text = "Loading / reloading model...";
            await LoadModelAndLabelsAsync();
            if (_session != null)
            {
                ResultsTextBox.Text += "\nModel loaded. Select an image and click 'Run Inference'.";
            }
        }

        private void EpCombo_SelectionChanged(object sender, System.Windows.Controls.SelectionChangedEventArgs e)
        {
            try
            {
                using var env = ModelManager.CreateEnvironment("WpfDeviceRefresh");
                PopulateDeviceCombo(env);
            }
            catch (Exception ex)
            {
                ResultsTextBox.Text = $"Failed refreshing devices: {ex.Message}";
            }
        }

        private static string FormatResultsForUI(List<(string Label, float Confidence)> predictions)
        {
            var sb = new StringBuilder();
            sb.AppendLine("Top 5 Predictions:");
            sb.AppendLine("-------------------------------------------");
            sb.AppendLine($"{"Label",-32} {"Confidence",10}");
            sb.AppendLine("-------------------------------------------");

            foreach (var (label, confidence) in predictions)
            {
                sb.AppendLine($"{label,-32} {confidence:P2}");
            }

            sb.AppendLine("-------------------------------------------");
            return sb.ToString();
        }
    }
}
