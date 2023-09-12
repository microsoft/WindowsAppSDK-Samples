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

using namespace winrt;
using namespace winrt::DynamicRefreshRateTool;

RefreshRateMeter::RefreshRateMeter()
{
	LARGE_INTEGER frequency = {};
	QueryPerformanceFrequency(&frequency);
	m_frequency = frequency.QuadPart;

	// Start monitor thread.
	m_monitorFuture = std::async(std::launch::async | std::launch::deferred, [this] { RefreshRateTrackingThread(); });
}

RefreshRateMeter::~RefreshRateMeter()
{
	m_shouldStopFpsCalculation = true;
	m_monitorFuture.wait();
}

float RefreshRateMeter::GetCurrentRefreshRate() const
{
	if (m_deltaHistory.empty())
	{
		// If there is no history yet - return default 60 FPS.
		return 60.0;
	}

	return static_cast<float>(m_frequency) / m_deltaHistory.back().second;
}

int64_t RefreshRateMeter::GetLastFrameDeltaTicks() const
{
	if (m_deltaHistory.empty())
	{
		// If there is no history yet - return default 1/60 delta.
		return m_frequency / 60;
	}

	return m_deltaHistory.back().second;
}

int64_t RefreshRateMeter::GetFrequency() const
{
	return m_frequency;
}

int64_t RefreshRateMeter::GetCurrentTick() const
{
	LARGE_INTEGER currentTime;
	QueryPerformanceCounter(&currentTime);
	return currentTime.QuadPart;
}

RefreshRateMeter& RefreshRateMeter::Instance()
{
	static RefreshRateMeter RefreshRateMeter;
	return RefreshRateMeter;
}

std::vector<RefreshRateMeter::DataPoint> RefreshRateMeter::GetRecentHistory(int64_t offsetTicks, int64_t historyLengthTicks, int aggregationSize, int keepEach) const
{
	if (m_deltaHistory.empty())
	{
		return {};
	}

	std::lock_guard<std::mutex> guard(m_mutex);

	int lastFrameIndex = static_cast<int>(m_deltaHistory.size()) - 1;
	for (int64_t accumulatedTotal = 0; lastFrameIndex >= 0 && accumulatedTotal + m_deltaHistory[lastFrameIndex].second <= offsetTicks; lastFrameIndex--) {
		accumulatedTotal += m_deltaHistory[lastFrameIndex].second;
	}

	if (lastFrameIndex < 0)
	{
		return {};
	}

	int firstFrameIndex = lastFrameIndex;
	for (int64_t accumulatedTotal = 0; firstFrameIndex >= 0 && accumulatedTotal + m_deltaHistory[firstFrameIndex].second <= historyLengthTicks; firstFrameIndex--) {
		accumulatedTotal += m_deltaHistory[firstFrameIndex].second;
	}

	if (firstFrameIndex < 0)
	{
		firstFrameIndex = 0;
	}

	std::vector<RefreshRateMeter::DataPoint> res;

	int64_t aggregatedDelta = 0;
	int64_t aggregatedFramesNumber = 0;

	for (int i = lastFrameIndex; i > lastFrameIndex - aggregationSize + 1 && i >= 0; i--)
	{
		aggregatedDelta += m_deltaHistory[i].second;
		aggregatedFramesNumber++;
	}

	for (int i = lastFrameIndex; i >= firstFrameIndex; i--)
	{
		if (i - aggregationSize + 1 >= 0)
		{
			aggregatedDelta += m_deltaHistory[i - aggregationSize + 1].second;
			aggregatedFramesNumber++;
		}

		if ((m_currentFrame - (m_deltaHistory.size() - i - 1)) % keepEach == 0)
		{
			float refreshRate = static_cast<float>(aggregatedFramesNumber * m_frequency) / aggregatedDelta;
			res.push_back(DataPoint{ m_deltaHistory[i].first, aggregatedDelta, aggregatedFramesNumber, refreshRate });
		}

		aggregatedDelta -= m_deltaHistory[i].second;
		aggregatedFramesNumber--;
	}

	std::reverse(res.begin(), res.end());

	return res;
}

std::vector<RefreshRateMeter::DataPoint> RefreshRateMeter::GetHistoryStartingFrom(int64_t startingFrom) const
{
	std::lock_guard<std::mutex> guard(m_mutex);
	std::vector<DataPoint> res;
	for (int i = (int)m_deltaHistory.size() - 1; i >= 0 && m_deltaHistory[i].first > startingFrom; i--)
	{
		res.push_back(DataPoint{ m_deltaHistory[i].first, m_deltaHistory[i].second, 1, static_cast<float>(m_frequency) / m_deltaHistory[i].second });
	}

	std::reverse(res.begin(), res.end());

	return res;
}

std::string RefreshRateMeter::RefreshRateToString(float fpsValue)
{
	std::stringstream stream;
	// One digit fixed precision (like "60.0").
	stream << std::fixed << std::setprecision(1) << fpsValue;
	return stream.str();
}

void RefreshRateMeter::RefreshRateTrackingThread()
{
	LARGE_INTEGER prevTime = {};

	while (!m_shouldStopFpsCalculation)
	{
		DCompositionWaitForCompositorClock(0, nullptr, INFINITE);
		LARGE_INTEGER currentTime{};
		QueryPerformanceCounter(&currentTime);

		if (prevTime.QuadPart != 0)
		{
			std::lock_guard<std::mutex> guard(m_mutex);
			m_deltaHistory.push_back({ currentTime.QuadPart, currentTime.QuadPart - prevTime.QuadPart });

			if (m_deltaHistory.size() > MAX_HISTORY_SIZE)
			{
				m_deltaHistory.pop_front();
			}
		}

		prevTime = currentTime;
		m_currentFrame++;
	}
}
