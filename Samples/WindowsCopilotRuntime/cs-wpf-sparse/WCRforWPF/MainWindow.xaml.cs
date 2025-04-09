using System.Windows;
using System.Windows.Media.Imaging;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.Generative;
using Microsoft.Windows.Vision;
using Windows.Storage;

namespace WCRforWPF;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    public MainWindow()
    {
        InitializeComponent();
    }

 
    private async void SelectFile_Click(object sender, RoutedEventArgs e)
    {
        _currentImage = null;
        this.Description.Text = "(picking a file)";
        this.FileContent.Text = "(picking a file)";

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
    }

    private void PasteImageContent(object sender, RoutedEventArgs e)
    {
        this.Description.Text = "(pasting)";
        this.FileContent.Text = "(pasting)";
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

    private ImageDescriptionGenerator? _ig;
    private ImageBuffer? _currentImage;

    private async Task<ImageBuffer> GetImageBufferFromFile(string path)
    {
        var storageFile = await StorageFile.GetFileFromPathAsync(path);
        var inputStream = await storageFile.OpenAsync(FileAccessMode.Read);
        var decoder = await Windows.Graphics.Imaging.BitmapDecoder.CreateAsync(inputStream);
        var frame = await decoder.GetFrameAsync(0);
        var softwareBitmap = await frame.GetSoftwareBitmapAsync();
        return ImageBuffer.CreateCopyFromBitmap(softwareBitmap);
    }

    private async void ProcessFile_Click(object sender, RoutedEventArgs e)
    {
        this.FileContent.Text = "Extracting text...";
        this.Description.Text = "(waiting)";

        // First, extract text from the image using the OCR model
        var ocr = await EnsureTextRecognizerAsync();
        if (ocr == null)
        {
            this.FileContent.Text = "No OCR model available";
            return;
        }

        // Use the current image selected
        if (_currentImage == null)
        {
            this.FileContent.Text = "No image selected, paste or click the selector";
            return;
        }

        var options = new TextRecognizerOptions();
        options.MaxLineCount = 50;
        this.FileContent.Text = "Extracting text...";
        var imageText = await ocr.RecognizeTextFromImageAsync(_currentImage, options);

        this.FileContent.Text = "";
        foreach (var line in imageText.Lines)
        {
            this.FileContent.Text += line.Text + "\n";
        }

        // Use the image descriptor and see what it thinks of the image
        var ig = await EnsureDescriptionGenerator();
        if (ig == null)
        {
            this.Description.Text = "No image descriptor available";
            return;
        }

        this.Description.Text = "Generating description...";
        var imageDescription = await ig.DescribeAsync(_currentImage, ImageDescriptionScenario.Accessibility);
        this.Description.Text = imageDescription.Response;
    }

    private TextRecognizer? _ocr;

    private async Task<TextRecognizer?> EnsureTextRecognizerAsync()
    {
        if (_ocr == null)
        {
            this.FileContent.Text = "Loading OCR model...";
            try
            {
                if (!TextRecognizer.IsAvailable())
                {
                    this.FileContent.Text = "Making the OCR model available...";
                    var op = await TextRecognizer.MakeAvailableAsync();
                }
                this.FileContent.Text = "Loading OCR model...";
                _ocr = await TextRecognizer.CreateAsync();
            }
            catch (Exception ex)
            {
                this.FileContent.Text = $"Caught exception {ex} trying to load the OCR model";
            }
        }
        return _ocr;
    }

    private async Task<ImageDescriptionGenerator?> EnsureDescriptionGenerator()
    {
        if (_ig == null)
        {
            this.Description.Text = "Loading image description generator...";

            try
            {
                if (!ImageDescriptionGenerator.IsAvailable())
                {
                    this.Description.Text = "Making the descriptor available...";
                    var op = await ImageDescriptionGenerator.MakeAvailableAsync();
                }
                this.Description.Text = "Loading image description generator...";
                _ig = await ImageDescriptionGenerator.CreateAsync();
            }
            catch (Exception ex)
            {
                this.Description.Text = $"Caught exception {ex} trying to load the image description generator";
            }
        }

        return _ig;
    }
}