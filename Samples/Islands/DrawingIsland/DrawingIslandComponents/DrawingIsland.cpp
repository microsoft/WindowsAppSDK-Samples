﻿// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DrawingIsland.h"
#include "DrawingIsland.g.cpp"

namespace winrt::DrawingIslandComponents::implementation
{
    DrawingIsland::DrawingIsland(
        const winrt::Compositor& compositor)
    {
        m_output.Compositor = compositor;
        m_output.TextRenderer = std::make_shared<TextRenderer>(m_output.Compositor);
        m_output.DeviceLostHelper.WatchDevice(m_output.TextRenderer->GetDevice());
        m_output.DeviceLostHelper.DeviceLost({ this, &DrawingIsland::OnDirect3DDeviceLost });

        // Create the Compositor and the Content:
        // - The Bridge's connection to the Window will keep everything alive, and perform an
        //   orderly tear-down:
        //
        //   Window -> Bridge -> Content -> Visual -> InputSite -> InputObject
        //
        // - When the ContentIsland is destroyed, ContentIsland.AppData will call IClosable.Close
        //   on this instance to break cycles and clean up expensive resources.

        m_output.RootVisual = m_output.Compositor.CreateContainerVisual();
        m_output.RootVisual.RelativeSizeAdjustment({ 1.0f, 1.0f });

        m_island = winrt::ContentIsland::Create(m_output.RootVisual);
        m_island.AppData(get_strong().as<winrt::IInspectable>());

        Output_Initialize();
        Input_Initialize();
        Environment_Initialize();
        Accessibility_Initialize();

        m_prevState.RasterizationScale = m_island.RasterizationScale();

        // Get notifications for resize, bridge changes, etc.
        (void)m_island.StateChanged(
            [&](auto&& ...)
        {
            return Island_OnStateChanged();
        });


        (void)m_island.Closed(
            [&]()
        {
            return Island_OnClosed();
        });

    }


    DrawingIsland::~DrawingIsland()
    {
        m_output.Compositor = nullptr;
    }


    void
    DrawingIsland::Close()
    {
        m_output.RootVisual = nullptr;

        m_input.KeyboardSource = nullptr;
        m_input.PretranslateSource = nullptr;
        m_input.PointerSource = nullptr;
        m_input.FocusController = nullptr;

        m_background.BrushDefault = nullptr;
        m_background.BrushA = nullptr;
        m_background.BrushB = nullptr;
        m_background.BrushC = nullptr;
        m_background.Visual = nullptr;

        m_items.Visuals = nullptr;
        m_items.Items.clear();
        m_items.SelectedItem = nullptr;

        if (nullptr != m_background.BackdropController)
        {
            // Clean up our SystemBackdropController.
            m_background.BackdropController.RemoveAllSystemBackdropTargets();
            m_background.BackdropController.Close();
            m_background.BackdropController = nullptr;
        }

        if (nullptr != m_background.BackdropConfiguration)
        {
            m_background.BackdropConfiguration = nullptr;
        }

        // Make sure automation is destroyed.
        for (auto& automationPeer : m_uia.AutomationPeers)
        {
            automationPeer.GetAutomationProvider()->SetCallbackHandler(nullptr);
        }
        m_uia.AutomationPeers.clear();

        if (nullptr != m_uia.FragmentRoot)
        {
            m_uia.FragmentRoot->SetCallbackHandler(nullptr);
            m_uia.FragmentRoot = nullptr;
        }


        // Destroy Content:
        // - This handles if the ContentIsland has already started destruction and is notifying the
        //   app.

        m_island.Close();
        m_island = nullptr;

        // TODO: Add the following test case in automated tests:
        // 1. We are recursively calling ContentIsland.Close while inside the ContentIsland.Closed
        // event.
        // 2. We are testing the potential final Release() of ContentIsland while inside the Closed
        // event.

        m_output.Compositor = nullptr;
    }


    void DrawingIsland::EnableBackgroundTransparency(
        bool value)
    {
        if (value != m_background.BackdropEnabled)
        {
            m_background.BackdropEnabled = value;
            Output_UpdateSystemBackdrop();
        }
    }


    IFACEMETHODIMP
    DrawingIsland::OnDirectMessage(
        IInputPreTranslateKeyboardSourceInterop* /*source*/,
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled)
    {
        *handled = false;

        // Alt+A Debug print to see the order of the PreTranslate callbacks
        if ((keyboardModifiers & FALT) &&
            (msg->message == WM_SYSKEYDOWN) &&
            (static_cast<winrt::Windows::System::VirtualKey>(msg->wParam) == winrt::VirtualKey::A))
        {

        }

        if (keyboardModifiers & FALT)
        {            
            *handled = Input_AcceleratorKeyActivated(
                static_cast<winrt::Windows::System::VirtualKey>(msg->wParam));
        }

        return S_OK;
    }


    IFACEMETHODIMP
    DrawingIsland::OnTreeMessage(
        IInputPreTranslateKeyboardSourceInterop* /*source*/,
        const MSG* msg,
        UINT keyboardModifiers,
        _Inout_ bool* handled)
    {
        // Alt+A Debug print to see the order of the PreTranslate callbacks
        if ((keyboardModifiers & FALT) &&
            (msg->message == WM_SYSKEYDOWN) &&
            (static_cast<winrt::Windows::System::VirtualKey>(msg->wParam) == winrt::VirtualKey::A))
        {

        }

        *handled = false;
        return S_OK;
    }


    winrt::Windows::Graphics::RectInt32
    DrawingIsland::GetScreenBoundsForAutomationFragment(
        _In_::IUnknown const* const sender) const
    {
        // Check if the automation provider that has sent this callback is the fragment root.
        if (m_uia.FragmentRoot.as<::IUnknown>().get() == sender)
        {
            winrt::Rect logicalRect;
            logicalRect.X = 0;
            logicalRect.Y = 0;
            logicalRect.Width = m_island.ActualSize().x;
            logicalRect.Height = m_island.ActualSize().y;

            // This will convert from the logical coordinate space of the ContentIsland to
            // Win32 screen coordinates that are needed by Accessibility.
            return m_island.CoordinateConverter().ConvertLocalToScreen(logicalRect);
        }

        // Else find the matching automation peer entry for the sending fragment.
        auto iterator = std::find_if(
            m_uia.AutomationPeers.begin(), m_uia.AutomationPeers.end(),
            [&sender](auto const& automationPeer)
            {
                return automationPeer.Match(sender);
            });

        if (m_uia.AutomationPeers.end() == iterator)
        {
            // Did not find any match for this automation provider.
            return { 0, 0, 0, 0 };
        }

        auto const& visualPeer = iterator->GetVisual();
        winrt::Rect logicalRect;
        logicalRect.X = visualPeer.Offset().x;
        logicalRect.Y = visualPeer.Offset().y;
        logicalRect.Width = visualPeer.Size().x;
        logicalRect.Height = visualPeer.Size().y;

        // This will convert from the logical coordinate space of the ContentIsland to
        // Win32 screen coordinates that are needed by Accessibility.
        return m_island.CoordinateConverter().ConvertLocalToScreen(logicalRect);
    }


    winrt::com_ptr<IRawElementProviderFragment>
    DrawingIsland::GetFragmentFromPoint(
        _In_ double x,
        _In_ double y) const
    {
        // UIA provides hit test points in screen space.
        // Convert the point into a dummy empty rectangle to use with the coordinate converter.
        winrt::Windows::Graphics::RectInt32 screenRect
            { static_cast<int>(x + 0.5), static_cast<int>(y + 0.5), 0, 0 };

        auto logicalRect = m_island.CoordinateConverter().ConvertScreenToLocal(screenRect);
        float2 localPoint{ logicalRect.X, logicalRect.Y };
        auto hitTestElement = HitTestItem(localPoint);

        // Find the automation peer for the hit test visual if any.
        if (nullptr != hitTestElement)
        {
            auto& hitTestVisual = hitTestElement->GetVisual();

            auto iterator = std::find_if(
                m_uia.AutomationPeers.begin(), m_uia.AutomationPeers.end(),
                [&hitTestVisual](auto const& automationPeer)
                {
                    return automationPeer.Match(hitTestVisual);
                });

            if (m_uia.AutomationPeers.end() != iterator)
            {
                // Return the automation provider if we found an automation peer
                // for the hit test visual.
                return iterator->GetAutomationProvider().as<IRawElementProviderFragment>();
            }
        }

        return nullptr;
    }


    winrt::com_ptr<IRawElementProviderFragment>
    DrawingIsland::GetFragmentInFocus() const
    {
        if (m_items.SelectedItem != nullptr)
        {
            auto& visual = m_items.SelectedItem->GetVisual();

            // Find the currently selected visual's automation peer.
            auto iterator = std::find_if(
                m_uia.AutomationPeers.begin(), m_uia.AutomationPeers.end(),
                [visual](auto const& automationPeer)
                {
                    return automationPeer.Match(visual);
                });

            if (m_uia.AutomationPeers.end() != iterator)
            {
                // Return the automation provider if we found an automation peer
                // for the selected visual.
                return iterator->GetAutomationProvider().as<IRawElementProviderFragment>();
            }
        }

        return nullptr;
    }


    Item* DrawingIsland::HitTestItem(float2 const& point) const
    {
        // Iterate from the end of the vector, i.e., from front to back.
        for (size_t i = m_items.Items.size(); i != 0; i--)
        {
            Item* item = m_items.Items[i - 1].get();
            auto& visual = item->GetVisual();

            winrt::float3 const offset = visual.Offset();
            float2 const size = visual.Size();

            if (point.x >= offset.x &&
                point.x < offset.x + size.x &&
                point.y >= offset.y &&
                point.y < offset.y + size.y)
            {
                return item;
            }
        }
        return nullptr;
    }


    void
    DrawingIsland::Accessibility_Initialize()
    {
        // Create an UI automation fragment root for our island's content.
        m_uia.FragmentRoot = winrt::make_self<AutomationFragmentRoot>();
        m_uia.FragmentRoot->SetName(L"Drawing Squares");
        m_uia.FragmentRoot->SetProviderOptions(
            ProviderOptions_ServerSideProvider |
            ProviderOptions_UseComThreading |
            ProviderOptions_RefuseNonClientSupport);
        m_uia.FragmentRoot->SetUiaControlTypeId(UIA_WindowControlTypeId);
        m_uia.FragmentRoot->SetCallbackHandler(this);

        (void)m_island.AutomationProviderRequested(
            { this, &DrawingIsland::Accessibility_OnAutomationProviderRequested });
    }


    void
    DrawingIsland::Accessibility_CreateItemFragment(
        const winrt::Visual& itemVisual)
    {
        // Create a new automation fragment.
        auto fragment = winrt::make_self<AutomationFragment>();
        fragment->SetName(s_colorNames[m_output.CurrentColorIndex].c_str());
        fragment->SetCallbackHandler(this);

        // Add an entry to our automation peers (a mapping between the Visual and the Fragment):
        // - This is keeping the UIA objects alive.
        // - The lookup is used to get back to the item Fragment for
        //   specific operations, such as hit-testing or tree walking.
        //   This avoids adding to more expensive data storage,
        //   such as the Visual.CustomProperties map.
        m_uia.AutomationPeers.push_back(AutomationPeer{ itemVisual, fragment });

        // Connect the automation fragment to our fragment root.
        m_uia.FragmentRoot->AddChildToEnd(fragment);
    }


    void
    DrawingIsland::Accessibility_OnAutomationProviderRequested(
        const winrt::ContentIsland& island,
        const winrt::ContentIslandAutomationProviderRequestedEventArgs& args)
    {
        // Make sure the host provider is up to date.
        m_uia.FragmentRoot->SetHostProvider(
            island.GetAutomationHostProvider().as<IRawElementProviderSimple>());

        // Return the fragment root as an IInspectable.
        args.AutomationProvider(m_uia.FragmentRoot.as<IInspectable>());

        args.Handled(true);
    }


    void
    DrawingIsland::Input_Initialize()
    {
        m_input.PointerSource = winrt::InputPointerSource::GetForIsland(m_island);

        m_input.PointerSource.PointerPressed(
            [this](winrt::InputPointerSource const&,
                   winrt::PointerEventArgs const& args)
            {
                auto currentPoint = args.CurrentPoint();
                auto properties = currentPoint.Properties();

                if (properties.IsLeftButtonPressed())
                {
                    Input_OnLeftButtonPressed(args);
                }
                else if (properties.IsRightButtonPressed())
                {
                    Input_OnRightButtonPressed(args);
                }
        });

        m_input.PointerSource.PointerMoved(
            [this](winrt::InputPointerSource const&,
                   winrt::PointerEventArgs const& args)
            {
                Input_OnPointerMoved(args);
            });

        m_input.PointerSource.PointerReleased(
            [&](auto&& ...)
            {
                Input_OnPointerReleased();
            });

        // Set up the keyboard source.
        m_input.KeyboardSource = winrt::InputKeyboardSource::GetForIsland(m_island);

        m_input.KeyboardSource.KeyDown(
            [this](winrt::InputKeyboardSource const&,
                   winrt::KeyEventArgs const& args)
        {
            bool handled = Input_OnKeyDown(args.VirtualKey());

            // Mark the event as handled
            if (handled)
            {
                args.Handled(true);
            }
        });

        m_input.PretranslateSource =
            winrt::InputPreTranslateKeyboardSource::GetForIsland(m_island);

        m_input.PretranslateSource.as<
            Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop>()->
                SetPreTranslateHandler(this);

        auto activationListener = winrt::InputActivationListener::GetForIsland(m_island);
        (void)activationListener.InputActivationChanged(
            [this, activationListener](
                winrt::InputActivationListener const&,
                winrt::InputActivationListenerActivationChangedEventArgs const&)
        {
            switch (activationListener.State())
            {
            case winrt::InputActivationState::Activated:
                m_output.RootVisual.Opacity(1.0f);
                if (nullptr != m_background.BackdropConfiguration)
                {
                    m_background.BackdropConfiguration.IsInputActive(true);
                }
                break;

            default:
                m_output.RootVisual.Opacity(m_output.Opacity);
                if (nullptr != m_background.BackdropConfiguration)
                {
                    m_background.BackdropConfiguration.IsInputActive(false);
                }
                break;
            }
        });

        m_input.FocusController = winrt::InputFocusController::GetForIsland(m_island);
        m_input.FocusController.NavigateFocusRequested(
            [this](winrt::InputFocusController const&,
                winrt::FocusNavigationRequestEventArgs const& args)
            {
                bool setFocus = m_input.FocusController.TrySetFocus();
                // Mark the event as handled
                if (setFocus)
                {
                    args.Result(winrt::FocusNavigationResult::Moved);
                }
            });
    }


    bool 
    DrawingIsland::Input_OnKeyDown(
        winrt::Windows::System::VirtualKey virtualKey)
    {
        bool handled = false;

        switch (virtualKey)
        {
            case winrt::VirtualKey::A:
            {
                m_background.Visual.Brush(m_background.BrushA);
                handled = true;
                break;
            }

            case winrt::VirtualKey::B:
            {
                m_background.Visual.Brush(m_background.BrushB);
                handled = true;
                break;
            }

            case winrt::VirtualKey::C:
            {
                m_background.Visual.Brush(m_background.BrushC);
                handled = true;
                break;
            }

            case winrt::VirtualKey::Space:
            {
                m_background.Visual.Brush(m_background.BrushDefault);
                break;
            }
                
            case winrt::Windows::System::VirtualKey::Number1:
            {
                m_output.CurrentColorIndex = 0;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Number2:
            {
                m_output.CurrentColorIndex = 1;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Number3:
            {
                m_output.CurrentColorIndex = 2;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Number4:
            {
                m_output.CurrentColorIndex = 3;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Delete:
            case winrt::Windows::System::VirtualKey::Escape:
            {
                m_items.Visuals.RemoveAll();
                m_items.Items.clear();

                // Update accessibility.
                m_uia.FragmentRoot->RemoveAllChildren();
                m_uia.AutomationPeers.clear();

                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Tab:
            {
                auto request = winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
                    winrt::Microsoft::UI::Input::FocusNavigationReason::First);
                m_input.FocusController.DepartFocus(request);
            }
        }

        Output_UpdateCurrentColorVisual();

        return handled;
    }

    bool 
    DrawingIsland::Input_AcceleratorKeyActivated(
        winrt::Windows::System::VirtualKey virtualKey)
    {
        bool handled = false;
        
        switch (virtualKey)
        {
            case winrt::VirtualKey::X:
            {
                m_background.Visual.Brush(m_background.BrushA);
                handled = true;
                break;
            }

            case winrt::VirtualKey::Y:
            {
                m_background.Visual.Brush(m_background.BrushB);
                handled = true;
                break;
            }

            case winrt::VirtualKey::Z:
            {
                m_background.Visual.Brush(m_background.BrushC);
                handled = true;
                break;
            }
        }

        return handled;
    }


    void
    DrawingIsland::Input_OnLeftButtonPressed(
        const winrt::PointerEventArgs& args)
    {
        // Left button manipulates the custom-drawn content
        float2 const point = args.CurrentPoint().Position();

        bool controlPressed = WI_IsFlagSet(
            args.KeyModifiers(),
            winrt::Windows::System::VirtualKeyModifiers::Control);

        OnLeftClick(point, controlPressed);
    }


    void
    DrawingIsland::Input_OnPointerReleased()
    {
        m_items.SelectedItem = nullptr;
    }


    void
    DrawingIsland::LeftClickAndRelease(
        const float2 currentPoint)
    {
        OnLeftClick(currentPoint, false);
        Input_OnPointerReleased();
    }


    void
    DrawingIsland::RightClickAndRelease(
        const float2 currentPoint)
    {
        OnRightClick(currentPoint);
        Input_OnPointerReleased();
    }


    void
    DrawingIsland::OnLeftClick(
        const float2 point,
        bool controlPressed)
    {
        m_items.SelectedItem = HitTestItem(point);
        
        if (m_items.SelectedItem != nullptr)
        {
            Item* item = m_items.SelectedItem;
            auto& visual = m_items.SelectedItem->GetVisual();
            winrt::float3 const offset = visual.Offset();

            m_items.Offset.x = offset.x - point.x;
            m_items.Offset.y = offset.y - point.y;

            // Move the visual to the top.
            m_items.Visuals.Remove(visual);
            m_items.Visuals.InsertAtTop(visual);

            // Move the VisualElement to the end of the vector if it isn't already.
            if (!m_items.Items.empty() && m_items.Items.back().get() != item)
            {
                auto i = std::find_if(
                    m_items.Items.begin(),
                    m_items.Items.end(),
                    [item](auto& elem) { return elem.get() == item; }
                );
                if (i != m_items.Items.end())
                {
                    std::rotate(i, i + 1, m_items.Items.end());
                }
            }

            // Update automation.
            // First find the existing automation peer.
            auto iterator = std::find_if(
                m_uia.AutomationPeers.begin(), m_uia.AutomationPeers.end(),
                [visual](auto const& automationPeer)
                {
                    return automationPeer.Match(visual);
                });
            if (m_uia.AutomationPeers.end() != iterator)
            {
                // Mirror the change to the visuals above.
                m_uia.FragmentRoot->RemoveChild(iterator->GetAutomationProvider());
                m_uia.FragmentRoot->AddChildToEnd(iterator->GetAutomationProvider());
            }
        }
        else
        {
            Output_AddVisual(point, controlPressed);
        }
    }


    void
    DrawingIsland::Input_OnRightButtonPressed(
        const winrt::PointerEventArgs& args)
    {
        OnRightClick(args.CurrentPoint().Position());
    }

    void
    DrawingIsland::OnRightClick(const float2 point)
    {
        // TODO - what is the purpose of this?
        UNREFERENCED_PARAMETER(point);
        // auto selectedVisual = HitTestVisual(point);
    }

    void
    DrawingIsland::Input_OnPointerMoved(
        const winrt::PointerEventArgs& args)
    {
        if (m_items.SelectedItem)
        {
            auto& visual = m_items.SelectedItem->GetVisual();
            float2 const point = args.CurrentPoint().Position();

            visual.Offset(
                { point.x + m_items.Offset.x,
                point.y + m_items.Offset.y,
                0.0f });
        }
    }


    void
    DrawingIsland::Island_OnClosed()
    {

    }


    void
    DrawingIsland::Island_OnStateChanged()
    {
        if (m_prevState.RasterizationScale != m_island.RasterizationScale())
        {
            float newScale = m_island.RasterizationScale();

            m_prevState.RasterizationScale = newScale;

            m_output.TextRenderer->SetDpiScale(newScale);

            for (auto& item : m_items.Items)
            {
                item->OnDpiScaleChanged();
            }
        }

        if (m_prevState.LayoutDirection != m_island.LayoutDirection())
        {
            Environment_SetLayoutDirection();
        }

        Output_UpdateCurrentColorVisual();
    }


    void
    DrawingIsland::Environment_SetLayoutDirection()
    {
        if (m_island.LayoutDirection() == ContentLayoutDirection::RightToLeft)
        {
            // The following will mirror the visuals. If any text is inside the visuals the text is
            // also mirrored. The text will need another RelativeOffsetAdjustment and Scale to
            // return to the normal space.

            m_output.RootVisual.RelativeOffsetAdjustment(winrt::float3(1, 0, 0));
            m_output.RootVisual.Scale(winrt::float3(-1, 1, 1));
        }
        else
        {
            m_output.RootVisual.RelativeOffsetAdjustment(winrt::float3(0, 0, 0));
            m_output.RootVisual.Scale(winrt::float3(1, 1, 1));
        }
        m_prevState.LayoutDirection = m_island.LayoutDirection();
    }


    void
    DrawingIsland::OnDirect3DDeviceLost(
        DeviceLostHelper const* /* sender */,
        DeviceLostEventArgs const& /* args */)
    {
        // This call comes in on a Threadpool worker thread, so use the DispatcherQueue
        // to marshal the call to the UI thread.
        m_island.DispatcherQueue().TryEnqueue(
            [this] {
                // Recreate the text renderer's D3D and D2D devices.
                m_output.TextRenderer->RecreateDirect2DDevice();

                // Give each item an opportunity to recreate its device-dependent resources.
                for (auto& item : m_items.Items)
                {
                    item->OnDeviceLost();
                }

                // Listen for device lost on the new device.
                m_output.DeviceLostHelper.WatchDevice(m_output.TextRenderer->GetDevice());
            });
    }


    void
    DrawingIsland::Output_Initialize()
    {
        // Create the background visual.
        m_background.Visual = m_output.Compositor.CreateSpriteVisual();
        m_background.Visual.RelativeSizeAdjustment({ 1.0f, 1.0f });

        m_background.BrushDefault = m_output.Compositor.CreateColorBrush(
            winrt::Color{ 0xFF, 0x20, 0x20, 0x20 });

        m_background.BrushA = m_output.Compositor.CreateColorBrush(
            winrt::Color{ 0xFF, 0x99, 0x20, 0x20 });

        m_background.BrushB = m_output.Compositor.CreateColorBrush(
            winrt::Color{ 0xFF, 0x20, 0x99, 0x20 });

        m_background.BrushC = m_output.Compositor.CreateColorBrush(
            winrt::Color{ 0xFF, 0x20, 0x20, 0x99 });

        m_background.Visual.Brush(m_background.BrushDefault);
        m_output.RootVisual.Children().InsertAtTop(m_background.Visual);

        for (int i = 0; i < _countof(m_output.ColorBrushes); i++)
        {
            m_output.ColorBrushes[i] = m_output.Compositor.CreateColorBrush(s_colors[i]);

            winrt::Color halfTransparent = s_colors[i];
            halfTransparent.A = 0x80;
            m_output.HalfTransparentColorBrushes[i] =
                m_output.Compositor.CreateColorBrush(halfTransparent);
        }

        winrt::ContainerVisual drawingVisualsRoot = m_output.Compositor.CreateContainerVisual();
        m_items.Visuals = drawingVisualsRoot.Children();
        m_output.RootVisual.Children().InsertAtTop(drawingVisualsRoot);

        m_items.CurrentColorVisual = m_output.Compositor.CreateSpriteVisual();
        m_items.CurrentColorVisual.Offset({0.0f, 0.0f, 0.0f});
        m_output.RootVisual.Children().InsertAtTop(m_items.CurrentColorVisual);

        Output_UpdateSystemBackdrop();

        Output_UpdateCurrentColorVisual();
    }


    void
    DrawingIsland::Output_AddVisual(
        float2 const point,
        bool halfTransparent)
    {
        // Determine the visual background and text colors.
        Color backgroundColor = s_colors[m_output.CurrentColorIndex];
        Color textColor = { 0xFF, 0, 0, 0 };
        if (halfTransparent)
        {
            backgroundColor.A /= 2;
            textColor.A /= 2;
        }

        // Create a TextElement object.
        auto textItem = std::make_unique<TextItem>(
            m_output.TextRenderer,
            backgroundColor,
            textColor,
            s_colorNames[m_output.CurrentColorIndex]
        );

        // Get the visual and its size in DIPs.
        auto& visual = textItem->GetVisual();
        float2 size = visual.Size();

        // Set the visual's offset.
        visual.Offset({ point.x - size.x / 2.0f, point.y - size.y / 2.0f, 0.0f });

        // Add the new text element to the vector.
        m_items.Items.push_back(std::move(textItem));

        // Add the visual as a child of the container visual.
        m_items.Visuals.InsertAtTop(visual);

        m_items.SelectedItem = m_items.Items.back().get();
        m_items.Offset.x = -size.x / 2.0f;
        m_items.Offset.y = -size.y / 2.0f;

        Accessibility_CreateItemFragment(visual);
    }


    void 
    DrawingIsland::Output_UpdateCurrentColorVisual()
    {
        m_items.CurrentColorVisual.Brush(m_output.ColorBrushes[m_output.CurrentColorIndex]);
        m_items.CurrentColorVisual.Offset({0.0f, m_island.ActualSize().y - 25.0f, 0.0f});
        m_items.CurrentColorVisual.Size({m_island.ActualSize().x, 25.0f});
    }


    void
    DrawingIsland::Output_UpdateSystemBackdrop()
    {
        if (m_background.BackdropEnabled && nullptr == m_background.BackdropController)
        {
            // Reduce the opacity of the background color visual so that we can see
            // the Mica/DesktopAcrylic behind it.
            m_background.Visual.Opacity(0.1f);

            // Create a new SystemBackdropConfiguration if we do not already have one.
            if (nullptr == m_background.BackdropConfiguration)
            {
                m_background.BackdropConfiguration = winrt::SystemBackdropConfiguration();
            }

            // Decide between Mica and DesktopAcrylic.
            if (winrt::MicaController::IsSupported())
            {
                m_background.BackdropController = winrt::MicaController();
                
            }
            else
            {
                m_background.BackdropController = winrt::DesktopAcrylicController();
            }

            // Link the SystemBackdropConfiguration to our SystemBackdropController.
            m_background.BackdropController.SetSystemBackdropConfiguration(
                m_background.BackdropConfiguration);

            // Add our island as the target.
            m_background.BackdropController.AddSystemBackdropTarget(m_island);
        }
        else
        {
            // Reset the background opacity to 1.0
            m_background.Visual.Opacity(1.0f);

            if (nullptr != m_background.BackdropController)
            {
                // Clean up our SystemBackdropController.
                m_background.BackdropController.RemoveAllSystemBackdropTargets();
                m_background.BackdropController.Close();
                m_background.BackdropController = nullptr;
            }
        }
    }

    void
    DrawingIsland::Environment_Initialize()
    {
        auto window = m_island.Environment();

        (void)window.SettingChanged(
            [this](winrt::ContentIslandEnvironment const&,
                winrt::ContentEnvironmentSettingChangedEventArgs const& args)
        {
            return Environment_OnSettingChanged(args);
        });

        (void)window.StateChanged(
            [this](winrt::ContentIslandEnvironment const& sender, winrt::IInspectable const&)
        {
            return Environment_OnStateChanged(sender);
        });
    }


    void
    DrawingIsland::Environment_OnSettingChanged(
        const winrt::ContentEnvironmentSettingChangedEventArgs& args)
    {
        auto settingChanged = args.SettingName();

        if (settingChanged == L"intl")
        {
            m_background.Visual.Brush(m_background.BrushA);
        }
    }


    void
    DrawingIsland::Environment_OnStateChanged(winrt::ContentIslandEnvironment const& /*sender*/)
    {

    }
}  // namespace winrt::OneTest::ContentIslandHelpers::implementation
