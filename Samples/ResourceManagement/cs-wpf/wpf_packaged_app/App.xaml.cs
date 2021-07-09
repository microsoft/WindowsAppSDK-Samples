// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System.Windows;
using Microsoft.ApplicationModel.Resources;

namespace wpf_packaged_app
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : System.Windows.Application
    {
        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
        }

        private void AppStartup(object sender, StartupEventArgs e)
        {
            m_resourceLoader = new ResourceLoader();
            m_resourceManager = new ResourceManager();

            // Fall back to other resource loaders if the resource is not found in MRT, in this case .net.
            // This enables carrying forward existing assets without the need to convert them.
            m_resourceManager.ResourceNotFound += (sender, args) =>
            {
                var candidate = new ResourceCandidate(
                    ResourceCandidateKind.String,
                    LegacyResources.ResourceManager.GetString(args.Name, LegacyResources.Culture));

                args.SetResolvedCandidate(candidate);
            };

            m_window = new MainWindow(m_resourceLoader, m_resourceManager);
            m_window.Show();

            this.InitializeComponent();
        }

        private System.Windows.Window m_window;

        // These do disk I/O and should be centrally cached.
        // Since both end up loading the same file, ideally an app should only use one.
        // The resource manager is a superset of the resource loader in terms of functionality.
        private ResourceLoader m_resourceLoader;
        private ResourceManager m_resourceManager;
    }
}
