// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.ApplicationModel.WindowsAppRuntime;
using System.Threading.Tasks;

namespace DeploymentManagerSample
{
    public partial class Scenario2_Initialize : Page
    {
        public Scenario2_Initialize()
        {
            this.InitializeComponent();
        }

        private void updateInitializeMessages()
        {
            resultStatus.Text = "Result Status: Running Initialize()...";
            resultExtendedError.Text = "Result ExtendedError:";
            resultImplication.Text = "";
        }

        private void updateDeploymentResultMessages(DeploymentResult deploymentResult)
        {
            resultStatus.Text = "Result Status: " + deploymentResult.Status.ToString();

            // Check the result.
            if (deploymentResult.Status == DeploymentStatus.Ok)
            {
                resultImplication.Text = "The WindowsAppRuntime was successfully initialized and is now ready for use!";

            }
            else
            {
                resultExtendedError.Text = "Result ExtendedError: " + deploymentResult.ExtendedError.ToString();

                // The WindowsAppRuntime is in a bad state which Initialize() did not fix.
                // Do error reporting or gather information for submitting a bug.
                // Gracefully exit the program or carry on without using the WindowsAppRuntime.
                resultImplication.Text = "Initialize() failed to ensure the WindowsAppRuntime.";
            }
        }

        private void InitializeScenarioButton_Click(object sender, RoutedEventArgs e)
        {
            updateInitializeMessages();

            if (DeploymentManager.GetStatus().Status != DeploymentStatus.Ok)
            {
                // Initialize does a status check, and if the status is not Ok it will attempt to get
                // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
                // status check, Initialize can sometimes take several seconds to deploy the packages.
                // These should be run on a separate thread so as not to hang your app while the
                // packages deploy. 
                var initializeTask = Task.Run(() => DeploymentManager.Initialize());
                initializeTask.Wait();

                updateDeploymentResultMessages(initializeTask.Result);
            }
            else
            {
                resultImplication.Text = "The WindowsAppRuntime was already in an Ok status, no action taken.";
            }
        }

        private void ForceInitializeScenarioButton_Click(object sender, RoutedEventArgs e)
        {
            updateInitializeMessages();

            if (DeploymentManager.GetStatus().Status != DeploymentStatus.Ok)
            {
                // Set force deploy option to true. This will shut down any proccesses associated
                // with the Main and Singleton packages if they are currently in use.
                DeploymentInitializeOptions deploymentInitializeOptions = new() {
                    ForceDeployment = true
                };

                // Initialize does a status check, and if the status is not Ok it will attempt to get
                // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
                // status check, Initialize can sometimes take several seconds to deploy the packages.
                // These should be run on a separate thread so as not to hang your app while the
                // packages deploy. 
                var initializeTask = Task.Run(() => DeploymentManager.Initialize(deploymentInitializeOptions));
                initializeTask.Wait();

                updateDeploymentResultMessages(initializeTask.Result); 
            }
            else
            {
                resultImplication.Text = "The WindowsAppRuntime was already in an Ok status, no action taken.";
            }
        }
    }
}
