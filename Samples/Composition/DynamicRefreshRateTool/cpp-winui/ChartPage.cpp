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
#include "ChartPage.h"
#if __has_include("ChartPage.g.cpp")
#include "ChartPage.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;
using namespace Microsoft::UI::Xaml::Controls;
using namespace Windows::Foundation;

namespace winrt::DynamicRefreshRateTool::implementation
{
	ChartPage::ChartPage()
	{
		InitializeComponent();
	}

	float ChartPage::QpcTimeToXCoordinate(int64_t qpcTime)
	{
		if (m_fpsHistory.empty())
		{
			return 0;
		}

		float width = static_cast<float>(FpsCanvas().ActualWidth());
		float relativeTime = static_cast<float>(m_fpsHistory.back().tick - qpcTime) / RefreshRateMeter::Instance().GetFrequency();
		return width - relativeTime * width / m_displayHistorySeconds;
	}

	float ChartPage::FpsToYCoordinate(float fps)
	{
		float height = static_cast<float>(FpsCanvas().ActualHeight());
		return height - fps / m_maxFps / 1.3f * height;
	}

	void ChartPage::RefreshChart()
	{

		m_fpsHistory = RefreshRateMeter::Instance().GetRecentHistory(0, static_cast<int64_t>(m_displayHistorySeconds) * RefreshRateMeter::Instance().GetFrequency(), m_aggregationSize);

		if (m_fpsHistory.empty())
		{
			return;
		}

		m_maxFps *= 0.95f;
		for (auto& fpsValue : m_fpsHistory)
		{
			m_maxFps = max(m_maxFps, fpsValue.refreshRate);
		}

		RefreshDashedLines();

		auto points = FpsChart().Points();

		points.Clear();

		Point firstPoint = { 0, FpsToYCoordinate(m_fpsHistory[0].refreshRate) };
		points.Append(firstPoint);

		for (auto& fpsValue : m_fpsHistory)
		{
			points.Append({ QpcTimeToXCoordinate(fpsValue.tick), FpsToYCoordinate(fpsValue.refreshRate) });
		}

		points.Append({ (float)FpsCanvas().ActualWidth(), FpsToYCoordinate(m_fpsHistory.back().refreshRate) });

		points.Append({ (float)FpsCanvas().ActualWidth() + 10, (float)FpsCanvas().ActualHeight() + 10 });
		points.Append({ -10, (float)FpsCanvas().ActualHeight() + 10 });

		RefreshTooltip();
	}

	void ChartPage::RefreshDashedLines()
	{
		DashLine().Points().Clear();

		bool leftToRight = 0;
		for (float level : {60.0f, 120.0f, 240.0f})
		{
			float y = FpsToYCoordinate(level);

			if (y < 0) continue;

			DashLine().Points().Append({ leftToRight ? -10.0f : (float)FpsCanvas().ActualWidth() + 10.0f, y });
			DashLine().Points().Append({ !leftToRight ? -10.0f : (float)FpsCanvas().ActualWidth() + 10.0f, y });

			leftToRight = !leftToRight;
		}
	}

	void ChartPage::RefreshTooltip()
	{
		// Place tooltip offscreen
		FpsCanvas().SetLeft(FpsTooltip(), -1000);
		FpsCanvas().SetLeft(FpsIndicator(), -1000);

		if (!m_mousePosition.has_value())
		{
			return;
		}

		float nearestDistance = 1e9;
		RefreshRateMeter::DataPoint nearestFpsValue = { 0, 0 };

		for (auto fpsValue : m_fpsHistory)
		{
			float distance = fabs(QpcTimeToXCoordinate(fpsValue.tick) - m_mousePosition->X);
			if (distance < nearestDistance)
			{
				nearestDistance = distance;
				nearestFpsValue = fpsValue;
			}
		}

		// Place tooltip to new coordinbates
		float y = FpsToYCoordinate(nearestFpsValue.refreshRate);
		FpsCanvas().SetLeft(FpsTooltip(), m_mousePosition->X);
		FpsCanvas().SetTop(FpsTooltip(), y - 30);
		FpsCanvas().SetLeft(FpsIndicator(), m_mousePosition->X - 3);
		FpsCanvas().SetTop(FpsIndicator(), y - 3);
		FpsTooltipText().Text(winrt::to_hstring(RefreshRateMeter::RefreshRateToString(nearestFpsValue.refreshRate)) + winrt::to_hstring(" FPS"));
	}

	void ChartPage::FpsCanvas_PointerMoved(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& e)
	{
		m_mousePosition = e.GetCurrentPoint(*this).Position();
		RefreshTooltip();
	}

	void ChartPage::FpsCanvas_PointerExited(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::Input::PointerRoutedEventArgs const& /*e*/)
	{
		m_mousePosition = std::nullopt;
		RefreshTooltip();
	}

	void ChartPage::Slider_ValueChanged(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
	{
		m_displayHistorySeconds = static_cast<float>(e.NewValue());
	}

	void ChartPage::Slider_ValueChanged_1(winrt::Windows::Foundation::IInspectable const& /*sender*/, winrt::Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const& e)
	{
		m_aggregationSize = static_cast<int>(e.NewValue());
	}
}
