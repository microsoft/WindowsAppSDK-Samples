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

using namespace winrt::DynamicRefreshRateTool;
using namespace std::chrono;

std::wstring GetFullPathForLoggingFile(std::wstring folderPath)
{
	std::time_t t = std::time(nullptr);
	std::tm tm;
	::localtime_s(&tm, &t);
	std::stringstream stream;
	stream << std::put_time(&tm, "%d-%m-%Y %H-%M-%S");
	auto str = stream.str();
	return folderPath + L"\\" + L"FPS Monitor " + std::wstring(str.begin(), str.end()) + L".txt";
}

RefreshRateLogger::RefreshRateLogger(std::wstring folderPath) :m_out(GetFullPathForLoggingFile(folderPath))
{
	WriteLog("Logging started.");

	m_loggerFuture = std::async(std::launch::async | std::launch::deferred,
		[this]()
		{
			int64_t startingFrom = RefreshRateMeter::Instance().GetCurrentTick();

			int levels = 4;
			int fpsLevels[] = { 0, 60, 120, 240 };
			int fpsLevelsFrames[] = { 0, 0, 0, 0, 0 };
			int prevLevel = 0;
			int total = 0;

			while (!m_stop)
			{
				auto history = RefreshRateMeter::Instance().GetHistoryStartingFrom(startingFrom);
				for (auto& data : history)
				{
					int curLevel = 0;
					total++;
					for (int i = 1; i < levels; i++)
					{
						if (fabs(fpsLevels[i] - data.refreshRate) < 10)
						{
							curLevel = i;
							break;
						}
					}

					fpsLevelsFrames[curLevel]++;

					if (curLevel != 0)
					{
						if (curLevel != prevLevel)
						{
							WriteLog("Running at " + std::to_string(fpsLevels[curLevel]) + "hz");
						}

						prevLevel = curLevel;
					}
				}

				if (!history.empty())
				{
					startingFrom = history.back().tick;
				}

				std::this_thread::sleep_for(std::chrono::milliseconds(200));
			}

			WriteLog("Logging finished.");
			WriteLog("Total number of frames captured: " + std::to_string(total));
			for (int i = 1; i < 4; i++)
			{
				WriteLog("Number of frames at " + std::to_string(fpsLevels[i]) + "hz: " + std::to_string(fpsLevelsFrames[i]) + " (~" + std::to_string(fpsLevelsFrames[i] * 100 / total) + "%)");
			}
			WriteLog("Other frames: " + std::to_string(fpsLevelsFrames[0]) + " (~" + std::to_string(fpsLevelsFrames[0] * 100 / total) + "%)");
		}
	);
}

RefreshRateLogger::~RefreshRateLogger()
{
	m_stop = true;
	m_loggerFuture.wait();
}

void RefreshRateLogger::BoostStateChanged(bool enabled)
{
	WriteLog(enabled ? "Boost enabled." : "Boost disabled.");
}

void RefreshRateLogger::WriteLog(std::string message)
{
	std::lock_guard<std::mutex> lock(m_mutex);

	std::time_t t = std::time(nullptr);
	std::tm tm;
	::localtime_s(&tm, &t);

	m_out << "[" << std::put_time(&tm, "%d-%m-%Y %H:%M:%S") << "] " << message << std::endl;
}
