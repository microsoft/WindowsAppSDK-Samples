using WinFormsWithIsland;

namespace WinFormsWithIslandApp
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

            this.desktopWindowXamlSourceControl1.Content = new WinUI3Page();
        }
    }
}
