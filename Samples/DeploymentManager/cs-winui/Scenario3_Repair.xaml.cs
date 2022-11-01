// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.ApplicationModel.WindowsAppRuntime;
using System.Threading.Tasks;

namespace DeploymentManagerSample
{
    public partial class Scenario3_Repair : Page
    {
        public Scenario3_Repair()
        {
            this.InitializeComponent();
        }

        private void updateRepairMessages()
        {
            resultStatus.Text = "Result Status: Running Repair()...";
            resultExtendedError.Text = "Result ExtendedError:";
            resultImplication.Text = "";
        }

        private void updateDeploymentResultMessages(DeploymentResult deploymentResult)
        {
            resultStatus.Text = "Result Status: " + deploymentResult.Status.ToString();

            // Check the result.
            if (deploymentResult.Status == DeploymentStatus.Ok)
            {
                resultImplication.Text = "The WindowsAppRuntime was successfully Repaired and is now ready for use!";

            }
            else
            {
                resultExtendedError.Text = "Result ExtendedError: " + deploymentResult.ExtendedError.ToString();

                // The WindowsAppRuntime is in a bad state which Repair() did not fix.
                // Do error reporting or gather information for submitting a bug.
                // Gracefully exit the program or carry on without using the WindowsAppRuntime.
                resultImplication.Text = "Repair() failed to ensure the WindowsAppRuntime.";
            }
        }

        private void RepairScenarioButton_Click(object sender, RoutedEventArgs e)
        {
            updateRepairMessages();

            if (DeploymentManager.GetStatus().Status != DeploymentStatus.Ok)
            {
                // Repair does NOT check for WindowsAppRuntime status and it will always attempt to get
                // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
                // status check, Repair can sometimes take several seconds to deploy the packages.
                // These should be run on a separate thread so as not to hang your app while the
                // packages deploy. 
                var RepairTask = Task.Run(() => DeploymentManager.Repair());
                RepairTask.Wait();

                updateDeploymentResultMessages(RepairTask.Result);
            }
            else
            {
                resultImplication.Text = "The WindowsAppRuntime was already in an Ok status, no action taken.";
            }
        }

        private void ForceRepairScenarioButton_Click(object sender, RoutedEventArgs e)
        {
            updateRepairMessages();

            if (DeploymentManager.GetStatus().Status != DeploymentStatus.Ok)
            {
                // Set force deploy option to true. This will shut down any proccesses associated
                // with the Main and Singleton packages if they are currently in use.
                DeploymentRepairOptions deploymentRepairOptions = new() {
                    ForceDeployment = true
                };

                // Repair does NOT check for WindowsAppRuntime status and it will always attempt to get
                // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
                // status check, Repair can sometimes take several seconds to deploy the packages.
                // These should be run on a separate thread so as not to hang your app while the
                // packages deploy. 
                var RepairTask = Task.Run(() => DeploymentManager.Repair(deploymentRepairOptions));
                RepairTask.Wait();

                updateDeploymentResultMessages(RepairTask.Result); 
            }
            else
            {
                resultImplication.Text = "The WindowsAppRuntime was already in an Ok status, no action taken.";
            }
        }
    }
}
