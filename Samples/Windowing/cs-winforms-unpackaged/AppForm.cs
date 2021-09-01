// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE in the project root for license information.

using System;
using System.Windows.Forms;
using Microsoft.ApplicationModel.Resources;

namespace winforms_unpackaged_app
{
    public partial class AppForm : Form
    {
        public AppForm(ResourceManager manager, ResourceLoader loader, ResourceContext resourceOverrideContext)
        {
            m_resourceManager = manager;
            m_resourceLoader = loader;
            m_resourceOverrideContext = resourceOverrideContext;
            InitializeComponent();
        }

        private void DefaultLanguageButton_Click(object sender, EventArgs e)
        {
            // The resource loader's default constructor assumes that resources are in the "Resources" scope.
            // Use the non-default constructor to load resources in a different scope.

            SampleStringTextBox.Text = m_resourceLoader.GetString("SampleString");
        }

        private void OverrideLanguageButton_Click(object sender, EventArgs e)
        {
            // The resource manager does not have a default scope and resolves resources based on the root.

            var resourceCandidate = m_resourceManager.MainResourceMap.GetValue("Resources/SampleString", m_resourceOverrideContext);
            SampleStringTextBox.Text = resourceCandidate.ValueAsString;
        }

        private void ResourceFallbackButton_Click(object sender, EventArgs e)
        {
            var resourceCandidate = m_resourceManager.MainResourceMap.GetValue("LegacyString");
            SampleStringTextBox.Text = resourceCandidate.ValueAsString;
        }

        private ResourceManager m_resourceManager;
        private ResourceLoader m_resourceLoader;
        private ResourceContext m_resourceOverrideContext;
    }
}
