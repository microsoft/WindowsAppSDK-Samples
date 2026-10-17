using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WinUILib
{
    public partial class MyCustomButton : Microsoft.UI.Xaml.Controls.Button
    {
        public MyCustomButton()
        {
            Content = "Click Me WinUI";
        }
    }
}
