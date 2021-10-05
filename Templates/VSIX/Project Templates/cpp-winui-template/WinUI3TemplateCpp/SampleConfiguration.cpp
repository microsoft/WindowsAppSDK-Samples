//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once
#include "pch.h"
#include "SampleConfiguration.h"
#include "MainPage.xaml.h" 

using namespace winrt;
using namespace winrt::$safeprojectname$;
using namespace Microsoft::UI::Xaml;
using namespace Windows::Foundation::Collections;

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
	{
		Scenario{ L"Scenario 1", winrt::hstring(winrt::name_of<$safeprojectname$::Scenario1_ShortName>())},
		Scenario{ L"Scenario 2", winrt::hstring(winrt::name_of<$safeprojectname$::Scenario2_ShortName>())}
	});

hstring Settings::FeatureName{ L"$safeprojectname$" };
ElementTheme Settings::CurrentTheme{ ElementTheme::Default };
