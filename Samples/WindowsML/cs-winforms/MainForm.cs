// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using Microsoft.Windows.AI.MachineLearning;
using System.Linq;
using System.Reflection;
using System.Text;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Storage;
using Windows.Storage.Streams;
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

                foreach (var provider in providers)
                {
                    providerInfo.AppendLine($"Provider: {provider.Name}");
                    providerInfo.AppendLine($"  Ready state: {provider.ReadyState}");
                    providerInfo.AppendLine();
                }

                providerInfo.AppendLine("========================================");
                providerInfo.AppendLine("Loading model...");
                resultsTextBox.Text = providerInfo.ToString();
                Application.DoEvents(); // Update UI

                await LoadModelAndLabelsAsync();
                resultsTextBox.Text += "\r\nModel loaded successfully. Select an image and click 'Run Inference' to see classification results.";
            }
            catch (Exception ex)
            {
                resultsTextBox.Text = $"Failed to initialize: {ex.Message}";
            }
        }

        private async Task LoadModelAndLabelsAsync()
        {
            // Use checkbox value for provider downloads
            bool allowDownload = allowProviderDownloadCheckBox.Checked;

            // Create ONNX Runtime environment using shared helper
            var ortEnv = ModelManager.CreateEnvironment("WindowsMLWinFormsSample");

            // Initialize execution providers using shared helper  
            await ModelManager.InitializeExecutionProvidersAsync(allowDownload: allowDownload);

            // Get model and labels paths using shared helper
            string executableFolder = Path.GetDirectoryName(Assembly.GetEntryAssembly()!.Location)!;
            var options = new Options { ModelPath = "SqueezeNet.onnx" };
            var (modelPath, compiledModelPath, labelsPath) = ModelManager.ResolvePaths(options);

            // Create inference session using shared helper
            string actualModelPath = ModelManager.ResolveActualModelPath(options, modelPath, compiledModelPath, ortEnv);
            _session = ModelManager.CreateSession(actualModelPath, options, ortEnv);

            // Load labels using shared helper
            _labels = ResultProcessor.LoadLabels(labelsPath).ToList();
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
    }
}
