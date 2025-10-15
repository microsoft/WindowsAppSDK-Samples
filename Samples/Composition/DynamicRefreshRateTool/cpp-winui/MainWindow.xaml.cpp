//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
// THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//*********************************************************

#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::DynamicRefreshRateTool::implementation
{

	MainWindow::MainWindow()
	{
		InitializeComponent();

		ExtendsContentIntoTitleBar(true);
		SetTitleBar(AppTitleBar());

        // ===================================================================================
        // Disable the virtualization for the application before the main SwapChain is created
        // ===================================================================================
		DXGIDisableVBlankVirtualization();

		m_updateFuture = std::async(std::launch::async | std::launch::deferred,
			[this]()
			{
				while (running)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(75));
					DispatcherQueue().TryEnqueue(
						[this]()
						{
							FpsText().Text(winrt::to_hstring(RefreshRateMeter::RefreshRateToString(RefreshRateMeter::Instance().GetCurrentRefreshRate())));
							if (Chart().Visibility() == Visibility::Visible)
							{
								Chart().RefreshChart();
							}
						}
					);
				}
			}
		);

		Closed(
			[&](auto, auto)
			{
				this->running = false;
				m_updateFuture.wait();
			}
		);
	}

	void MainWindow::Page_KeyUp(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::Input::KeyRoutedEventArgs const& e)
	{
		if (e.Key() == winrt::Windows::System::VirtualKey::B)
		{
			BoostToggleSwitch().IsOn(!BoostToggleSwitch().IsOn());
		}
		if (e.Key() == winrt::Windows::System::VirtualKey::L && LoggingToggleSwitch().IsEnabled())
		{
			LoggingToggleSwitch().IsOn(!LoggingToggleSwitch().IsOn());
		}
	}

	winrt::Windows::Foundation::IAsyncAction MainWindow::ChooseFolder_Click(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& e)
	{
		Microsoft::Windows::Storage::Pickers::FolderPicker picker{ this->AppWindow().Id() };
		picker.ViewMode(Microsoft::Windows::Storage::Pickers::PickerViewMode::List);
		picker.SuggestedStartLocation(Microsoft::Windows::Storage::Pickers::PickerLocationId::ComputerFolder);

		if (auto result = co_await picker.PickSingleFolderAsync())
		{
			// Store the picked folder path directly as a string
			auto folderPath = result.Path();
			m_loggingFolderPath = std::wstring{ folderPath };
			FolderPath().Text(folderPath);
			FolderPath().Visibility(Visibility::Visible);
			FolderNotSelected().Visibility(Visibility::Collapsed);
			LoggingToggleSwitch().IsEnabled(true);
		}

	}

	void MainWindow::BoostToggled(IInspectable const& /*sender*/, RoutedEventArgs const& /*args*/)
	{
		// =================================================
		// Request system to switch to a higher refresh rate
		// =================================================
		DCompositionBoostCompositorClock(BoostToggleSwitch().IsOn());
		
		if (IsLogging())
		{
			m_logger->BoostStateChanged(BoostToggleSwitch().IsOn());
		}
	}

	void MainWindow::StartLogging()
	{
		m_logger.emplace(m_loggingFolderPath);
	}

	void MainWindow::StopLogging()
	{
		m_logger = std::nullopt;
	}

	bool MainWindow::IsLogging()
	{
		return LoggingToggleSwitch().IsOn() && m_logger;
	}

	void MainWindow::LoggingToggleSwitch_Toggled(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::RoutedEventArgs const& /*e*/)
	{
		if (LoggingToggleSwitch().IsOn())
		{
			StartLogging();
		}
		else
		{
			StopLogging();
		}
	}
}
