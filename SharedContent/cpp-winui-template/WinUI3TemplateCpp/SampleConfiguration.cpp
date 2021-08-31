#pragma once
#include "pch.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h" 

using namespace winrt;
using namespace winrt::WinUI3TemplateCpp;
using namespace Windows::Foundation::Collections;

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
	{
		Scenario{ L"Scenario 1", L"WinUI3TemplateCpp.Scenario1_ShortName"},
		Scenario{ L"Scenario 2", L"WinUI3TemplateCpp.Scenario2_ShortName"}
	});

hstring Settings::FeatureName{ L"Windows App SDK Template C++ Sample" };
hstring Settings::CurrentTheme{ L"" };