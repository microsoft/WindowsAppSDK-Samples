// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WidgetProvider.h"
#include <iostream>

std::unordered_map<winrt::hstring, std::shared_ptr<CountingWidgetInfo>> WidgetProvider::m_runningWidgets{};

WidgetProvider::WidgetProvider()
{
    RecoverRunningWidgets();
}

std::shared_ptr<CountingWidgetInfo> WidgetProvider::FindRunningWidget(winrt::hstring widgetId)
{
    if (m_runningWidgets.find(widgetId) != m_runningWidgets.end())
    {
        return m_runningWidgets[widgetId];
    }
    else
    {
        return nullptr;
    }
}

// Handle the CreateWidget call. During this function call you should store
// the WidgetId value so you can use it to update corresponding widget.
// It is our way of notifying you that the user has pinned your widget
// on the board. You can optionally push the template/data to the service
// in this method but it can also be done in the Activate call.
void WidgetProvider::CreateWidget(winrt::WidgetContext widgetContext)
{
    auto widgetId = widgetContext.Id();
    auto countingWidget = std::make_shared<CountingWidgetInfo>(widgetId, 0, false);
    m_runningWidgets[widgetId] = countingWidget;

    winrt::WidgetUpdateRequestOptions updateOptions{ widgetId };
    updateOptions.Template(GetTemplateForWidget());
    updateOptions.Data(GetDataForWidget(widgetId));
    // You can store some custom state in the widget service that you will be able to query at any time.
    updateOptions.CustomState(GetStateForWidget(widgetId));
    // Update the widget
    winrt::WidgetManager::GetDefault().UpdateWidget(updateOptions);
}

// Handle the DeleteWidget call. This is notifying you that
// you don't need to provide new content for the given WidgetId
// since the user has unpinned the widget or it was deleted by the Host
// for any other reason.
void WidgetProvider::DeleteWidget(winrt::hstring const& widgetId, winrt::hstring const& /*customState*/)
{
    m_runningWidgets.erase(widgetId);
}

// Handle the OnActionInvoked call. This function call is fired when the user's
// interaction with the widget resulted in an execution of one of the defined
// actions that you've indicated in the template of the Widget.
// For example: clicking a button or submitting input.
void WidgetProvider::OnActionInvoked(winrt::WidgetActionInvokedArgs actionInvokedArgs)
{
    auto verb = actionInvokedArgs.Verb();
    auto widgetContext = actionInvokedArgs.WidgetContext();
    auto widgetId = widgetContext.Id();
    if (auto&& widget = FindRunningWidget(widgetId))
    {
        if (verb == L"inc")
        {
            widget->m_clickCount++;
            // Generate template/data you want to send back
            // The template has not changed so it does not neeed to be updated
            auto widgetData = GetDataForWidget(widgetId);

            // Build update options and update the widget
            winrt::WidgetUpdateRequestOptions updateOptions{ widgetId };
            updateOptions.Data(widgetData);
            updateOptions.CustomState(GetStateForWidget(widgetId));

            winrt::WidgetManager::GetDefault().UpdateWidget(updateOptions);
            return;
        }
        throw winrt::hresult_invalid_argument{ L"OnActionInvoked invoked for widget with id: " + widgetId + L" and unknown verb: " + verb };
    }
    throw winrt::hresult_invalid_argument{ L"OnActionInvoked invoked for unknown widget with id: " + widgetId };
}

// Handle the WidgetContextChanged call. This function is called when the context a widget
// has changed. Currently it only signals that the user has changed the size of the widget.
// There are 2 ways to respond to this event:
// 1) Call UpdateWidget() with the new data/template to fit the new requested size.
// 2) If previously sent data/template accounts for various sizes based on $host.widgetSize - you can use this event solely for telemtry.
void WidgetProvider::OnWidgetContextChanged(winrt::WidgetContextChangedArgs contextChangedArgs)
{
    auto widgetContext = contextChangedArgs.WidgetContext();
    auto widgetId = widgetContext.Id();
    if (auto&& widget = FindRunningWidget(widgetId))
    {
        // Optionally: if the data/template for the new context is different
        // from the previously sent data/template - send an update.
        return;
    }
    throw winrt::hresult_invalid_argument{ L"WidgetContextChanged invoked for unknown widget with id: " + widgetId };
}

// Handle the Activate call. This function is called when widgets host starts listening
// to the widget updates. It generally happens when the widget becomes visible and the updates
// will be promptly displayed to the user. It's recommended to start sending updates from this moment
// until Deactivate function was called.
void WidgetProvider::Activate(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext)
{
    auto widgetId = widgetContext.Id();
    if (auto&& widget = FindRunningWidget(widgetId))
    {
        widget->m_isActivated = true;

        winrt::WidgetUpdateRequestOptions updateOptions{ widgetId };
        updateOptions.Template(GetTemplateForWidget());
        updateOptions.Data(GetDataForWidget(widgetId));
        // You can store some custom state in the widget service that you will be able to query at any time.
        updateOptions.CustomState(GetStateForWidget(widgetId));
        // Update the widget
        winrt::WidgetManager::GetDefault().UpdateWidget(updateOptions);

    }
    throw winrt::hresult_invalid_argument{ L"Trying to deactivate unknown widget with id: " + widgetId };
}

// Handle the Deactivate call. This function is called when widgets host stops listening
// to the widget updates. It generally happens when the widget is not visible to the user
// anymore and any further updates won't be displayed until the widget is visible again.
// It's recommended to stop sending updates until Activate function was called.
void WidgetProvider::Deactivate(winrt::hstring widgetId)
{
    if (auto&& widget = FindRunningWidget(widgetId))
    {
        widget->m_isActivated = false;
    }
    throw winrt::hresult_invalid_argument{ L"Trying to activate unknown widget with id: " + widgetId };
}

// This function will be called in WidgetProvider Constructor
// to get information about all the widgets that this provider
// is currently providing. It's helpful in case of the Provider
// subsequent launches to restore the previous state of the running widgets.
void WidgetProvider::RecoverRunningWidgets()
{
    winrt::WidgetManager widgetManager = winrt::WidgetManager::GetDefault();
    for (auto widgetInfo : widgetManager.GetWidgetInfos())
    {
        auto widgetContext = widgetInfo.WidgetContext();
        auto widgetId = widgetContext.Id();
        auto customState = widgetInfo.CustomState();
        auto isActive = widgetContext.IsActive();

        // Check if this widgetId is known
        if (!FindRunningWidget(widgetId))
        {
            // Hook up an instance with this context
            // Ignore the return result if this widget is not supported
            auto clickCount = 0;
            try
            {
                int cachedCount = std::stoi(customState.c_str());
                clickCount = cachedCount;
            }
            catch (...)
            {
                // Parsing the state was not successfull: cached state might've been corrupted.
                // count stays 0.
            }
            auto countingWidget = std::make_shared<CountingWidgetInfo>(widgetId, clickCount, isActive);
            m_runningWidgets[widgetId] = countingWidget;
        }
    }
}

winrt::hstring WidgetProvider::GetTemplateForWidget()
{
    // This widget has the same template for all the sizes/themes so we load it only once.
    static winrt::hstring widgetTemplate;
    if (widgetTemplate.empty())
    {
        auto storageFile = winrt::Windows::Storage::StorageFile::GetFileFromApplicationUriAsync(winrt::Windows::Foundation::Uri(L"ms-appx:///Templates/CountingWidgetTemplate.json")).get();
        widgetTemplate = winrt::Windows::Storage::FileIO::ReadTextAsync(storageFile).get();
    }
    return widgetTemplate;
}

winrt::hstring WidgetProvider::GetDataForWidget(winrt::hstring const& widgetId)
{
    if (const auto localWidgetInfo = FindRunningWidget(widgetId))
    {
        return L"{ \"count\": " + winrt::to_hstring(localWidgetInfo->m_clickCount) + L" }";
    }
    throw winrt::hresult_invalid_argument{ L"Trying to get data for unknown widget with id: " + widgetId };
}


winrt::hstring WidgetProvider::GetStateForWidget(winrt::hstring const& widgetId)
{
    if (const auto localWidgetInfo = FindRunningWidget(widgetId))
    {
        return winrt::to_hstring(localWidgetInfo->m_clickCount);
    }
    throw winrt::hresult_invalid_argument{ L"Trying to get state for unknown widget with id: " + widgetId };
}
