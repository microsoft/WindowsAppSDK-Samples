#pragma once
#include "pch.h"

namespace winrt::WinUI3TemplateCpp
{
	static class Settings
	{
	public:
		static hstring FeatureName;
		static hstring CurrentTheme;
	};

	hstring Settings::FeatureName{ L"Windows App SDK Template C# Sample" };
	//hstring Settings::CurrentTheme{ L"" };
}