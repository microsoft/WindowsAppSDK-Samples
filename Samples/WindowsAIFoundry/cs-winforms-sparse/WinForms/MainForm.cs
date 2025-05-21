﻿using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.ContentSafety;
using Microsoft.Windows.AI.Imaging;
using Microsoft.Windows.AI.Text;
using Windows.ApplicationModel;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;

namespace WindowsAISample
{
    // This is a sample application that demonstrates how to use the Windows AI APIs
    // to perform text recognition and summarization on an image.
    // To learn more about the Windows AI API usage, visit https://learn.microsoft.com/windows/ai/apis/
    public partial class MainForm : Form
    {
        private string pathToImage = string.Empty;
        private LanguageModel? languageModel = null;
        private TextRecognizer? textRecognizer = null;

        public MainForm(string? args)
        {
            InitializeComponent();
            this.Text = "Windows AI Sample App with Package Identity";
            ShowPackageInfo();
        }

        private void ShowPackageInfo()
        {
            Package currentPackage = Package.Current;

            MessageBox.Show(null,
                $"Package Name: {currentPackage.DisplayName}\n" +
                $"Package Version: {currentPackage.Id.Version.Major}.{currentPackage.Id.Version.Minor}.{currentPackage.Id.Version.Build}.{currentPackage.Id.Version.Revision}\n" +
                $"Installed Location: {currentPackage.InstalledLocation.Path}", "Package Information");
        }

        private void SelectFile_Click(object sender, EventArgs e)
        {
            using (OpenFileDialog openFileDialog = new OpenFileDialog())
            {
                openFileDialog.Title = "Browse Image Files";
                openFileDialog.Filter = "Image files (*.jpg, *.jpeg, *.png)|*.jpg;*.jpeg;*.png";

                if (openFileDialog.ShowDialog() == DialogResult.OK)
                {
                    //Get the path of specified file
                    pathToImage = openFileDialog.FileName;
                    this.imagePath.Text = pathToImage;

                    // Load the image into a PictureBox
                    SelectedImage.Image = new Bitmap(pathToImage);
                }
            }
        }

        private async void ProcessButton_Click(object sender, EventArgs e)
        {
            try
            {
                richTextBoxForImageText.Text = "Loading AI models...";
                richTextBoxForImageSummary.Text = "Loading AI models...";
                await LoadAIModels();
            }
            catch (Exception ex)
            {
                TaskDialog.ShowDialog(this, new TaskDialogPage {
                    Caption = "Windows AI WinForms Sample Error",
                    Heading = "An error occurred in loading the AI models",
                    Text = ex.Message,
                    Icon = TaskDialogIcon.Error,
                    Buttons = new TaskDialogButtonCollection
                    {
                        TaskDialogButton.OK
                    }
                });
                return;
            }

            string textInImage = string.Empty;
            try
            {
                richTextBoxForImageText.Text = "Performing text recognition...";
                richTextBoxForImageSummary.Text = "Waiting on text recognition...";
                textInImage = await PerformTextRecognition();
            }
            catch (Exception ex)
            {
                TaskDialog.ShowDialog(this, new TaskDialogPage 
                {
                    Caption = "Windows AI WinForms Sample Error",
                    Heading = "An error occurred during text recognition",
                    Text = ex.Message,
                    Icon = TaskDialogIcon.Error,
                    Buttons = new TaskDialogButtonCollection
                    {
                        TaskDialogButton.OK
                    }
                });
                return;
            }

            try
            {
                richTextBoxForImageSummary.Text = "Summarizing image text...";
                await SummarizeImageText(textInImage);
            }
            catch (Exception ex)
            {
                TaskDialog.ShowDialog(this, new TaskDialogPage {
                    Caption = "Windows AI WinForms Sample Error",
                    Heading = "An error occurred while summarizing the text in the image",
                    Text = ex.Message,
                    Icon = TaskDialogIcon.Error,
                    Buttons = new TaskDialogButtonCollection
                    {
                        TaskDialogButton.OK
                    }
                });
                return;
            }
        }

        private async Task LoadAIModels()
        {
            // Load the AI models needed for image processing
            switch (LanguageModel.GetReadyState())
            {
                case Microsoft.Windows.AI.AIFeatureReadyState.NotReady:
                    System.Diagnostics.Debug.WriteLine("Ensure LanguageModel is ready");
                    var op = await LanguageModel.EnsureReadyAsync();
                    System.Diagnostics.Debug.WriteLine($"LanguageModel.EnsureReadyAsync completed with status: {op.Status}");
                    if (op.Status != Microsoft.Windows.AI.AIFeatureReadyResultState.Success)
                    {
                        richTextBoxForImageSummary.Text = "Language model not ready for use";
                        throw new Exception("Language model not ready for use");
                    }
                    break;
                case Microsoft.Windows.AI.AIFeatureReadyState.DisabledByUser:
                    System.Diagnostics.Debug.WriteLine("Language model disabled by user");
                    richTextBoxForImageSummary.Text = "Language model disabled by user";
                    return;
                case Microsoft.Windows.AI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
                    System.Diagnostics.Debug.WriteLine("Language model not supported on current system");
                    richTextBoxForImageSummary.Text = "Language model not supported on current system";
                    return;
            }

            languageModel = await LanguageModel.CreateAsync();
            if (languageModel == null)
            {
                throw new Exception("Failed to create LanguageModel instance.");
            }

            switch (TextRecognizer.GetReadyState())
            {
                case Microsoft.Windows.AI.AIFeatureReadyState.NotReady:
                    System.Diagnostics.Debug.WriteLine("Ensure TextRecognizer is ready");
                    var op = await TextRecognizer.EnsureReadyAsync();
                    System.Diagnostics.Debug.WriteLine($"TextRecognizer.EnsureReadyAsync completed with status: {op.Status}");
                    if (op.Status != Microsoft.Windows.AI.AIFeatureReadyResultState.Success)
                    {
                        richTextBoxForImageText.Text = "Text recognizer not ready for use";
                        throw new Exception("Text recognizer not ready for use");
                    }
                    break;
                case Microsoft.Windows.AI.AIFeatureReadyState.DisabledByUser:
                    System.Diagnostics.Debug.WriteLine("Text Recognizer disabled by user");
                    richTextBoxForImageText.Text = "Text recognizer disabled by user";
                    return;
                case Microsoft.Windows.AI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
                    System.Diagnostics.Debug.WriteLine("Text Recognizer not supported on current system");
                    richTextBoxForImageText.Text = "Text recognizer not supported on current system";
                    return;
            }

            textRecognizer = await TextRecognizer.CreateAsync();
            if (textRecognizer == null)
            {
                throw new Exception("Failed to create TextRecognizer instance.");
            }
        }

        private async Task<string> PerformTextRecognition()
        {
            ImageBuffer? imageBuffer = await LoadImageBufferFromFileAsync(pathToImage);

            if (imageBuffer == null)
            {
                throw new Exception("Failed to load image buffer.");
            }

            RecognizedText recognizedText = textRecognizer!.RecognizeTextFromImage(imageBuffer);

            var recognizedTextLines = recognizedText.Lines.Select(line => line.Text);
            string text = string.Join(Environment.NewLine, recognizedTextLines);

            richTextBoxForImageText.Text = text;
            return text;
        }

        private async Task<ImageBuffer?> LoadImageBufferFromFileAsync(string filePath)
        {
            StorageFile file = await StorageFile.GetFileFromPathAsync(filePath);
            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                SoftwareBitmap bitmap = await decoder.GetSoftwareBitmapAsync();

                return bitmap != null ? ImageBuffer.CreateForSoftwareBitmap(bitmap) : null;
            }
        }

        private async Task SummarizeImageText(string text)
        {
            string systemPrompt = "You summarize user-provided text to a software developer audience." +
                "Respond only with the summary and no additional text.";

            // Update the property names to match the correct ones based on the provided type signature.  
            var promptMaxAllowedSeverityLevel = new TextContentFilterSeverity
            {
                Hate = SeverityLevel.Low,
                Sexual = SeverityLevel.Low,
                Violent = SeverityLevel.Low,
                SelfHarm = SeverityLevel.Low
            };

            var responseMaxAllowedSeverityLevel = new TextContentFilterSeverity
            {
                Hate = SeverityLevel.Low,
                Sexual = SeverityLevel.Low,
                Violent = SeverityLevel.Low,
                SelfHarm = SeverityLevel.Low
            };

            var contentFilterOptions = new ContentFilterOptions
            {
                PromptMaxAllowedSeverityLevel = promptMaxAllowedSeverityLevel,
                ResponseMaxAllowedSeverityLevel = responseMaxAllowedSeverityLevel
            };

            // Create a context for the language model  
            var languageModelContext = languageModel!.CreateContext(systemPrompt, contentFilterOptions);
            string prompt = "Summarize the following text: " + text;

            var output = await languageModel!.GenerateResponseAsync(languageModelContext, prompt, new LanguageModelOptions());

            richTextBoxForImageSummary.Text = output.Text;
        }
    }
}
