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
using namespace winrt::WinUI3TemplateCpp;
using namespace Windows::Foundation::Collections;

IVector<Scenario> implementation::MainPage::scenariosInner = winrt::single_threaded_observable_vector<Scenario>(
	{
		Scenario{ L"Scenario 1", winrt::hstring(winrt::name_of<WinUI3TemplateCpp::Scenario1_ShortName>()) },
		Scenario{ L"Scenario 2", winrt::hstring(winrt::name_of<WinUI3TemplateCpp::Scenario2_ShortName>())}
	});

hstring Settings::FeatureName{ L"Windows App SDK Template C++ Sample" };
hstring Settings::CurrentTheme{ L"" };