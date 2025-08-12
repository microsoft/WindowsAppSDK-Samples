// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Imaging;
using System.Runtime.InteropServices.WindowsRuntime;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Storage;
using Microsoft.ML.OnnxRuntime;
using Microsoft.ML.OnnxRuntime.Tensors;
using System.Reflection;

namespace WindowsMLSample
{
    public sealed partial class MainWindow : Window
    {
        private InferenceSession? _session;
        private List<string> _labels = new();

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
                // Initialize the model and load labels
                var appDirectory = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
                if (string.IsNullOrEmpty(appDirectory))
                {
                    throw new InvalidOperationException("Could not determine application directory");
                }

                var modelPath = Path.Combine(appDirectory, "SqueezeNet.onnx");
                var labelsPath = Path.Combine(appDirectory, "SqueezeNet.Labels.txt");
                var imagePath = Path.Combine(appDirectory, "image.jpg");

                if (!File.Exists(modelPath))
                {
                    throw new FileNotFoundException($"Model file not found: {modelPath}");
                }
                if (!File.Exists(labelsPath))
                {
                    throw new FileNotFoundException($"Labels file not found: {labelsPath}");
                }
                if (!File.Exists(imagePath))
                {
                    throw new FileNotFoundException($"Image file not found: {imagePath}");
                }

                // Load the ONNX model
                _session = new InferenceSession(modelPath);

                // Load labels
                _labels = LoadLabels(labelsPath);

                // Load and display the image
                await LoadAndDisplayImageAsync(imagePath);

                // Run inference automatically
                await RunInferenceAsync(imagePath);
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Initialization failed: {ex.Message}";
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

        private List<string> LoadLabels(string labelsPath)
        {
            return File.ReadAllLines(labelsPath)
                .Select(line => line.Split(',', 2)[1])
                .ToList();
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

                // Load and preprocess the image
                var imageFile = await StorageFile.GetFileFromPathAsync(imagePath);
                using var stream = await imageFile.OpenAsync(FileAccessMode.Read);
                var decoder = await BitmapDecoder.CreateAsync(stream);
                var softwareBitmap = await decoder.GetSoftwareBitmapAsync();

                // Convert to VideoFrame
                var videoFrame = VideoFrame.CreateWithSoftwareBitmap(softwareBitmap);

                // Preprocess the image to match model requirements (224x224, normalized)
                var inputTensor = await PreprocessImageAsync(videoFrame);

                // Run inference
                var inputName = _session.InputMetadata.First().Key;
                var inputs = new List<NamedOnnxValue>
                {
                    NamedOnnxValue.CreateFromTensor(inputName, inputTensor)
                };

                using var results = _session.Run(inputs);
                var output = results.FirstOrDefault()?.AsEnumerable<float>().ToArray();

                if (output != null)
                {
                    // Apply softmax to convert logits to probabilities
                    var probabilities = ApplySoftmax(output);

                    // Process results
                    var topResults = GetTopPredictions(probabilities, 5);
                    var resultText = new StringBuilder();
                    resultText.AppendLine("Top 5 Predictions:");
                    resultText.AppendLine();

                    for (int i = 0; i < topResults.Count; i++)
                    {
                        var (index, probability) = topResults[i];
                        var label = index < _labels.Count ? _labels[index] : $"Class {index}";
                        resultText.AppendLine($"{i + 1}. {label}: {probability:P2}");
                    }

                    ResultsText.Text = resultText.ToString();
                }
                else
                {
                    ResultsText.Text = "No results returned from model.";
                }

                // Clean up resources
                videoFrame?.Dispose();
                softwareBitmap?.Dispose();
            }
            catch (Exception ex)
            {
                ResultsText.Text = $"Inference failed: {ex.Message}";
            }
        }

        private async Task<Tensor<float>> PreprocessImageAsync(VideoFrame videoFrame)
        {
            // Resize to 224x224 (SqueezeNet input size)
            var resizedFrame = new VideoFrame(BitmapPixelFormat.Bgra8, 224, 224);
            await videoFrame.CopyToAsync(resizedFrame);

            // Convert to SoftwareBitmap
            var softwareBitmap = resizedFrame.SoftwareBitmap;
            if (softwareBitmap == null)
            {
                throw new InvalidOperationException("Failed to get software bitmap from video frame");
            }

            // Convert to RGB format
            if (softwareBitmap.BitmapPixelFormat != BitmapPixelFormat.Bgra8)
            {
                softwareBitmap = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8);
            }

            // Get pixel data
            var buffer = new byte[224 * 224 * 4]; // BGRA
            softwareBitmap.CopyToBuffer(buffer.AsBuffer());

            // ImageNet normalization values
            float[] mean = [0.485f, 0.456f, 0.406f];
            float[] std = [0.229f, 0.224f, 0.225f];

            // Convert to RGB float tensor [1, 3, 224, 224] with proper normalization
            var tensor = new DenseTensor<float>(new[] { 1, 3, 224, 224 });

            for (int y = 0; y < 224; y++)
            {
                for (int x = 0; x < 224; x++)
                {
                    int pixelIndex = (y * 224 + x) * 4;

                    // Extract BGR values and convert to RGB, normalize to [0,1]
                    float blue = buffer[pixelIndex] / 255.0f;
                    float green = buffer[pixelIndex + 1] / 255.0f;
                    float red = buffer[pixelIndex + 2] / 255.0f;

                    // Apply ImageNet normalization
                    red = (red - mean[0]) / std[0];
                    green = (green - mean[1]) / std[1];
                    blue = (blue - mean[2]) / std[2];

                    // Set RGB values in CHW format
                    tensor[0, 0, y, x] = red;   // R channel
                    tensor[0, 1, y, x] = green; // G channel
                    tensor[0, 2, y, x] = blue;  // B channel
                }
            }

            // Dispose the temporary frame
            resizedFrame.Dispose();

            return tensor;
        }

        private float[] ApplySoftmax(float[] logits)
        {
            // Apply softmax to convert logits to probabilities
            float maxLogit = logits.Max();
            var expScores = logits.Select(r => MathF.Exp(r - maxLogit)).ToArray();
            float sumExp = expScores.Sum();
            return expScores.Select(e => e / sumExp).ToArray();
        }

        private List<(int Index, float Probability)> GetTopPredictions(float[] probabilities, int topCount)
        {
            return probabilities
                .Select((probability, index) => new { Index = index, Probability = probability })
                .OrderByDescending(x => x.Probability)
                .Take(topCount)
                .Select(x => (x.Index, x.Probability))
                .ToList();
        }
    }
}
