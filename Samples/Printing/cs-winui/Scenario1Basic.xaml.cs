// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.Graphics.Printing;
using SDKTemplate;

namespace PrintSample
{
    /// <summary>
    /// Scenario that demonstrates how to invoke the Print UI
    /// </summary>
    public sealed partial class Scenario1Basic : Page
    {
        private PrintHelper printHelper;

        public Scenario1Basic()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// This is the click handler for the 'Print' button.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private async void OnPrintButtonClick(object sender, RoutedEventArgs e)
        {
            await printHelper.ShowPrintUIAsync();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (PrintManager.IsSupported())
            {
                // Tell the user how to print
                MainPage.Current.NotifyUser("Print contract registered with customization, use the Print button to print.", NotifyType.StatusMessage);
            }
            else
            {
                // Remove the print button
                InvokePrintingButton.Visibility = Visibility.Collapsed;

                // Inform user that Printing is not supported
                MainPage.Current.NotifyUser("Printing is not supported.", NotifyType.ErrorMessage);

                // Printing-related event handlers will never be called if printing
                // is not supported, but it's okay to register for them anyway.
            }

            // Initalize common helper class and register for printing
            printHelper = new PrintHelper(this);
            printHelper.RegisterForPrinting();

            // Initialize print content for this scenario
            printHelper.PreparePrintContent(new PageToPrint());
        }

        protected override void OnNavigatedFrom(NavigationEventArgs e)
        {
            if (printHelper != null)
            {
                printHelper.UnregisterForPrinting();
            }
        }
    }
}
