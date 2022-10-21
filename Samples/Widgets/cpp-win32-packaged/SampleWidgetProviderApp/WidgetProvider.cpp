// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "WidgetProvider.h"

using CreateWidgetImplFn = std::function<std::shared_ptr<WidgetImplBase>(winrt::WidgetContext, winrt::hstring)>;

struct WidgetImplCreationInfo
{
    const wchar_t* const widgetName;
    CreateWidgetImplFn factoryFn;
};

template<typename T>
std::shared_ptr<WidgetImplBase> CreateWidgetFn(winrt::WidgetContext widgetContext, winrt::hstring state)
{
    auto widgetId = widgetContext.Id();
    auto newWidget = std::make_shared<T>(widgetId, state);

    // Call UpdateWidget() to send data/template to the newly created widget.
    winrt::WidgetUpdateRequestOptions updateOptions{ widgetId };
    updateOptions.Template(newWidget->GetTemplateForWidget());
    updateOptions.Data(newWidget->GetDataForWidget());
    // You can store some custom state in the widget service that you will be able to query at any time.
    updateOptions.CustomState(newWidget->State());
    // Update the widget
    winrt::WidgetManager::GetDefault().UpdateWidget(updateOptions);
    return newWidget;
}

#include "CountingWidgetImpl.h"
#include "WeatherWidgetImpl.h"

// Register all widget types here, optionally specify an is enabled function
const static std::array<WidgetImplCreationInfo, 2> s_widgetImplRegistry = { { {L"Counting_Widget", &CreateWidgetFn<CountingWidget>}, {L"Weather_Widget", &CreateWidgetFn<WeatherWidget>} } };

std::unordered_map<winrt::hstring, std::shared_ptr<WidgetImplBase>> WidgetProvider::m_runningWidgetImpl{};

WidgetProvider::WidgetProvider()
{
    RecoverRunningWidgets();
}

std::shared_ptr<WidgetImplBase> WidgetProvider::InitializeWidgetInternal(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext, winrt::hstring state)
{
    auto widgetName = widgetContext.DefinitionId();
    auto widgetId = widgetContext.Id();
    auto creationInfoIt = std::find_if(s_widgetImplRegistry.begin(), s_widgetImplRegistry.end(), [widgetName](const auto& e) { return widgetName == e.widgetName; });
    if (creationInfoIt != s_widgetImplRegistry.end())
    {
        auto widgetImpl = creationInfoIt->factoryFn(widgetContext, state);
        m_runningWidgetImpl[widgetId] = widgetImpl;
        return widgetImpl;
    }
    return nullptr;
}

std::shared_ptr<WidgetImplBase> WidgetProvider::FindRunningWidget(winrt::hstring widgetId)
{
    if (m_runningWidgetImpl.find(widgetId) != m_runningWidgetImpl.end())
    {
        return m_runningWidgetImpl[widgetId];
    }
    else
    {
        return nullptr;
    }
}

// Handle the CreateWidget call. During this function call you should store
// the WidgetId value so you can use it to update corresponding widget.
// It is our way of notifying you that the user has pinned your widget
// and you should start pushing updates.
void WidgetProvider::CreateWidget(winrt::WidgetContext widgetContext)
{
    // Since it's a new widget - there's no cached state and we pass an empty string instead.
    InitializeWidgetInternal(widgetContext, L"");
}

// Handle the DeleteWidget call. This is notifying you that
// you don't need to provide new content for the given WidgetId
// since the user has unpinned the widget or it was deleted by the Host
// for any other reason.
void WidgetProvider::DeleteWidget(winrt::hstring const& widgetId, [[maybe_unused]] winrt::hstring const& customState)
{
    m_runningWidgetImpl.erase(widgetId);
}

// Handle the OnActionInvoked call. This function call is fired when the user's
// interaction with the widget resulted in an execution of one of the defined
// actions that you've indicated in the template of the Widget.
// For example: clicking a button or submitting input.
void WidgetProvider::OnActionInvoked(winrt::WidgetActionInvokedArgs actionInvokedArgs)
{
    auto widgetId = actionInvokedArgs.WidgetContext().Id();
    if (const auto& runningWidget = FindRunningWidget(widgetId))
    {
        runningWidget->OnActionInvoked(actionInvokedArgs);
    }
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
    if (const auto& runningWidget = FindRunningWidget(widgetId))
    {
        runningWidget->OnWidgetContextChanged(contextChangedArgs);
        // Optionally: if the data/template for the new context is different
        // from the previously sent data/template - send an update.
    }
}

// Handle the Activate call. This function is called when widgets host starts listening
// to the widget updates. It generally happens when the widget becomes visible and the updates
// will be promptly displayed to the user. It's recommended to start sending updates from this moment
// until Deactivate function was called.
void WidgetProvider::Activate(winrt::Microsoft::Windows::Widgets::Providers::WidgetContext widgetContext)
{
    auto widgetId = widgetContext.Id();
    if (const auto& runningWidget = FindRunningWidget(widgetId))
    {
        runningWidget->Activate(widgetContext);
    }
}

// Handle the Deactivate call. This function is called when widgets host stops listening
// to the widget updates. It generally happens when the widget is not visible to the user
// anymore and any further updates won't be displayed until the widget is visible again.
// It's recommended to stop sending updates until Activate function was called.
void WidgetProvider::Deactivate(winrt::hstring widgetId)
{
    if (const auto& runningWidget = FindRunningWidget(widgetId))
    {
        runningWidget->Deactivate(widgetId);
    }
}

// This function will be called in WidgetProvider Constructor
// to get information about all the widgets that this provider
// is currently providing. It's helpful in case of the Provider
// subsequent launches to restore the previous state of the running widgets.
void WidgetProvider::RecoverRunningWidgets()
{
    try
    {
        winrt::WidgetManager widgetManager = winrt::WidgetManager::GetDefault();
        for (const auto& widgetInfo : widgetManager.GetWidgetInfos())
        {
            auto widgetContext = widgetInfo.WidgetContext();
            auto widgetId = widgetContext.Id();
            auto customState = widgetInfo.CustomState();

            // Check if this widgetId is known
            if (!FindRunningWidget(widgetId))
            {
                // Hook up an instance with this context
                // Ignore the return result if this widget is not supported
                InitializeWidgetInternal(widgetContext, customState);
            }
        }
    }
    catch (...)
    {

    }
}
