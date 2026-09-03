// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WeatherWidgetImpl.h"

WeatherWidget::WeatherWidget(winrt::hstring const& id, winrt::hstring const& state) : WidgetImplBase(id, state) {}

// This function wil be invoked when the Increment button was clicked by the user.
void WeatherWidget::OnActionInvoked(winrt::WidgetActionInvokedArgs actionInvokedArgs)
{
}

// This function will be invoked when WidgetContext has changed.
void WeatherWidget::OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs)
{
    // This function is raised when
    // (Optional) There a several things that can be done here:
    // 1. If you need to adjust template/data for the new context (i.e. widget size has chaned) - you can do it here.
    // 2. Log this call for telemetry to monitor what size users choose the most.
}

// This function will be invoked when widget is Activated.
void WeatherWidget::Activate()
{
    // Since this widget doesn't update data for any reason
    // except when 'Increment' button was clicked - 
    // there's nothing to do here. However, for widgets that
    // constantly push updates this is the signal to start
    // pushing those updates since widget is now visible.
    m_isActivated = true;
}

// This function will be invoked when widget is Deactivated.
void WeatherWidget::Deactivate()
{
    // This is the moment to stop sending all further updates until
    // Activate() was called again.
    m_isActivated = false;
}

winrt::hstring WeatherWidget::GetDefaultTemplate()
{
    auto uri = winrt::Uri(L"ms-appx:///Templates/WeatherWidgetTemplate.json");
    auto storageFile = winrt::StorageFile::GetFileFromApplicationUriAsync(uri).get();
    return winrt::FileIO::ReadTextAsync(storageFile).get();
}

winrt::hstring WeatherWidget::GetTemplateForWidget()
{
    // This widget has the same template for all the sizes/themes so we load it only once.
    static winrt::hstring widgetTemplate = GetDefaultTemplate();
    return widgetTemplate;
}

winrt::hstring WeatherWidget::GetDataForWidget()
{
    return L"{}";
}
