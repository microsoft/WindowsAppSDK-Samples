// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Windowing;
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
using WinRT;

namespace SampleApp
{
    public sealed partial class MainWindow : Window
    {
        public String m_windowTitle = "WinUI Desktop C# Sample App";

        public MainWindow()
        {
            this.InitializeComponent();
            this.Title = m_windowTitle;
        }

        private void NavigationView_Loaded(object sender, RoutedEventArgs e)
        {
            ContentFrame.Navigate(typeof(DemoPage), this);           
        }

        private void NavigationView_ItemInvoked(NavigationView sender, NavigationViewItemInvokedEventArgs args)
        {
            if (args.InvokedItemContainer != null)
            {
                var tag = args.InvokedItemContainer.Tag.ToString();
                switch (tag)
                {
                    case "basics":
                        ContentFrame.Navigate(typeof(DemoPage), this);           
                        break;

                    case "presenters":
                        ContentFrame.Navigate(typeof(PresenterPage), this);
                        break;

                    case "configs":
                        ContentFrame.Navigate(typeof(ConfigPage), this);
                        break;

                    case "titlebar":
                        ContentFrame.Navigate(typeof(TitlebarPage), this);
                        break;

                }
            }

        }

        private void NavigationView_BackRequested(NavigationView sender, NavigationViewBackRequestedEventArgs args)
        {
            if (ContentFrame.CanGoBack)
            {
                ContentFrame.GoBack();
            }
        }

        private void MyWindowIcon_DoubleTapped(object sender, DoubleTappedRoutedEventArgs e)
        {
            this.Close();
        }
    }
}
