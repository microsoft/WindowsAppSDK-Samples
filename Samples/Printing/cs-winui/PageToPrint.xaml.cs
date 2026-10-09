// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

namespace PrintSample
{
    /// <summary>
    /// Page content to send to the printer
    /// </summary>
    public sealed partial class PageToPrint : Page
    {
        public RichTextBlock TextContentBlock { get; set; }

        public PageToPrint()
        {
            this.InitializeComponent();
            TextContentBlock = TextContent;
        }
    }
}
