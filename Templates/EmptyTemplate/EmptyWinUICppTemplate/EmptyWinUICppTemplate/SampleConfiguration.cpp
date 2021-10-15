// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#include "pch.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h" 

using namespace winrt;
using namespace winrt::EmptyWinUICppTemplate;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation::Collections;

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
	{
		Scenario{ L"Scenario 1", winrt::hstring(winrt::name_of<EmptyWinUICppTemplate::Scenario1_ShortName>())},
		Scenario{ L"Scenario 2", winrt::hstring(winrt::name_of<EmptyWinUICppTemplate::Scenario2_ShortName>())}
	});

hstring SampleConfig::FeatureName{ L"EmptyWinUICppTemplate" };
ElementTheme SampleConfig::CurrentTheme{ ElementTheme::Default };
