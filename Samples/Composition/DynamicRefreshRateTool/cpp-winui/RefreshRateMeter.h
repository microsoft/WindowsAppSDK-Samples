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
#include "pch.h"

namespace winrt::DynamicRefreshRateTool {

	/**
	 * Helper for monitoring dcomp refresh rate (fps) and capturing its history.
	 * Can provide current refresh rate or refresh rate at some previous point in time.
	 */
	class RefreshRateMeter {
	public:
		RefreshRateMeter();

		~RefreshRateMeter();

		// Global instance.
		static RefreshRateMeter& Instance();

		// Get current refresh rate.
		float GetCurrentRefreshRate() const;

		// Get last frame delta in ticks.
		int64_t GetLastFrameDeltaTicks() const;

		// Get timer frequency, or ticks per second.
		int64_t GetFrequency() const;

		// Get current tick.
		int64_t GetCurrentTick() const;

		// History data point.
		struct DataPoint {
			// QPC tick - start of the segment.
			int64_t tick;
			// Number of QPC ticks - duration of the segment.
			int64_t deltaTicks;
			// Number of frames in the segment.
			int64_t framesNumber;
			// Average refresh rate in the segment.
			float refreshRate;
		};

		std::vector<DataPoint> GetRecentHistory(int64_t offsetTicks, int64_t historyLengthTicks, int aggregationSize = 1, int keepEach = 1) const;

		std::vector<DataPoint> GetHistoryStartingFrom(int64_t startingFrom) const;

		// Helper that converts refresh rate float value to a readable string.
		static std::string RefreshRateToString(float fpsValue);

	private:
		void RefreshRateTrackingThread();

		int64_t m_frequency = 0;
		uint64_t m_currentFrame = 0;

		// Up to 10 minutes of history at 60 fps rate
		const size_t MAX_HISTORY_SIZE = 60 * 60 * 10;
		// List of pairs (start frame QPC tick, frame duration QPC ticks) for the last ~10 minutes
		std::deque<std::pair<int64_t, int64_t>> m_deltaHistory;

		// Future that owns monitor thread.
		std::future<void> m_monitorFuture;

		// Flag to stop the monitor thread.
		bool m_shouldStopFpsCalculation = false;

		mutable std::mutex m_mutex;
	};
}