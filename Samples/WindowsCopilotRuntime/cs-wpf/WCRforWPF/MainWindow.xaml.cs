using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Drawing.Imaging;
using Microsoft.Graphics.Imaging;
using Microsoft.Windows.AI.Generative;
using Microsoft.Windows.Vision;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Microsoft.Windows.AI;
using Microsoft.Windows.AI.ContentModeration;
using Windows.Storage.Streams;

namespace WCRforWPF;

/// <summary>
/// Interaction logic for MainWindow.xaml
/// </summary>
public partial class MainWindow : Window
{
    private ImageBuffer? _currentImage;
    private LanguageModel? languageModel = null;
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
        return ImageBuffer.CreateCopyFromBitmap(softwareBitmap);
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
        catch (Exception ex)
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
            this.FileContent.Text = "An error has occured: Performing Text Recognition...";
            return;
        }

        try
        {
            this.Description.Text = "Performing Text Description";
            await SummarizeImageText(textInImage);
        }
        catch (Exception ex)
        {
            this.Description.Text = "An error has occured: Performing Text Description...";
            return;
        }   
    }

    private async Task LoadAIModels()
    {
        // Load the AI models needed for image processing
        switch (LanguageModel.GetReadyState())
        {
            case Microsoft.Windows.AI.AIFeatureReadyState.EnsureNeeded:
                System.Diagnostics.Debug.WriteLine("Ensure LanguageModel is ready");
                var op = await LanguageModel.EnsureReadyAsync();
                System.Diagnostics.Debug.WriteLine($"LanguageModel.EnsureReadyAsync completed with status: {op.Status}");
                if (op.Status != Microsoft.Windows.AI.AIFeatureReadyResultState.Success)
                {
                    this.Description.Text = "Language model not ready for use";
                    throw new Exception("Language model not ready for use");
                }
                break;
            case Microsoft.Windows.AI.AIFeatureReadyState.DisabledByUser:
                System.Diagnostics.Debug.WriteLine("Language model disabled by user");
                this.Description.Text = "Language model disabled by user";
                return;
            case Microsoft.Windows.AI.AIFeatureReadyState.NotSupportedOnCurrentSystem:
                System.Diagnostics.Debug.WriteLine("Language model not supported on current system");
                this.Description.Text = "Language model not supported on current system";
                return;
        }

        languageModel = await LanguageModel.CreateAsync();
        if (languageModel == null)
        {
            throw new Exception("Failed to create LanguageModel instance.");
        }

        switch (TextRecognizer.GetReadyState())
        {
            case Microsoft.Windows.AI.AIFeatureReadyState.EnsureNeeded:
                System.Diagnostics.Debug.WriteLine("Ensure TextRecognizer is ready");
                var op = await TextRecognizer.EnsureReadyAsync();
                System.Diagnostics.Debug.WriteLine($"TextRecognizer.EnsureReadyAsync completed with status: {op.Status}");
                if (op.Status != Microsoft.Windows.AI.AIFeatureReadyResultState.Success)
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

    private async Task<string> PerformTextRecognition()
    {
        if (_currentImage == null)
        {
            throw new Exception("Failed to load image buffer.");
        }

        TextRecognizerOptions options = new TextRecognizerOptions { };
        RecognizedText recognizedText = textRecognizer!.RecognizeTextFromImage(_currentImage, options);

        var recognizedTextLines = recognizedText.Lines.Select(line => line.Text);
        string text = string.Join(Environment.NewLine, recognizedTextLines);

        this.FileContent.Text = text;
        return text;
    }

    private async Task SummarizeImageText(string text)
    {
        string systemPrompt = "You summarize user-provided text to a software developer audience." +
            "Respond only with the summary and no additional text.";

        // Update the property names to match the correct ones based on the provided type signature.  
        var promptMaxAllowedSeverityLevel = new TextContentFilterSeverity {
            Hate = SeverityLevel.Low,
            Sexual = SeverityLevel.Low,
            Violent = SeverityLevel.Low,
            SelfHarm = SeverityLevel.Low
        };

        var responseMaxAllowedSeverityLevel = new TextContentFilterSeverity {
            Hate = SeverityLevel.Low,
            Sexual = SeverityLevel.Low,
            Violent = SeverityLevel.Low,
            SelfHarm = SeverityLevel.Low
        };

        var contentFilterOptions = new ContentFilterOptions {
            PromptMaxAllowedSeverityLevel = promptMaxAllowedSeverityLevel,
            ResponseMaxAllowedSeverityLevel = responseMaxAllowedSeverityLevel
        };

        if (languageModel != null)
        {
            // Create a context for the language model
            var languageModelContext = languageModel!.CreateContext(systemPrompt, contentFilterOptions);
            string prompt = "Summarize the following text: " + text;
            var output = await languageModel.GenerateResponseAsync(languageModelContext, prompt, new LanguageModelOptions());
            this.Description.Text = output.Text;
        }
        else
        {
            System.Diagnostics.Debug.WriteLine("Error: LanguageModel is null but should have been created during LoadAIModels()");
            this.Description.Text = "Error: LanguageModel is null";
        }
    }
}