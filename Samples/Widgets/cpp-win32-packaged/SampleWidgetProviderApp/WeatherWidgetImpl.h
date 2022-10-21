﻿// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include "WidgetImplBase.h"

class WeatherWidget : public WidgetImplBase
{
public:
    // Initalize a widget with saved state
    WeatherWidget(winrt::hstring const& id, winrt::hstring const& state);

    void OnActionInvoked(winrt::WidgetActionInvokedArgs actionInvokedArgs);
    void OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs);
    void Activate();
    void Deactivate();
    winrt::hstring GetTemplateForWidget();
    winrt::hstring GetDataForWidget();
private:
    winrt::hstring GetDefaultTemplate();
};

