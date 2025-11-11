using Microsoft.Graphics.Imaging;
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
    // to perform text recognition and image description on an image.
    // To learn more about the Windows AI API usage, visit https://learn.microsoft.com/windows/ai/apis/
    public partial class MainForm : Form
    {
        private string pathToImage = string.Empty;
        private ImageDescriptionGenerator? imageDescriptionGenerator = null;
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
                richTextBoxForImageSummary.Text = "Describing image...";
                await DescribeImage();
            }
            catch (Exception ex)
            {
                TaskDialog.ShowDialog(this, new TaskDialogPage {
                    Caption = "Windows AI WinForms Sample Error",
                    Heading = "An error occurred while describing the image",
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
            switch (ImageDescriptionGenerator.GetReadyState())
            {
                case Microsoft.Windows.AI.AIFeatureReadyState.NotReady:
                    System.Diagnostics.Debug.WriteLine("Ensure ImageDescriptionGenerator is ready");
                    var opImage = await ImageDescriptionGenerator.EnsureReadyAsync();
                    System.Diagnostics.Debug.WriteLine($"ImageDescriptionGenerator.EnsureReadyAsync completed with status: {opImage.Status}");
                    if (opImage.Status != Microsoft.Windows.AI.AIFeatureReadyResultState.Success)
                    {
                        richTextBoxForImageSummary.Text = "Image description generator not ready for use";
                        throw new Exception("Image description generator not ready for use");
                    }
                    break;
                case Microsoft.Windows.AI.AIFeatureReadyState.DisabledByUser:
                    System.Diagnostics.Debug.WriteLine("Image Description Generator disabled by user");
                    richTextBoxForImageSummary.Text = "Image description generator disabled by user";
                    return;
                case Microsoft.Windows.AI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
                    System.Diagnostics.Debug.WriteLine("Image Description Generator not supported on current system");
                    richTextBoxForImageSummary.Text = "Image description generator not supported on current system";
                    return;
            }

            imageDescriptionGenerator = await ImageDescriptionGenerator.CreateAsync();
            if (imageDescriptionGenerator == null)
            {
                throw new Exception("Failed to create ImageDescriptionGenerator instance.");
            }

            switch (TextRecognizer.GetReadyState())
            {
                case Microsoft.Windows.AI.AIFeatureReadyState.NotReady:
                    System.Diagnostics.Debug.WriteLine("Ensure TextRecognizer is ready");
                    var opText = await TextRecognizer.EnsureReadyAsync();
                    System.Diagnostics.Debug.WriteLine($"TextRecognizer.EnsureReadyAsync completed with status: {opText.Status}");
                    if (opText.Status != Microsoft.Windows.AI.AIFeatureReadyResultState.Success)
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

        private async Task DescribeImage()
        {
            ImageBuffer? imageBuffer = await LoadImageBufferFromFileAsync(pathToImage);

            if (imageBuffer == null)
            {
                throw new Exception("Failed to load image buffer.");
            }

            ContentFilterOptions filterOptions = new();
            var result = await imageDescriptionGenerator!.DescribeAsync(imageBuffer, ImageDescriptionKind.BriefDescription, filterOptions);

            if (result.Status != ImageDescriptionResultStatus.Complete)
            {
                throw new Exception($"Image description failed with status: {result.Status}");
            }

            richTextBoxForImageSummary.Text = result.Description;
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
    }
}
