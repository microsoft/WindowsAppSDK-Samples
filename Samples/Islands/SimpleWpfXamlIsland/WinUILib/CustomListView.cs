using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WinUILib
{
    public class CustomListView : ListView
    {
        public CustomListView()
        {
            ItemTemplate = Application.Current.Resources["ListViewItemTemplate"] as Microsoft.UI.Xaml.DataTemplate;
        }
    }
}
