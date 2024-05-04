// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "DrawingIsland.h"
#include "DrawingIsland.g.cpp"

#include "IslandFragmentRoot.h"
#include "NodeSimpleFragment.h"

namespace winrt::DrawingIslandComponents::implementation
{
    DrawingIsland::DrawingIsland(
        const winrt::Compositor& compositor)
    {
        m_compositor = compositor;
        m_textRenderer = std::make_shared<TextRenderer>(m_compositor);

        // Create the Compositor and the Content:
        // - The Bridge's connection to the Window will keep everything alive, and perform an orderly
        //   tear-down:
        //
        //   Window -> Bridge -> Content -> Visual -> InputSite -> InputObject
        //
        // - When the ContentIsland is destroyed, ContentIsland.AppData will call IClosable.Close on
        //   this instance to break cycles and clean up expensive resources.

        m_backgroundVisual = m_compositor.CreateSpriteVisual();
        m_backgroundVisual.RelativeSizeAdjustment(float2(1.0f, 1.0f));

        m_island = winrt::ContentIsland::Create(m_backgroundVisual);
        m_island.AppData(get_strong().as<winrt::IInspectable>());

        Output_Initialize();
        Input_Initialize();
        Window_Initialize();
        Accessibility_Initialize();

        m_prevRasterizationScale = m_island.RasterizationScale();

        // Get notifications for resize, bridge changes, etc.
        (void)m_island.StateChanged(
            [&](auto&& ...)
        {
            return Island_OnStateChanged();
        });

        // Just adding EnqueueFromBackgroundThread method for testing.
        EnqueueFromBackgroundThread();

#if FALSE
        // Get notifications for island disconnection.
        (void)m_island.Connected(
            [&](auto&& ...)
        {
            return Island_OnConnected();
        });

        (void)m_island.Disconnected(
            [&](auto&& ...)
        {
            return Island_OnDisconnected();
        });
#endif

        (void)m_island.Closed(
            [&]()
        {
            return Island_OnClosed();
        });


    }


    DrawingIsland::~DrawingIsland()
    {
#if FALSE
        m_siteBridge = nullptr;
#endif
#if TRUE
        m_fragmentRoot = nullptr;
        m_fragmentFactory = nullptr;
#endif
        m_compositor = nullptr;
    }


    void
    DrawingIsland::Close()
    {
        m_visuals = nullptr;
        m_visualElements.clear();
        m_selectedVisual = nullptr;
        m_backgroundBrushDefault = nullptr;
        m_backgroundBrushA = nullptr;
        m_backgroundBrushB = nullptr;
        m_backgroundBrushC = nullptr;
        m_backgroundVisual = nullptr;

#if FALSE
        // Destroy SystemBackdropController objects.
        if (m_backdropController != nullptr)
        {
            if (m_backdropConfiguration != nullptr)
            {
                // m_backdropConfiguration is only initialized for the DrawingIsland that owns the system backdrop controller.
                m_backdropController.Close();
                m_backdropController = nullptr;
                m_backdropConfiguration = nullptr;
            }
            else
            {
                // We're closing a DrawingIsland in a popup, not the owner DrawingIsland of the system backdrop controller.
                // Remove the current target from the system backdrop controller.
                m_backdropController.RemoveSystemBackdropTarget(m_backdropTarget);
            }
        }
#endif

        // Destroy Content:
        // - This handles if the ContentIsland has already started destruction and is notifying the app.
        m_island.Close();
        m_island = nullptr;

        // TODO: Add the following test case in automated tests:
        // 1. We are recursively calling ContentIsland.Close while inside the ContentIsland.Closed
        // event.
        // 2. We are testing the potential final Release() of ContentIsland while inside the Closed
        // event.
    }

    // Adding this method just for testing.
    void
    DrawingIsland::EnqueueFromBackgroundThread()
    {
        std::thread updateThread1{ [&]()
        {
            winrt::Microsoft::UI::Dispatching::DispatcherQueue dispatcherQueue = m_island.DispatcherQueue();

            bool result = dispatcherQueue.TryEnqueue(winrt::Microsoft::UI::Dispatching::DispatcherQueuePriority::High, [&]
            {
                winrt::Windows::Foundation::Numerics::float2 size = { 500, 500 };
                m_island.RequestSize(size);
            });

            if (!result)
            {
            }
        } };
        updateThread1.join();
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


    void 
    DrawingIsland::EvaluateUseSystemBackdrop()
    {
        BYTE backgroundBrushOpacity = 0xFF;
        // If we use system backdrops, it will be behind our background visual. Make sure we can
        // see through the background visual to reveal the system backdrop.
        // reveal the system backdrop underneath.
        if (m_useSystemBackdrop)
        {
            backgroundBrushOpacity = 0x30;
        }

        // Create the background parent Visual that the individual square will be added into.
        m_backgroundBrushDefault = m_compositor.CreateColorBrush(winrt::Color{ backgroundBrushOpacity, 0x20, 0x20, 0x20 });
        m_backgroundBrushA = m_compositor.CreateColorBrush(winrt::Color{ backgroundBrushOpacity, 0x99, 0x20, 0x20 });
        m_backgroundBrushB = m_compositor.CreateColorBrush(winrt::Color{ backgroundBrushOpacity, 0x20, 0x99, 0x20 });
        m_backgroundBrushC = m_compositor.CreateColorBrush(winrt::Color{ backgroundBrushOpacity, 0x20, 0x20, 0x99 });

        m_backgroundVisual.Brush(m_backgroundBrushDefault);
    }


    void 
    DrawingIsland::UseSystemBackdrop(
        boolean value)
    {
        if (m_useSystemBackdrop != value)
        {
            m_useSystemBackdrop = value;

            EvaluateUseSystemBackdrop();
        }
    }


    void
    DrawingIsland::IgnoreLeftButtonPressed(
        boolean value)
    {
        if (m_ignoreLeftButtonPressed != value)
        {
            m_ignoreLeftButtonPressed = value;
        }
    }

    void
    DrawingIsland::Input_Initialize()
    {
        m_pointerSource = winrt::InputPointerSource::GetForIsland(m_island);

        m_pointerSource.PointerPressed(
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

        m_pointerSource.PointerMoved(
            [this](winrt::InputPointerSource const&,
                   winrt::PointerEventArgs const& args)
            {
                Input_OnPointerMoved(args);
            });

        m_pointerSource.PointerReleased(
            [&](auto&& ...)
            {
                Input_OnPointerReleased();
            });

        // Set up the keyboard source. We store this in a member variable so we can easily call 
        // TrySetFocus() in response to left clicks in the content later on.
        m_keyboardSource = winrt::InputKeyboardSource::GetForIsland(m_island);

        m_keyboardSource.KeyDown(
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

        m_pretranslateSource = winrt::InputPreTranslateKeyboardSource::GetForIsland(m_island);

        m_pretranslateSource.as<Microsoft::UI::Input::IInputPreTranslateKeyboardSourceInterop>()->SetPreTranslateHandler(this);

        auto activationListener = winrt::InputActivationListener::GetForIsland(m_island);
        (void)activationListener.InputActivationChanged(
            [this, activationListener](
                winrt::InputActivationListener const&,
                winrt::InputActivationListenerActivationChangedEventArgs const&)
        {
            switch (activationListener.State())
            {
            case winrt::InputActivationState::Activated:
                m_backgroundVisual.Opacity(1.0f);
                break;

            default:
                m_backgroundVisual.Opacity(m_backgroundOpacity);
                break;
            }
        });

        m_focusController = winrt::InputFocusController::GetForIsland(m_island);
        m_focusController.NavigateFocusRequested(
            [this](winrt::InputFocusController const&, winrt::FocusNavigationRequestEventArgs const& args) {
                bool setFocus = m_focusController.TrySetFocus();
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
                m_backgroundVisual.Brush(m_backgroundBrushA);
                handled = true;
                break;
            }

            case winrt::VirtualKey::B:
            {
                m_backgroundVisual.Brush(m_backgroundBrushB);
                handled = true;
                break;
            }

            case winrt::VirtualKey::C:
            {
                m_backgroundVisual.Brush(m_backgroundBrushC);
                handled = true;
                break;
            }

            case winrt::VirtualKey::Space:
            {
                m_backgroundVisual.Brush(m_backgroundBrushDefault);
                break;
            }
                
            case winrt::Windows::System::VirtualKey::Number1:
            {
                m_currentColorIndex = 0;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Number2:
            {
                m_currentColorIndex = 1;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Number3:
            {
                m_currentColorIndex = 2;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Number4:
            {
                m_currentColorIndex = 3;
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Delete:
            case winrt::Windows::System::VirtualKey::Escape:
            {
                m_visuals.RemoveAll();
                m_visualElements.clear();
                handled = true;
                break;
            }

            case winrt::Windows::System::VirtualKey::Tab:
            {
                auto request = winrt::Microsoft::UI::Input::FocusNavigationRequest::Create(
                    winrt::Microsoft::UI::Input::FocusNavigationReason::First);
                m_focusController.DepartFocus(request);
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
                m_backgroundVisual.Brush(m_backgroundBrushA);
                handled = true;
                break;
            }

            case winrt::VirtualKey::Y:
            {
                m_backgroundVisual.Brush(m_backgroundBrushB);
                handled = true;
                break;
            }

            case winrt::VirtualKey::Z:
            {
                m_backgroundVisual.Brush(m_backgroundBrushC);
                handled = true;
                break;
            }
        }

        return handled;
    }


#if TRUE
    void
    DrawingIsland::Accessibility_Initialize()
    {
        m_fragmentRoot = winrt::make_self<IslandFragmentRoot>(m_island);
        m_fragmentRoot->SetName(L"Drawing Squares");

        m_fragmentFactory = winrt::make_self<NodeSimpleFragmentFactory>();

#if FALSE
        if (m_crossProcUIA)
        {
            m_fragmentRoot->SetName(L"CrossProc Island");
        }
        else if (m_siteBridge != nullptr)
        {
            auto desktopChildBridge = m_siteBridge.try_as<winrt::IDesktopChildSiteBridge>();
            if (desktopChildBridge != nullptr)
            {
                m_fragmentRoot->SetName(L"Child Island");
            }
            else
            {
                m_fragmentRoot->SetName(L"Popup Island");
            }
        }
#endif

        (void)m_island.AutomationProviderRequested({ this, &DrawingIsland::Accessibility_OnAutomationProviderRequested });
    }


    void
    DrawingIsland::Accessibility_OnAutomationProviderRequested(
        const winrt::ContentIsland& /*island*/,
        const winrt::ContentIslandAutomationProviderRequestedEventArgs& args)
    {
        IInspectable providerAsIInspectable;
        m_fragmentRoot->QueryInterface(winrt::guid_of<IInspectable>(), winrt::put_abi(providerAsIInspectable));
        args.AutomationProvider(std::move(providerAsIInspectable));

        args.Handled(true);
    }
#endif


    VisualElement*
    DrawingIsland::HitTestVisual(
        float2 const point)
    {
        VisualElement* selectedElement{ nullptr };
        for (auto& element : m_visualElements)
        {
            auto visual = element->GetVisual();
            winrt::float3 const offset = visual.Offset();
            float2 const size = visual.Size();

            if (point.x >= offset.x &&
                point.x < offset.x + size.x &&
                point.y >= offset.y &&
                point.y < offset.y + size.y)
            {
                selectedElement = element.get();
            }
        }

        return selectedElement;
    }


    // Continue converting to inspect CTRL key.

    void
    DrawingIsland::Input_OnLeftButtonPressed(
        const winrt::PointerEventArgs& args)
    {
        if (!m_ignoreLeftButtonPressed)
        {
            // Left button manipulates the custom-drawn content
            float2 const point = args.CurrentPoint().Position();

            bool controlPressed = WI_IsFlagSet(
                args.KeyModifiers(),
                winrt::Windows::System::VirtualKeyModifiers::Control);

            OnLeftClick(point, controlPressed);
        }
    }

    void
    DrawingIsland::Input_OnPointerReleased()
    {
        m_selectedVisual = nullptr;
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
        m_selectedVisual = HitTestVisual(point);
        
        if (m_selectedVisual != nullptr)
        {
            auto& visual = m_selectedVisual->GetVisual();
            winrt::float3 const offset = visual.Offset();

            m_offset.x = offset.x - point.x;
            m_offset.y = offset.y - point.y;

            m_visuals.Remove(visual);
            m_visuals.InsertAtTop(visual);

            // Move the selected text block to the end of the vector if it isn't already.
            if (!m_visualElements.empty() && m_visualElements.back().get() != m_selectedVisual)
            {
                auto p = m_selectedVisual;
                auto i = std::find_if(
                    m_visualElements.begin(),
                    m_visualElements.end(),
                    [p](auto& elem) { return elem.get() == p; }
                );
                if (i != m_visualElements.end())
                {
                    std::rotate(i, i + 1, m_visualElements.end());
                }
            }

            // TODO: The m_fragmentRoots child should be removed and added to the end as well.
        }
        else
        {
            Output_AddVisual(point, controlPressed);
        }

#if FALSE
        // Only transfer focus when we are hosted inside a DesktopChildSiteBridge. When we are
        // hosted inside a PopupWindowSiteBridge, we expect to control focus and activaton by 
        // setting InputPointerSource->ActivationBehavior.
        auto desktopChildBridge = m_siteBridge.try_as<winrt::IDesktopChildSiteBridge>();
        if (desktopChildBridge != nullptr)
        {
            m_focusController.TrySetFocus();
        }
#endif
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
        // TODO - what is meant to happen here?
        UNREFERENCED_PARAMETER(point);
        //TextBlock* selectedTextBlock = HitTestVisual(point);
    }

    void
    DrawingIsland::Input_OnPointerMoved(
        const winrt::PointerEventArgs& args)
    {
        if (m_selectedVisual != nullptr)
        {
            float2 const point = args.CurrentPoint().Position();
            m_selectedVisual->GetVisual().Offset({ point.x + m_offset.x, point.y + m_offset.y, 0.0f });
        }
    }


    void
    DrawingIsland::Island_OnStateChanged()
    {
        if (m_prevRasterizationScale != m_island.RasterizationScale())
        {
            m_prevRasterizationScale = m_island.RasterizationScale();

            m_textRenderer->SetDpiScale(m_prevRasterizationScale);

            for (auto& element : m_visualElements)
            {
                element->OnDpiScaleChanged();
            }
        }

        if (m_prevLayout != m_island.LayoutDirection())
        {
            SetLayoutDirectionForVisuals();
        }

        Output_UpdateCurrentColorVisual();
    }

#if FALSE
    void
    DrawingIsland::Island_OnConnected()
    {
        SetLayoutDirectionForVisuals();
    }
#endif

    void
    DrawingIsland::SetLayoutDirectionForVisuals()
    {
        if (m_island.LayoutDirection() == ContentLayoutDirection::RightToLeft)
        {
            // The following will mirror the visuals. If any text is inside the visuals the text
            // is also mirrored. The text will need another RelativeOffsetAdjustment and Scale to
            // return to the normal space.
            m_backgroundVisual.RelativeOffsetAdjustment(winrt::float3(1, 0, 0));
            m_backgroundVisual.Scale(winrt::float3(-1, 1, 1));
        }
        else
        {
            m_backgroundVisual.RelativeOffsetAdjustment(winrt::float3(0, 0, 0));
            m_backgroundVisual.Scale(winrt::float3(1, 1, 1));
        }
        m_prevLayout = m_island.LayoutDirection();
    }

#if FALSE
    void
    DrawingIsland::Island_OnDisconnected()
    {

    }
#endif

    void
    DrawingIsland::Island_OnClosed()
    {

    }

    void
    DrawingIsland::Output_Initialize()
    {
        for (int i = 0; i < _countof(m_colorBrushes); i++)
        {
            m_colorBrushes[i] = m_compositor.CreateColorBrush(s_colors[i]);

            winrt::Color halfTransparent = s_colors[i];
            halfTransparent.A = 0x80;
            m_halfTransparentColorBrushes[i] = m_compositor.CreateColorBrush(halfTransparent);
        }

        m_currentColorVisual = m_compositor.CreateSpriteVisual();
        m_currentColorVisual.Offset({0.0f, 0.0f, 0.0f});
        m_backgroundVisual.Children().InsertAtTop(m_currentColorVisual);

        winrt::ContainerVisual drawingVisualsRoot = m_compositor.CreateContainerVisual();
        m_visuals = drawingVisualsRoot.Children();
        m_backgroundVisual.Children().InsertAtTop(drawingVisualsRoot);

        EvaluateUseSystemBackdrop();

        Output_UpdateCurrentColorVisual();
    }

    static std::wstring GetTimeString()
    {
        SYSTEMTIME time;
        GetLocalTime(&time);
        wchar_t buffer[16];
        size_t length = swprintf_s(buffer, L"%d:%02d:%02d", time.wHour, time.wMinute, time.wSecond);
        return std::wstring(buffer, length);
    }

    void
    DrawingIsland::Output_AddVisual(
        float2 const point,
        bool halfTransparent)
    {
        // Determine the visual background and text colors.
        Color backgroundColor = s_colors[m_currentColorIndex];
        Color textColor = { 0xFF, 0, 0, 0 };
        if (halfTransparent)
        {
            backgroundColor.A /= 2;
            textColor.A /= 2;
        }

        // Get the visual brush.
        // TODO - this will go away later
        auto visualBrush = halfTransparent ?
            m_halfTransparentColorBrushes[m_currentColorIndex] :
            m_colorBrushes[m_currentColorIndex];

        // Create a text block object.
        auto textBlock = std::make_unique<TextElement>(
#if TRUE
            m_fragmentFactory,
            m_fragmentRoot,
#endif
            m_textRenderer,
            visualBrush,
            backgroundColor,
            textColor,
            GetTimeString()
        );

        // Get the visual and its size in DIPs.
        auto& visual = textBlock->GetVisual();
        float2 size = visual.Size();

        // Set the visual's offset.
        visual.Offset({ point.x - size.x / 2.0f, point.y - size.y / 2.0f, 0.0f });

        // Add the new text block to the vector of the text blocks.
        m_visualElements.push_back(std::move(textBlock));

        // Add the visual as a child of the container visual.
        m_visuals.InsertAtTop(visual);

        m_selectedVisual = m_visualElements.back().get();
        m_offset.x = -size.x / 2.0f;
        m_offset.y = -size.y / 2.0f;

#if TRUE
        Accessibility_UpdateScreenCoordinates(m_selectedVisual);
#endif
    }


#if TRUE
    void
    DrawingIsland::Accessibility_UpdateScreenCoordinates(VisualElement* textBlock)
    {
        auto& visual = textBlock->GetVisual();
        auto& fragment = textBlock->GetFragment();

        winrt::Rect logicalRect;
        logicalRect.X = visual.Offset().x;
        logicalRect.Y = visual.Offset().y;
        logicalRect.Width = visual.Size().x;
        logicalRect.Height = visual.Size().y;

        // This will convert from the logical coordinate space of the ContentIsland to
        // Win32 screen coordinates that are needed by Accesibility.
        auto screenRect = m_island.CoordinateConverter().ConvertLocalToScreen(logicalRect);

        UiaRect uiaRect;
        uiaRect.left = screenRect.X;
        uiaRect.top = screenRect.Y;
        uiaRect.width = screenRect.Width;
        uiaRect.height = screenRect.Height;
        
        fragment->SetBoundingRects(uiaRect);
    }
#endif


    void 
    DrawingIsland::Output_UpdateCurrentColorVisual()
    {
        m_currentColorVisual.Brush(m_colorBrushes[m_currentColorIndex]);
        m_currentColorVisual.Offset({0.0f, m_island.ActualSize().y - 25.0f, 0.0f});
        m_currentColorVisual.Size({m_island.ActualSize().x, 25.0f});
    }


#if FALSE
    void
    DrawingIsland::SystemBackdrop_Initialize()
    {
        // Don't initilize system backdrop if we haven't been configured for it.
        if (!m_useSystemBackdrop) return;

        if (m_backdropController == nullptr)
        {
            m_backdropConfiguration = winrt::SystemBackdropConfiguration();
            m_backdropController = winrt::DesktopAcrylicController();
            m_backdropController.SetSystemBackdropConfiguration(m_backdropConfiguration);

            auto activationListener = winrt::InputActivationListener::GetForIsland(m_island);
            (void)activationListener.InputActivationChanged(
                [this, activationListener](
                    winrt::InputActivationListener const&,
                    winrt::InputActivationListenerActivationChangedEventArgs const&)
            {
                switch (activationListener.State())
                {
                case winrt::InputActivationState::Activated:
                    m_backdropConfiguration.IsInputActive(true);
                    break;

                default:
                    m_backdropConfiguration.IsInputActive(false);
                    break;
                }
            });
        }


        if (IsHostedByPopupWindowSiteBridge())
        {
            // For popups, we want to draw shadows around the edges, so clip the backdrop visual to
            // allow room on the edges for the shadows.

            m_backdropLink = winrt::ContentExternalBackdropLink::Create(m_compositor);

            // This will be the size of the "cut out" we will make in the lifted composition surface 
            // so that the Backdrop System Sprite Visual will show through. This is specified in 
            // logical coordinates.
            m_backdropLink.PlacementVisual().Size(m_island.ActualSize());

            // Clip the backdrop.
            m_backdropClip = m_compositor.CreateRectangleClip(
                10.0f,
                10.0f, 
                m_island.ActualSize().x - 10.0f,
                m_island.ActualSize().y - 10.0f,
                {10.0f, 10.0f},
                {10.0f, 10.0f},
                {10.0f, 10.0f},
                {10.0f, 10.0f});
            m_backdropLink.PlacementVisual().Clip(m_backdropClip);

            // Clip the overall background.
            m_backgroundClip = m_compositor.CreateRectangleClip(
                0.0f,
                0.0f, 
                m_island.ActualSize().x,
                m_island.ActualSize().y,
                {10.0f, 10.0f},
                {10.0f, 10.0f},
                {10.0f, 10.0f},
                {10.0f, 10.0f});
            m_backgroundVisual.Clip(m_backgroundClip);
            
            // Add the backdropLink into the LiftedVisual tree of the popup.
            m_backgroundVisual.Children().InsertAtBottom(m_backdropLink.PlacementVisual());

            auto animation = m_compositor.CreateVector3KeyFrameAnimation();
            animation.InsertKeyFrame(0.0f, { 0.0f, -m_island.ActualSize().y, 0.0f });
            animation.InsertKeyFrame(1.0f, { 0.0f, 0.0f, 0.0f });
            animation.Duration(std::chrono::milliseconds(2000));
            animation.IterationBehavior(AnimationIterationBehavior::Count);
            animation.IterationCount(1);
            m_backgroundVisual.StartAnimation(L"Offset", animation);

            // For Popups, we want to customize the clip and offset of the system backdrop, so we 
            // pass the ContentExternalBackdropLink as the target to the BackdropController. 

            m_backdropTarget = m_backdropLink;
        }
        else
        {
            // If we are the main content, we don't want to add custom clips or offsets to our 
            // backdrop, so we can pass the ContentIsland as the target to the BackdropController. 
            // This will by default fill the entire ContentIsland backdrop surface.

            m_backdropTarget = m_island;
        }
    
        m_backdropController.AddSystemBackdropTarget(m_backdropTarget);
    }
#endif

    void
    DrawingIsland::Window_Initialize()
    {
        auto window = m_island.Environment();

        (void)window.SettingChanged(
            [this](winrt::ContentIslandEnvironment const&, winrt::ContentEnvironmentSettingChangedEventArgs const& args)
        {
            return Window_OnSettingChanged(args);
        });

        (void)window.StateChanged(
            [this](winrt::ContentIslandEnvironment const& sender, winrt::IInspectable const&)
        {
            return Window_OnStateChanged(sender);
        });
    }


    void
    DrawingIsland::Window_OnSettingChanged(
        const winrt::ContentEnvironmentSettingChangedEventArgs& args)
    {
        auto settingChanged = args.SettingName();

        if (settingChanged == L"intl")
        {
            m_backgroundVisual.Brush(m_backgroundBrushA);
        }
    }


    void
    DrawingIsland::Window_OnStateChanged(winrt::ContentIslandEnvironment const& /*sender*/)
    {

    }
}  // namespace winrt::OneTest::ContentIslandHelpers::implementation
