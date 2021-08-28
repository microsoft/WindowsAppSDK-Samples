#pragma once
#include "pch.h"

namespace winrt::WinUI3TemplateCpp
{
	static class Settings
	{
	public:
		static hstring FeatureName;
		hstring CurrentTheme{ L"" };
	};

	hstring Settings::FeatureName{ L"Windows App SDK Template C# Sample" };
}