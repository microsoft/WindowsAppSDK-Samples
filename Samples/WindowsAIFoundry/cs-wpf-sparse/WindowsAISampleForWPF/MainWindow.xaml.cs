using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.ContentSafety;
using Microsoft.Windows.AI.Imaging;
using Microsoft.Windows.AI.Text;
using System.Windows;
using System.Windows.Media.Imaging;
using Windows.Storage;

namespace WindowsAISampleForWPF;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    private ImageBuffer? _currentImage;
    private ImageDescriptionGenerator? imageDescriptionGenerator = null;
    private TextRecognizer? textRecognizer = null;
    public MainWindow()
    {
        InitializeComponent();
    }
    private async void SelectFile_Click(object sender, RoutedEventArgs e)
    {
        _currentImage = null;
        this.Description.Text = "(picking a file)";
        this.FileContent.Text = this.Description.Text;

        var fileDialog = new Microsoft.Win32.OpenFileDialog();
        fileDialog.DefaultExt = ".jpg";
        fileDialog.Filter = "Image files (*.jpg, *.jpeg, *.png)|*.jpg;*.jpeg;*.png";
        if (fileDialog.ShowDialog() == true)
        {
            this.FilePath.Text = fileDialog.FileName;
            this.ProcessFile.IsEnabled = true;
            _currentImage = await GetImageBufferFromFile(fileDialog.FileName);
            var source = new BitmapImage(new Uri(fileDialog.FileName));
            this.InputImage.Source = source;
        }
        this.Description.Text = "(done picking)";
        this.FileContent.Text = this.Description.Text;
    }

    private void PasteImageContent(object sender, RoutedEventArgs e)
    {
        this.Description.Text = "(pasting)";
        this.FileContent.Text = this.Description.Text;
        var clipboard = System.Windows.Clipboard.GetDataObject();
        if (clipboard == null)
        {
            this.FileContent.Text = "Can't open the clipboard, try again";
            return;
        }

        var bitmapContent = clipboard.GetData(DataFormats.Bitmap, true) as BitmapImage;
        if (bitmapContent == null)
        {
            this.FileContent.Text = "That wasn't a bitmap, try again";
            return;
        }
    }

    private async Task<ImageBuffer> GetImageBufferFromFile(string path)
    {
        var storageFile = await StorageFile.GetFileFromPathAsync(path);
        var inputStream = await storageFile.OpenAsync(FileAccessMode.Read);
        var decoder = await Windows.Graphics.Imaging.BitmapDecoder.CreateAsync(inputStream);
        var frame = await decoder.GetFrameAsync(0);
        var softwareBitmap = await frame.GetSoftwareBitmapAsync();
        return ImageBuffer.CreateForSoftwareBitmap(softwareBitmap);
    }

    private async void ProcessFile_Click(object sender, RoutedEventArgs e)
    {
        this.FileContent.Text = "Extracting text...";
        this.Description.Text = "(waiting)";

        try
        {
            this.FileContent.Text = "Loading AI models...";
            this.Description.Text = this.FileContent.Text;
            await LoadAIModels();
        }
        catch (Exception)
        {
            this.FileContent.Text = "An error has occured: Loading AI models...";
            this.Description.Text = this.FileContent.Text;
            return;
        }

        string textInImage = "";
        try
        {
            this.FileContent.Text = "Performing Text Recognition";
            textInImage = await PerformTextRecognition();
        }
        catch (Exception ex)
        {
            this.FileContent.Text = "An error has occured: Performing Text Recognition..." + ex.Message;
            return;
        }

        try
        {
            this.Description.Text = "Performing Image Description";
            await DescribeImage();
        }
        catch (Exception ex)
        {
            this.Description.Text = "An error has occured: Performing Image Description..." + ex.Message;
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
                    this.FileContent.Text = "Image description generator not ready for use";
                    throw new Exception("Image description generator not ready for use");
                }
                break;
            case Microsoft.Windows.AI.AIFeatureReadyState.DisabledByUser:
                System.Diagnostics.Debug.WriteLine("Image Description Generator disabled by user");
                this.FileContent.Text = "Image description generator disabled by user";
                return;
            case Microsoft.Windows.AI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
                System.Diagnostics.Debug.WriteLine("Image Description Generator not supported on current system");
                this.FileContent.Text = "Image description generator not supported on current system";
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
                    this.FileContent.Text = "Text recognizer not ready for use";
                    throw new Exception("Text recognizer not ready for use");
                }
                break;
            case Microsoft.Windows.AI.AIFeatureReadyState.DisabledByUser:
                System.Diagnostics.Debug.WriteLine("Text Recognizer disabled by user");
                this.FileContent.Text = "Text recognizer disabled by user";
                return;
            case Microsoft.Windows.AI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
                System.Diagnostics.Debug.WriteLine("Text Recognizer not supported on current system");
                this.FileContent.Text = "Text recognizer not supported on current system";
                return;
        }

        textRecognizer = await TextRecognizer.CreateAsync();
        if (textRecognizer == null)
        {
            throw new Exception("Failed to create TextRecognizer instance.");
        }
    }

    private Task<string> PerformTextRecognition()
    {
        if (_currentImage == null)
        {
            throw new Exception("Failed to load image buffer.");
        }

        RecognizedText recognizedText = textRecognizer!.RecognizeTextFromImage(_currentImage);

        var recognizedTextLines = recognizedText.Lines.Select(line => line.Text);
        string text = string.Join(Environment.NewLine, recognizedTextLines);

        this.FileContent.Text = text;
        return Task.FromResult(text);
    }

    private async Task DescribeImage()
    {
        if (_currentImage == null)
        {
            throw new Exception("Failed to load image buffer.");
        }

        ContentFilterOptions filterOptions = new();
        var result = await imageDescriptionGenerator!.DescribeAsync(_currentImage, ImageDescriptionKind.BriefDescription, filterOptions);

        if (result.Status != ImageDescriptionResultStatus.Complete)
        {
            throw new Exception($"Image description failed with status: {result.Status}");
        }

        this.Description.Text = result.Description;
    }
}