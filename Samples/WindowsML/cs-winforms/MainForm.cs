// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using Microsoft.Windows.AI.MachineLearning;
using System.Reflection;
using System.Text;
using WindowsML.Shared;

namespace WindowsMLWinFormsSample
{
    public partial class MainForm : Form
    {
        private string? _selectedImagePath;
        private InferenceSession? _session;
        private List<string> _labels = new();

        public MainForm()
        {
            InitializeComponent();
            Load += MainForm_Load;
            FormClosed += MainForm_FormClosed;
        }

        private async void MainForm_Load(object? sender, EventArgs e)
        {
            try
            {
                await InitializeAsync();
            }
            catch (Exception ex)
            {
                resultsTextBox.Text = $"Failed to load application: {ex.Message}";
            }
        }

        private void MainForm_FormClosed(object? sender, FormClosedEventArgs e)
        {
            _session?.Dispose();
        }

        private async Task InitializeAsync()
        {
            try
            {
                // Display execution provider information first
                resultsTextBox.Text = "Getting available providers...\r\n";
                Application.DoEvents(); // Update UI

                // Get provider information directly from ExecutionProviderCatalog
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

                // Initialize providers (downloads optional) so that device discovery is accurate
                await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowProviderDownloadCheckBox.Checked);

                // Create a temporary environment just to discover devices for populating dropdowns
                using var discoveryEnv = ModelManager.CreateEnvironment("WinFormsDiscovery");
                var devices = discoveryEnv.GetEpDevices();

                var epGroups = devices
                    .GroupBy(d => d.EpName)
                    .OrderBy(g => g.Key, StringComparer.OrdinalIgnoreCase)
                    .ToList();

                epCombo.Items.Clear();
                foreach (var grp in epGroups)
                {
                    epCombo.Items.Add(grp.Key);
                }

                if (epCombo.Items.Count > 0)
                {
                    epCombo.SelectedIndex = 0;
                }

                // Populate device combo based on initial EP selection
                PopulateDeviceCombo(discoveryEnv);

                providerInfo.AppendLine("========================================");
                providerInfo.AppendLine("Loading model...");
                resultsTextBox.Text = providerInfo.ToString();
                Application.DoEvents(); // Update UI

                // Only load model after user clicks the Load/Reload button (gives chance to change EP/device)
                resultsTextBox.Text += "\r\nSelect EP/device (if required) then click 'Load / Reload Model'.";
            }
            catch (Exception ex)
            {
                resultsTextBox.Text = $"Failed to initialize: {ex.Message}";
            }
        }

        private async Task LoadModelAndLabelsAsync()
        {
            if (epCombo.SelectedItem == null)
            {
                resultsTextBox.Text = "Select an execution provider first.";
                return;
            }

            // Dispose previous session if reloading
            _session?.Dispose();
            _session = null;

            bool allowDownload = allowProviderDownloadCheckBox.Checked;

            var ortEnv = ModelManager.CreateEnvironment("WindowsMLWinFormsSample");
            await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

            var options = new Options
            {
                ModelPath = "SqueezeNet.onnx",
                EpName = epCombo.SelectedItem!.ToString(),
                DeviceType = deviceCombo.Enabled ? deviceCombo.SelectedItem?.ToString() : null
            };

            // Validate device selection for any EP
            if (deviceCombo.Enabled && deviceCombo.SelectedItem == null)
            {
                resultsTextBox.Text = "Select a device type for the selected execution provider.";
                return;
            }

            var (modelPath, compiledModelPath, labelsPath) = ModelManager.ResolvePaths(options);
            string actualModelPath = ModelManager.ResolveActualModelPath(options, modelPath, compiledModelPath, ortEnv);
            _session = ModelManager.CreateSession(actualModelPath, options, ortEnv);
            _labels = ResultProcessor.LoadLabels(labelsPath).ToList();

            resultsTextBox.Text += "\r\nModel loaded.";
        }

        private void SelectImageButton_Click(object sender, EventArgs e)
        {
            using var openFileDialog = new OpenFileDialog();
            openFileDialog.Title = "Select Image File";
            openFileDialog.Filter = "Image files (*.jpg, *.jpeg, *.png)|*.jpg;*.jpeg;*.png";
            openFileDialog.FilterIndex = 1;

            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                _selectedImagePath = openFileDialog.FileName;
                imagePathLabel.Text = Path.GetFileName(_selectedImagePath);

                // Display the selected image
                try
                {
                    selectedImagePictureBox.Image?.Dispose();
                    selectedImagePictureBox.Image = new Bitmap(_selectedImagePath);
                    runInferenceButton.Enabled = true;
                    resultsTextBox.Text = "Image selected. Click 'Run Inference' to classify the image.";
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error loading image: {ex.Message}", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private async void RunInferenceButton_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrEmpty(_selectedImagePath) || _session == null)
            {
                resultsTextBox.Text = "Please select an image first.";
                return;
            }

            try
            {
                resultsTextBox.Text = "Running inference...";
                Application.DoEvents(); // Update UI

                // Load and preprocess the image using shared helper
                using var videoFrame = await ImageProcessor.LoadImageFileAsync(_selectedImagePath);
                var inputTensor = await ImageProcessor.PreprocessImageAsync(videoFrame);

                // Run inference using shared helper
                using var results = InferenceEngine.RunInference(_session, inputTensor);
                var resultTensor = InferenceEngine.ExtractResults(_session, results);

                // Process and display results using shared helper
                var topPredictions = ResultProcessor.GetTopPredictions(resultTensor, _labels, 5);
                var formattedResults = FormatResultsForUI(topPredictions);
                resultsTextBox.Text = formattedResults;
            }
            catch (Exception ex)
            {
                resultsTextBox.Text = $"Error during inference: {ex.Message}";
            }
        }

        private string FormatResultsForUI(List<(string Label, float Confidence)> predictions)
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

        // Generalized device population: any EP with multiple distinct hardware device types
        private void PopulateDeviceCombo(OrtEnv env)
        {
            deviceCombo.Items.Clear();
            deviceCombo.Enabled = false;

            var selectedEp = epCombo.SelectedItem?.ToString();
            if (string.IsNullOrEmpty(selectedEp)) return;

            var devices = env.GetEpDevices()
                             .Where(d => d.EpName == selectedEp)
                             .ToList();

            var types = devices
                .Select(d => d.HardwareDevice.Type.ToString())
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .OrderBy(s => s, StringComparer.OrdinalIgnoreCase)
                .ToList();

            if (types.Count == 1)
            {
                deviceCombo.Items.Add(types[0]);
                deviceCombo.SelectedIndex = 0; // disabled, just display
                deviceCombo.Enabled = false;
            }
            else if (types.Count > 1)
            {
                foreach (var t in types)
                {
                    deviceCombo.Items.Add(t);
                }
                deviceCombo.SelectedIndex = 0;
                deviceCombo.Enabled = true;
            }
            else
            {
                // Log an error – no devices for selected EP
                resultsTextBox.Text = $"No devices found for selected execution provider: {selectedEp}";
            }
        }

        private void EpCombo_SelectedIndexChanged(object? sender, EventArgs e)
        {
            try
            {
                using var env = ModelManager.CreateEnvironment("WinFormsDeviceRefresh");
                PopulateDeviceCombo(env);
            }
            catch (Exception ex)
            {
                resultsTextBox.Text = $"Failed refreshing devices: {ex.Message}";
            }
        }

        private async void ReloadSessionButton_Click(object? sender, EventArgs e)
        {
            resultsTextBox.Text = "Loading / reloading model...";
            await LoadModelAndLabelsAsync();
        }
    }
}
