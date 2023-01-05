// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario2_Initialize.xaml.h"
#if __has_include("Scenario2_Initialize.g.cpp")
#include "Scenario2_Initialize.g.cpp"
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
    Scenario2_Initialize::Scenario2_Initialize()
    {
        InitializeComponent();
    }

    void Scenario2_Initialize::UpdateInitializeMessages()
    {
        resultStatus().Text(L"Result Status: Running Initialize()...");
        resultExtendedError().Text(L"Result ExtendedError:");
        resultImplication().Text(L"");
    }

    void Scenario2_Initialize::UpdateDeploymentResultMessages(DeploymentResult deploymentResult)
    {
        resultStatus().Text(L"Result Status: "); // TODO DeploymentStatus to string

        // Check the result.
        if (deploymentResult.Status() == DeploymentStatus::Ok)
        {
            resultImplication().Text(L"The WindowsAppRuntime was successfully initialized and is now ready for use!");

        }
        else
        {
            resultExtendedError().Text(L"Result ExtendedError: " + to_hstring(deploymentResult.ExtendedError()));

            // The WindowsAppRuntime is in a bad state which Initialize() did not fix.
            // Do error reporting or gather information for submitting a bug.
            // Gracefully exit the program or carry on without using the WindowsAppRuntime.
            resultImplication().Text(L"Initialize() failed to ensure the WindowsAppRuntime.");
        }
    }

    void Scenario2_Initialize::InitializeScenarioButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        UpdateInitializeMessages();

        if (DeploymentManager::GetStatus().Status() != DeploymentStatus::Ok)
        {
            // Initialize does a status check, and if the status is not Ok it will attempt to get
            // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
            // status check, Initialize can sometimes take several seconds to deploy the packages.
            // These should be run on a separate thread so as not to hang your app while the
            // packages deploy. 
            DeploymentResult deploymentResult = DeploymentManager::Initialize(); // TODO how to run on separate thread?
           
            UpdateDeploymentResultMessages(deploymentResult);
        }
        else
        {
            resultImplication().Text(L"The WindowsAppRuntime was already in an Ok status, no action taken.");
        }
    }

    void Scenario2_Initialize::ForceInitializeScenarioButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
    {
        UNREFERENCED_PARAMETER(sender);
        UNREFERENCED_PARAMETER(e);

        UpdateInitializeMessages();

        if (DeploymentManager::GetStatus().Status() != DeploymentStatus::Ok)
        {
            // Set force deploy option to true. This will shut down any proccesses associated
            // with the Main and Singleton packages if they are currently in use.
            DeploymentInitializeOptions deploymentInitializeOptions;
            deploymentInitializeOptions.ForceDeployment(true);

            // Initialize does a status check, and if the status is not Ok it will attempt to get
            // the WindowsAppRuntime into a good state by deploying packages. Unlike a simple
            // status check, Initialize can sometimes take several seconds to deploy the packages.
            // These should be run on a separate thread so as not to hang your app while the
            // packages deploy. 
            DeploymentResult deploymentResult = DeploymentManager::Initialize(deploymentInitializeOptions); // TODO how to run on separate thread?

            UpdateDeploymentResultMessages(deploymentResult);
        }
        else
        {
            resultImplication().Text(L"The WindowsAppRuntime was already in an Ok status, no action taken.");
        }
    }
}
