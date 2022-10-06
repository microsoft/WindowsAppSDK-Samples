using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using System.Text;
// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace WinUIComponentCs
{
  public sealed class NameReporterModel
  { 
    public string FName { get; set; }
    public string LName { get; set; }

    public override string ToString()
    {
      return FName + " -> " + LName;
    }
  }

  public sealed partial class NameReporter : UserControl
  {
    public NameReporter()
    {
      this.InitializeComponent();
    }

    private void Button_Click(object sender, RoutedEventArgs e)
    {
      StringBuilder displayText = new StringBuilder("Hello, ");
      displayText.AppendFormat("{0} {1}.", firstName.Text, lastName.Text);
      result.Text = displayText.ToString();
    }

    public string GetFirstName()
    {
      return firstName.Text;
    }

    public NameReporterModel GetModel()
    {
      var data = new NameReporterModel();
      data.FName = firstName.Text;
      data.LName = lastName.Text;
      return data;
    }
  }
}
