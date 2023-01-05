// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario1_GetStatus.xaml.h"
#if __has_include("Scenario1_GetStatus.g.cpp")
#include "Scenario1_GetStatus.g.cpp"
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
    Scenario1_GetStatus::Scenario1_GetStatus()
    {
        InitializeComponent();
    }

	void Scenario1_GetStatus::GetStatusButton_Click(IInspectable const& sender, RoutedEventArgs const& e)
	{
		UNREFERENCED_PARAMETER(sender);
		UNREFERENCED_PARAMETER(e);

		resultStatus().Text(L"Result Status");
		resultExtendedError().Text(L"Result ExtendedError:");
		resultImplication().Text(L"");

		// GetStatus() is a fast check to see if all of the packages the WindowsAppRuntime
		// requires and expects are present in in an Ok state.
		DeploymentResult result = DeploymentManager::GetStatus();

		// The two properties for a result include the resulting status and the extended
		// error code. The Status is usually the property of interest, as the ExtendedError is
		// typically used for diagnostic purposes or troubleshooting.
		// 
		//hstring stringResult = unbox_value<hstring>(result);
		//DeploymentStatus status = result.Status();

		resultStatus().Text(L"Result Status: "); // TODO how to convert DeploymentStatus enum to string?
		resultExtendedError().Text(L"Result ExtendedError: " + to_hstring(result.ExtendedError()));

		// Check the resulting Status.
		if (result.Status() == DeploymentStatus::Ok)
		{
			resultImplication().Text(L"The Windows App Runtime is ready for use!");
		}
		else
		{
			// A not-Ok status means it is not ready for us. The Status will indicate the
			// reason it is not Ok, such as some packages need to be installed.
			resultImplication().Text(L"The Windows App Runtime is not ready for use.");
		}
	}
}
