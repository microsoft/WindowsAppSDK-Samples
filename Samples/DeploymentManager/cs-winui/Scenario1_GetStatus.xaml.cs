// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.ApplicationModel.WindowsAppRuntime;

namespace DeploymentManagerSample
{
    public partial class Scenario1_GetStatus : Page
    {
        private MainPage rootPage = MainPage.Current;

        public Scenario1_GetStatus()
        {
            this.InitializeComponent();
        }
        private void GetStatus_Click(object sender, RoutedEventArgs e)
        {
            resultStatus.Text = "Result Status:";
            resultExtendedError.Text = "Result ExtendedError:";
            resultImplication.Text = "";

            // GetStatus() is a fast check to see if all of the packages the WindowsAppRuntime
            // requires and expects are present in in an Ok state.
            DeploymentResult result = DeploymentManager.GetStatus();

            // The two properties for a result include the resulting status and the extended
            // error code. The Status is usually the property of interest, as the ExtendedError is
            // typically used for diagnostic purposes or troubleshooting. 
            resultStatus.Text = "Result Status: " + result.Status.ToString();

            // Check the resulting Status.
            if (result.Status == DeploymentStatus.Ok)
            {
                resultImplication.Text = "The Windows App Runtime is ready for use!";
            }
            else
            {
                resultExtendedError.Text = "Result ExtendedError: " + result.ExtendedError.ToString();
                // A not-Ok status means it is not ready for us. The Status will indicate the
                // reason it is not Ok, such as some packages need to be installed.
                resultImplication.Text = "The Windows App Runtime is not ready for use.";
            }
        }
    }
}
