﻿// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using Microsoft.UI.Xaml;
using Microsoft.Windows.ApplicationModel.Resources;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winui_desktop_packaged_app
{
    public sealed partial class MainWindow : Window
    {
        public MainWindow(ResourceLoader resourceLoader, ResourceManager resourceManager)
        {
            m_resourceLoader = resourceLoader;
            m_resourceManager = resourceManager;

            // Create custom resource context for the window. Set the language to German.
            m_resourceContext = resourceManager.CreateResourceContext();
            m_resourceContext.QualifierValues["Language"] = "de-DE";
            
            this.InitializeComponent();            
        }

        private void default_Click(object sender, RoutedEventArgs e)
        {
            // The resource loader's default constructor assumes that resources are in the "Resources" scope.
            // Use the non-default constructor to load resources in a different scope.

            var resourceString = m_resourceLoader.GetString("SampleString");
            output.Text = resourceString;
        }

        private void override_Click(object sender, RoutedEventArgs e)
        {
            // The resource manager does not have a default scope and resolves resources based on the root.

            var resourceCandidate = m_resourceManager.MainResourceMap.GetValue("Resources/SampleString", m_resourceContext);
            var resourceString = resourceCandidate.ValueAsString;

            output.Text = resourceString;
        }

        private void fallback_Click(object sender, RoutedEventArgs e)
        {
            var resourceCandidate = m_resourceManager.MainResourceMap.GetValue("LegacyString");
            var resourceString = resourceCandidate.ValueAsString;

            output.Text = resourceString;
        }

        private void defaultViaLib_Click(object sender, RoutedEventArgs e)
        {
            var classLib = new ClassLibrary.Class();
            output.Text = classLib.GetClassLibrarySampleString();
        }

        private ResourceLoader m_resourceLoader;
        private ResourceManager m_resourceManager;
        private ResourceContext m_resourceContext;
    }
}
