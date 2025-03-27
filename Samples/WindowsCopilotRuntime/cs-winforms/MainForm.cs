using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.Generative;
using Microsoft.Windows.Vision;
using Windows.Graphics.Imaging;
using Windows.Storage.Streams;
using Windows.Storage;
using Microsoft.Windows.AI.ContentModeration;

namespace WindowsCopilotRuntimeSample
{
    // This is a sample application that demonstrates how to use the Windows Copilot Runtime API
    // to perform text recognition and summarization on an image.
    // To learn more about the Windows Copilot Runtime API usage, visit https://learn.microsoft.com/windows/ai/apis/
    public partial class MainForm : Form
    {
        private string pathToImage = string.Empty;
        private LanguageModel? languageModel = null;
        private TextRecognizer? textRecognizer = null;

        public MainForm()
        {
            InitializeComponent();
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
                    Caption = "WCR WinForms Sample Error",
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
                    Caption = "WCR WinForms Sample Error",
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
                    Caption = "WCR WinForms Sample Error",
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
            if (!LanguageModel.IsAvailable())
            {
                var result = await LanguageModel.MakeAvailableAsync();
                if (result.Status != Microsoft.Windows.Management.Deployment.PackageDeploymentStatus.CompletedSuccess)
                {
                    throw new Exception(result.ExtendedError.Message);
                }
            }

            languageModel = await LanguageModel.CreateAsync();
            if (languageModel == null)
            {
                throw new Exception("Failed to create LanguageModel instance.");
            }

            if (!TextRecognizer.IsAvailable())
            {
                var result = await TextRecognizer.MakeAvailableAsync();
                if (result.Status != Microsoft.Windows.Management.Deployment.PackageDeploymentStatus.CompletedSuccess)
                {
                    throw new Exception(result.ExtendedError.Message);
                }
            }

            textRecognizer = await TextRecognizer.CreateAsync();
            if (textRecognizer == null)
            {
                throw new Exception("Failed to create TextRecognizer instance.");
            }
        }

        private async Task<string> PerformTextRecognition()
        {
            // The OCR model requires the LanguageModel to be used first or
            // else it returns an interface not registered error.
            // This issue is currently under investigation.
            string prompt = "What is Windows App SDK?";
            var output = await languageModel!.GenerateResponseAsync(prompt);

            ImageBuffer? imageBuffer = await LoadImageBufferFromFileAsync(pathToImage);

            if (imageBuffer == null)
            {
                throw new Exception("Failed to load image buffer.");
            }

            TextRecognizerOptions options = new TextRecognizerOptions { };
            RecognizedText recognizedText = textRecognizer!.RecognizeTextFromImage(imageBuffer, options);

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

                return bitmap != null ? ImageBuffer.CreateBufferAttachedToBitmap(bitmap) : null;
            }
        }

        private async Task SummarizeImageText(string text)
        {
            string systemPrompt = "You summarize user-provided text to a software developer audience." +
                "Respond only with the summary and no additional text.";

            // To learn more about content moderation, visit https://learn.microsoft.com/windows/ai/apis/content-moderation
            var promptMinSeverityLevelToBlock = new TextContentFilterSeverity {
                HateContentSeverity = SeverityLevel.Low,
                SexualContentSeverity = SeverityLevel.Low,
                ViolentContentSeverity = SeverityLevel.Low,
                SelfHarmContentSeverity = SeverityLevel.Low
            };

            var responseMinSeverityLevelToBlock = new TextContentFilterSeverity {
                HateContentSeverity = SeverityLevel.Low,
                SexualContentSeverity = SeverityLevel.Low,
                ViolentContentSeverity = SeverityLevel.Low,
                SelfHarmContentSeverity = SeverityLevel.Low
            };

            var contentFilterOptions = new ContentFilterOptions {
                PromptMinSeverityLevelToBlock = promptMinSeverityLevelToBlock,
                ResponseMinSeverityLevelToBlock = responseMinSeverityLevelToBlock
            };

            // Create a context for the language model
            var languageModelContext = languageModel!.CreateContext(systemPrompt, contentFilterOptions);
            string prompt = "Summarize the following text: " + text;
            var output = await languageModel!.GenerateResponseAsync(new LanguageModelOptions(), prompt, contentFilterOptions, languageModelContext);
            richTextBoxForImageSummary.Text = output.Response;
        }
    }
}
