using Microsoft.Windows.System.Power;

namespace WinFormsApp
{
    public partial class MainForm : Form
    {
        public MainForm()
        {
            InitializeComponent();
            InitializeLabelText();
        }

        private void InitializeLabelText()
        {
            // The resource manager is used to access the resources in the app package.
            // Construct a resource manager using the resource index generated during build.
            var manager = new Microsoft.Windows.ApplicationModel.Resources.ResourceManager();

            // Look up a string in the resources file using the string's name.
            label1.Text = manager.MainResourceMap.GetValue("Resources/Output").ValueAsString;
        }

        private void button1_Click(object sender, EventArgs e)
        {
            // DisplayStatus is a property of the PowerManager class that indicates the current display status for the app session.
            label1.Text = PowerManager.DisplayStatus.ToString();
        }
    }
}
