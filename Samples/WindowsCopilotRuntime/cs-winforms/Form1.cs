using Microsoft.Windows.AI.Generative;

namespace WinFormsApp1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private async void button1_ClickAsync(object sender, EventArgs e)
        {
            if (!LanguageModel.IsAvailable())
            {
                var op = await LanguageModel.MakeAvailableAsync();
            }

            using LanguageModel languageModel = await LanguageModel.CreateAsync();

            string prompt = "Provide the molecular formula for glucose.";
            var result = await languageModel.GenerateResponseAsync(prompt);

            Console.WriteLine(result.Response);
            label1.Text = result.Response;
        }
    }
}
