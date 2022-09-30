// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario3_Repair.xaml.h"
#if __has_include("Scenario3_Repair.g.cpp")
#include "Scenario3_Repair.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::Windows::ApplicationModel::WindowsAppRuntime;
    using namespace Windows::Foundation;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::DeploymentManagerSample::implementation
{
    Scenario3_Repair::Scenario3_Repair()
    {
        InitializeComponent();
    }

    void Scenario3_Repair::UpdateRepairMessages()
    {
        resultStatus().Text(L"Result Status: Running Repair()...");
        resultExtendedError().Text(L"Result ExtendedError:");
        resultImplication().Text(L"");
    }

    void Scenario3_Repair::UpdateDeploymentResultMessages(DeploymentResult deploymentResult)
    {
        resultStatus().Text(L"Result Status: "); // TODO DeploymentStatus to string

        // Check the result.
        if (deploymentResult.Status() == DeploymentStatus::Ok)
        {
            resultImplication().Text(L"The WindowsAppRuntime was successfully Repaired and is now ready for use!");

        }
        else
        {
            resultExtendedError().Text(L"Result ExtendedError: " + to_hstring(deploymentResult.ExtendedError()));

            // The WindowsAppRuntime is in a bad state which Repair() did not fix.
            // Do error reporting or gather information for submitting a bug.
            // Gracefully exit the program or carry on without using the WindowsAppRuntime.
            resultImplication().Text(L"Repair() failed to ensure the WindowsAppRuntime.");
        }
    }

    void Scenario3_Repair::RepairScenarioButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        UpdateRepairMessages();

        // Repair does NOT do a status check and it will attempt to repair the
        // WindowsAppRuntime into a good state by deploying packages. Unlike a simple
        // status check, Repair can sometimes take several seconds to deploy the packages.
        // These should be run on a separate thread so as not to hang your app while the
        // packages deploy. 
        DeploymentResult deploymentResult = DeploymentManager::Repair(); // TODO how to run on separate thread?

        UpdateDeploymentResultMessages(deploymentResult);
    }

    void Scenario3_Repair::ForceRepairScenarioButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        UpdateRepairMessages();

        if (DeploymentManager::GetStatus().Status() == DeploymentStatus::Ok)
        {
            // Set force deploy option to true. This will shut down any proccesses associated
            // with the Main and Singleton packages if they are currently in use.
            DeploymentRepairOptions deploymentRepairOptions;
            deploymentRepairOptions.ForceDeployment(true);

            // Repair does a status check, and if the status is not Ok it will attempt to get
            // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
            // status check, Repair can sometimes take several seconds to deploy the packages.
            // These should be run on a separate thread so as not to hang your app while the
            // packages deploy. 
            DeploymentResult deploymentResult = DeploymentManager::Repair(deploymentRepairOptions); // TODO how to run on separate thread?

            UpdateDeploymentResultMessages(deploymentResult);
        }
        else
        {
            resultImplication().Text(L"The WindowsAppRuntime was already in an Ok status, no action taken.");
        }
    }
}
