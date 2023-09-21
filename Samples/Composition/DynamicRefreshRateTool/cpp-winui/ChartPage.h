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

#pragma once

#include "ChartPage.g.h"

namespace winrt::DynamicRefreshRateTool::implementation
{
	struct ChartPage : ChartPageT<ChartPage>
	{
		ChartPage();

		// Mouse events over canvas.
		void FpsCanvas_PointerExited(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);
		void FpsCanvas_PointerMoved(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e);

		// Sliders events.
		void Slider_ValueChanged(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);
		void Slider_ValueChanged_1(winrt::Windows::Foundation::IInspectable const& sender, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e);

		void RefreshChart();

	private:

		// Helper functions to convert fps data values to coordinates on canvas.
		float QpcTimeToXCoordinate(int64_t fpsValue);
		float FpsToYCoordinate(float fps);

		// Methods that refresh canvas content.
		void RefreshDashedLines();
		void RefreshTooltip();

		// Timer that awakes to refresh chart.
		std::future<void> m_monitorFuture;
		bool running = true;

		// Optional mouse position. Set to nullopt if mouse is not over the canvas.
		std::optional<Windows::Foundation::Point> m_mousePosition;

		// Latest fps data history and query parameters that can be tweaked in UI.
		std::vector<RefreshRateMeter::DataPoint> m_fpsHistory;
		float m_displayHistorySeconds = 16.0f;
		int m_aggregationSize = 1;

		// Maximal fps value in recorded history.
		float m_maxFps = 60.0f;
	};
}

namespace winrt::DynamicRefreshRateTool::factory_implementation
{
    struct ChartPage : ChartPageT<ChartPage, implementation::ChartPage>
    {
    };
}
