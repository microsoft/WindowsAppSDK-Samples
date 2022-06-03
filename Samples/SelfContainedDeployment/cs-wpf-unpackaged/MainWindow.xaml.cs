using System;
using System.Collections.Generic;
using System.Linq;
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
using Microsoft.Windows.ApplicationModel.Resources;

namespace SelfContainedDeployment
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            var manager = new ResourceManager("SelfContainedDeployment.pri");

            Description.Text = manager.MainResourceMap.GetValue("Resources/Description").ValueAsString;
        }
    }
}
