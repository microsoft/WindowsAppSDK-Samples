// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using System.Threading.Tasks;
using Microsoft.Windows.ApplicationModel.WindowsAppRuntime;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace SampleApp
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
            this.Title = "WindowsAppRuntime DeploymentManager C# Sample App";
            scenario.Text = "Please click a scenario to run.";
            resultStatus.Text = "Result Status:";
            resultExtendedError.Text = "Result ExtendedError:";
            resultImplication.Text = "";
        }

        private void getStatusScenarioButton_Click(object sender, RoutedEventArgs e)
        {
            scenario.Text = "Scenario: DeploymentManager.GetStatus()";
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
            resultExtendedError.Text = "Result ExtendedError: " + result.ExtendedError.ToString();

            // Check the resulting Status.
            if (result.Status == DeploymentStatus.Ok)
            {
                resultImplication.Text = "The WindowsAppRuntime is ready for use!";
            }
            else
            {
                // A not-Ok status means it is not ready for us. The Status will indicate the
                // reason it is not Ok, such as some packages need to be installed.
                resultImplication.Text = "The WindowsAppRuntime is not ready for use.";
            }
        }

        private void initializeScenarioButton_Click(object sender, RoutedEventArgs e)
        {
            scenario.Text = "Scenario: DeploymentManager.Initialize()";
            resultStatus.Text = "Result Status: Running Initialize()...";
            resultExtendedError.Text = "Result ExtendedError:";
            resultImplication.Text = "";

            if (DeploymentManager.GetStatus().Status != DeploymentStatus.Ok)
            {
                // Initialize does a status check, and if the status is not Ok it will attempt to get
                // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
                // status check, Initialize can sometimes take several seconds to deploy the packages.
                // These should be run on a separate thread so as not to hang your app while the
                // packages deploy. 
                var initializeTask = Task.Run(() => DeploymentManager.Initialize());
                initializeTask.Wait();
                resultStatus.Text = "Result Status: " + initializeTask.Result.Status.ToString();
                if (initializeTask.Result.Status == DeploymentStatus.Ok)
                {
                    resultImplication.Text = "The WindowsAppRuntime was successfully initialized and is now ready for use!";
                }
                else
                {
                    resultExtendedError.Text = "Result ExtendedError: " + initializeTask.Result.ExtendedError.ToString();
                    // The WindowsAppRuntime is in a bad state which Initialize() did not fix.
                    // Do error reporting or gather information for submitting a bug.
                    // Gracefully exit the program or carry on without using the WindowsAppRuntime.
                    resultImplication.Text = "Initialize() failed to ensure the WindowsAppRuntime.";
                }
            }
            else
            {
                resultImplication.Text = "The WindowsAppRuntime was already in an Ok status, no action taken.";
            }
        }
    }
}
