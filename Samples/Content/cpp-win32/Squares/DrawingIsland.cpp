// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DrawingIsland.h"
#include "NodeSimpleFragment.h"
#include "IslandFragmentRoot.h"

namespace Squares
{
    DrawingIsland::DrawingIsland(
        const winrt::Compositor& compositor)
    {
        m_compositor = compositor;

        m_island = winrt::ContentIsland::Create(compositor);

        Output_Initialize();
        Input_Initialize();
        Accessibility_Initialize();
        Window_Initialize();
    }


    DrawingIsland::~DrawingIsland()
    {
        m_fragmentRoot = nullptr;
        m_fragmentFactory = nullptr;
        m_compositor = nullptr;
    }


    void
    DrawingIsland::Close()
    {
        m_visuals = nullptr;
        m_selectedVisual = nullptr;
        m_backgroundBrushDefault = nullptr;
        m_backgroundBrushA = nullptr;
        m_backgroundBrushB = nullptr;
        m_backgroundBrushC = nullptr;
        m_backgroundVisual = nullptr;

        // Destroy Content:
        m_island.Close();
        m_island = nullptr;
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

        // Create the background parent Visual that the individual square will be added into.
        m_backgroundBrushDefault = m_compositor.CreateColorBrush(winrt::Color{ 0x99, 0x20, 0x20, 0x20 });
        m_backgroundBrushA = m_compositor.CreateColorBrush(winrt::Color{ 0x99, 0x99, 0x20, 0x20 });
        m_backgroundBrushB = m_compositor.CreateColorBrush(winrt::Color{ 0x99, 0x20, 0x99, 0x20 });
        m_backgroundBrushC = m_compositor.CreateColorBrush(winrt::Color{ 0x99, 0x20, 0x20, 0x99 });

        m_backgroundVisual = m_compositor.CreateSpriteVisual();
        m_backgroundVisual.Brush(m_backgroundBrushDefault);
        m_backgroundVisual.RelativeSizeAdjustment(winrt::float2(1, 1));
        m_island.Root(m_backgroundVisual);

        winrt::ContainerVisual drawingVisualsRoot = m_compositor.CreateContainerVisual();
        m_visuals = drawingVisualsRoot.Children();
        m_backgroundVisual.Children().InsertAtTop(drawingVisualsRoot);
    }


    void
    DrawingIsland::Output_AddVisual(
        winrt::float2 const point,
        bool halfTransparent)
    {
        winrt::SpriteVisual visual = m_compositor.CreateSpriteVisual();
        visual.Brush(halfTransparent ?
            m_halfTransparentColorBrushes[m_currentColorIndex] :
            m_colorBrushes[m_currentColorIndex]);

        m_currentColorIndex = (m_currentColorIndex + 1) % 4;

        float const BlockSize = 30.0f;
        visual.Size({ BlockSize, BlockSize });
        visual.Offset({ point.x - BlockSize / 2.0f, point.y - BlockSize / 2.0f, 0.0f });

        m_visuals.InsertAtTop(visual);

        m_selectedVisual = visual;
        m_offset.x = -BlockSize / 2.0f;
        m_offset.y = -BlockSize / 2.0f;

        CreateUIAProviderForVisual();

        Accessibility_UpdateScreenCoordinates(m_selectedVisual);
    }


    void
    DrawingIsland::Input_Initialize()
    {
        auto pointerSource = winrt::InputPointerSource::GetForIsland(m_island);

        pointerSource.PointerPressed(
            [this](winrt::InputPointerSource const&,
                winrt::PointerEventArgs const& args)
            {
                auto currentPoint = args.CurrentPoint();
                auto properties = currentPoint.Properties();

                if (properties.IsLeftButtonPressed())
                {
                    Input_OnLeftButtonPressed(args);
                }
            });

        pointerSource.PointerMoved(
            [this](winrt::InputPointerSource const&,
                winrt::PointerEventArgs const& args)
            {
                Input_OnPointerMoved(args);
            });

        pointerSource.PointerReleased(
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
                    m_backgroundVisual.Opacity(0.5f);
                    break;
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
        }

        return handled;
    }

    winrt::Visual
    DrawingIsland::HitTestVisual(
        winrt::float2 const point)
    {
        winrt::Visual selectedVisual{ nullptr };
        for (winrt::Visual visual : m_visuals)
        {
            winrt::float3 const offset = visual.Offset();
            winrt::float2 const size = visual.Size();

            if (point.x >= offset.x &&
                point.x < offset.x + size.x &&
                point.y >= offset.y &&
                point.y < offset.y + size.y)
            {
                selectedVisual = visual;
            }
        }

        return selectedVisual;
    }


    void
    DrawingIsland::Input_OnLeftButtonPressed(
        const winrt::PointerEventArgs& args)
    {
        // Left button manipulates the custom-drawn content
        winrt::float2 const point = args.CurrentPoint().Position();

        bool controlPressed = WI_IsFlagSet(
            args.KeyModifiers(),
            winrt::Windows::System::VirtualKeyModifiers::Control);

        OnLeftClick(point, controlPressed);
    }


    void
    DrawingIsland::Input_OnPointerReleased()
    {
        m_selectedVisual = nullptr;
    }


    void
    DrawingIsland::OnLeftClick(
        const winrt::float2 point,
        bool controlPressed)
    {
        m_selectedVisual = HitTestVisual(point);

        if (m_selectedVisual)
        {
            winrt::float3 const offset = m_selectedVisual.Offset();

            m_offset.x = offset.x - point.x;
            m_offset.y = offset.y - point.y;

            m_visuals.Remove(m_selectedVisual);
            m_visuals.InsertAtTop(m_selectedVisual);
        }
        else
        {
            Output_AddVisual(point, controlPressed);
        }

        m_keyboardSource.TrySetFocus();
    }


    void
    DrawingIsland::Input_OnPointerMoved(
        const winrt::PointerEventArgs& args)
    {
        if (m_selectedVisual)
        {
            winrt::float2 const point = args.CurrentPoint().Position();
            m_selectedVisual.Offset({ point.x + m_offset.x, point.y + m_offset.y, 0.0f });
        }
    }


    void
    DrawingIsland::Accessibility_Initialize()
    {
        m_fragmentRoot = winrt::make_self<IslandFragmentRoot>(m_island);
        m_fragmentRoot->SetName(L"Content Island");

        m_fragmentFactory = winrt::make_self<NodeSimpleFragmentFactory>();


        (void)m_island.AutomationProviderRequested(
            [this](winrt::ContentIsland const& sender,
                   winrt::ContentIslandAutomationProviderRequestedEventArgs const& args)
            {
                return Accessibility_OnAutomationProviderRequested(sender, args);
            });
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


    void
    DrawingIsland::Accessibility_UpdateScreenCoordinates(winrt::Visual visual)
    {
        winrt::Rect logicalRect;
        logicalRect.X = visual.Offset().x;
        logicalRect.Y = visual.Offset().y;
        logicalRect.Width = visual.Size().x;
        logicalRect.Height = visual.Size().y;

        auto fragment = m_visualToFragmentMap[visual];

        // This will convert from the logical coordinate space of the ContentIsland to
        // Win32 screen coordinates that are needed by Accesibility.
        auto screenRect = m_island.Coordinates().ComputeScreenCoordinates(logicalRect);

        UiaRect uiaRect;
        uiaRect.left = screenRect.X;
        uiaRect.top = screenRect.Y;
        uiaRect.width = screenRect.Width;
        uiaRect.height = screenRect.Height;
        
        fragment->SetBoundingRects(uiaRect);
    }


    void
    DrawingIsland::CreateUIAProviderForVisual()
    {
        winrt::com_ptr<NodeSimpleFragment> fragment = m_fragmentFactory->Create(s_colorNames[m_currentColorIndex].c_str(), m_fragmentRoot);

        m_visualToFragmentMap[m_selectedVisual] = fragment;

        // Set up children roots.
        m_fragmentRoot->AddChild(fragment);

        // Finally set up parent chain.
        fragment->SetParent(m_fragmentRoot);
    }

    void
    DrawingIsland::Window_Initialize()
    {
        auto window = m_island.Window();

        (void)window.StateChanged(
            [this](winrt::IContentWindow const&, winrt::IInspectable const&)
            {
                return Window_StateChanged();
            });
    }

    void
    DrawingIsland::Window_StateChanged()
    {
        bool isLandscapeOrientation =
            (m_island.Window().CurrentOrientation() == winrt::ContentDisplayOrientations::Landscape ||
                m_island.Window().CurrentOrientation() == winrt::ContentDisplayOrientations::LandscapeFlipped);
        if (isLandscapeOrientation)
        {
            m_backgroundBrushDefault = m_compositor.CreateColorBrush(winrt::Colors::DarkOrange());
            m_backgroundVisual.Brush(m_backgroundBrushDefault);
        }
        else
        {
            m_backgroundBrushDefault = m_compositor.CreateColorBrush(winrt::Colors::DarkGray());
            m_backgroundVisual.Brush(m_backgroundBrushDefault);
        }
    }
}  
