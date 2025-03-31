namespace MauiWindowsCopilotRuntimeSample
{
    public partial class MainPage : ContentPage
    {
        int count = 0;

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

        partial void ShowImageScalingPanelIfSupported();
        partial void ShowTextGenerationPanelIfSupported();

        partial void ScaleImage(double scale);
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

        partial void GenerateTextFromEntryPrompt();
        private void OnGenerateClicked(object sender, EventArgs e)
        {
            GenerateTextFromEntryPrompt();
        }
    }

}
