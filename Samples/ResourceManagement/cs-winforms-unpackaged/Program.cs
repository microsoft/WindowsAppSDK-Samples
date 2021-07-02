// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System;
using System.Windows.Forms;
using Microsoft.ApplicationModel.Resources;
using Microsoft.Windows.ApplicationModel;

namespace winforms_unpackaged_app
{
    static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.SetHighDpiMode(HighDpiMode.SystemAware);
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

            // Initialize dynamic dependencies so we can consume the Project Reunion APIs in the Project Reunion framework package from this unpackaged app.
            // Take a dependency on Project Reunion v0.8 preview.
            MddBootstrap.Initialize(8, "preview");
            
            var resourceManager = new ResourceManager("winforms_unpackaged_app.pri");
            // Fall back to other resource loaders if the resource is not found in MRT, in this case .net.
            // This enables carrying forward existing assets without the need to convert them.
            resourceManager.ResourceNotFound += (sender, args) =>
            {
                var candidate = new ResourceCandidate(
                    ResourceCandidateKind.String,
                    LegacyResources.ResourceManager.GetString(args.Name, LegacyResources.Culture));

                args.SetResolvedCandidate(candidate);
            };

            var resourceLoader = new ResourceLoader();

            // Create a custom resource context. Set the language to German.
            var overrideResourceContext = resourceManager.CreateResourceContext();
            overrideResourceContext.QualifierValues["Language"] = "de-DE";

            // The resource manager and the resource loader do disk I/O and should be centrally cached.
            // Since both end up loading the same file, ideally an app should only use one.
            // The resource manager is a superset of the resource loader in terms of functionality.
            Application.Run(new AppForm(resourceManager, resourceLoader, overrideResourceContext));

            // Uninitialize dynamic dependencies.
            MddBootstrap.Shutdown();
        }
    }
}
