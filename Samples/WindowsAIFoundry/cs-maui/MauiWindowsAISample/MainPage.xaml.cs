namespace MauiWindowsAISample
{
    public partial class MainPage : ContentPage
    {
        public MainPage()
        {
            InitializeComponent();

            // Assume image scaling and text generation aren't supported
            imageScalingPanel.IsVisible = false;
            textGenerationPanel.IsVisible = false;

            // Show the content if supported
            ShowImageScalingPanelIfSupported();
            ShowTextGenerationPanelIfSupported();
        }

        // Declare partial methods on this MainPage class which can be implemented by platform-specific
        // definitions of this partial class. In this sample, these partial methods are implemented only
        // in the Windows-specific Platforms/Windows/MainPage.cs. These will no-op on other platforms.
        partial void ShowImageScalingPanelIfSupported();
        partial void ShowTextGenerationPanelIfSupported();
        partial void ScaleImage(double scale);
        partial void GenerateTextFromEntryPrompt();

        // Event handlers for image scaling
        private void OnUnscaledClicked(object sender, EventArgs e)
        {
            ScaleImage(0);
        }
        private void OnScale2xClicked(object sender, EventArgs e)
        {
            ScaleImage(2.0);
        }
        private void OnScale4xClicked(object sender, EventArgs e)
        {
            ScaleImage(4.0);
        }

        // Event handler for text generation
        private void OnGenerateClicked(object sender, EventArgs e)
        {
            GenerateTextFromEntryPrompt();
        }
    }

}
