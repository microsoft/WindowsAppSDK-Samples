using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinUIApp
{
    public class CustomListView2 : ListView
    {
        public CustomListView2()
        {
            ItemTemplate = Application.Current.Resources["ListViewItemTemplate"] as Microsoft.UI.Xaml.DataTemplate;
        }
    }
}
