// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CountingWidgetImpl.h"

CountingWidget::CountingWidget(winrt::hstring const& id, winrt::hstring const& state) : WidgetImplBase(id, state)
{
    if (state.empty())
    {
        State(L"0");
    }
    else
    {
        try
        {
            // This particular widget stores its clickCount
            // in the state as integer. Attempt to parse the saved state
            // and convert it to integer.
            m_clickCount = std::stoi(state.c_str());
        }
        catch (...)
        {
            // Parsing the state was not successful: cached state might've been corrupted.
            // m_clickCount stays 0.
        }
    }
}

// This function wil be invoked when the Increment button was clicked by the user.
void CountingWidget::OnActionInvoked(winrt::WidgetActionInvokedArgs actionInvokedArgs)
{
    auto verb = actionInvokedArgs.Verb();
    if (verb == L"inc")
    {
        // Increment the count
        m_clickCount++;
        State(winrt::to_hstring(m_clickCount));

        // Generate template/data you want to send back
        // The template has not changed so it does not neeed to be updated
        auto widgetData = GetDataForWidget();

        // Build update options and update the widget
        winrt::WidgetUpdateRequestOptions updateOptions{ Id()};
        updateOptions.Data(widgetData);
        updateOptions.CustomState(State());

        winrt::WidgetManager::GetDefault().UpdateWidget(updateOptions);
    }
}

// This function will be invoked when WidgetContext has changed.
void CountingWidget::OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs)
{
    // (Optional) There a several things that can be done here:
    // 1. If you need to adjust template/data for the new context (i.e. widget size has chaned) - you can do it here.
    // 2. Log this call for telemetry to monitor what size users choose the most.
}

// This function will be invoked when widget is Activated.
void CountingWidget::Activate()
{
    // Since this widget doesn't update data for any reason
    // except when 'Increment' button was clicked - 
    // there's nothing to do here. However, for widgets that
    // constantly push updates this is the signal to start
    // pushing those updates since widget is now visible.
    m_isActivated = true;
}

// This function will be invoked when widget is Deactivated.
void CountingWidget::Deactivate()
{
    // This is the moment to stop sending all further updates until
    // Activate() was called again.
    m_isActivated = false;
}

winrt::hstring CountingWidget::GetDefaultTemplate()
{
    auto uri = winrt::Uri(L"ms-appx:///Templates/CountingWidgetTemplate.json");
    auto storageFile = winrt::StorageFile::GetFileFromApplicationUriAsync(uri).get();
    return winrt::FileIO::ReadTextAsync(storageFile).get();
}

winrt::hstring CountingWidget::GetTemplateForWidget()
{
    // This widget has the same template for all the sizes/themes so we load it only once.
    static winrt::hstring widgetTemplate = GetDefaultTemplate();
    return widgetTemplate;
}

winrt::hstring CountingWidget::GetDataForWidget()
{
    return LR"({ "count": )" + State() + L" }";
}
