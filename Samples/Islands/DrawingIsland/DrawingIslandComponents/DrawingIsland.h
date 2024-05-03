// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DrawingIsland.g.h"
#include "NodeSimpleFragment.h"
#include "TextRenderer.h"
#include "TextBlock.h"

namespace winrt::DrawingIslandComponents::implementation
{
    struct DrawingIsland :
        public DrawingIslandT<
        DrawingIsland,
        Microsoft::UI::Input::IInputPreTranslateKeyboardSourceHandler>
    {
    public:
        DrawingIsland(
            const winrt::Compositor& compositor);

        ~DrawingIsland();

        // IClosable methods
        void Close();

        // Properties 
        boolean UseSystemBackdrop()
        {
            return m_useSystemBackdrop;
        }

        void UseSystemBackdrop(
            boolean value);

        boolean IgnoreLeftButtonPressed()
        {
            return m_ignoreLeftButtonPressed;
        }

        void IgnoreLeftButtonPressed(
            boolean value);

        winrt::ContentIsland Island() const
        {
            return m_island;
        }

        // Methods

        void LeftClickAndRelease(
            const float2 currentPoint);

        void RightClickAndRelease(
            const float2 currentPoint);

#if FALSE
        void SetHostBridge(
            const winrt::IContentSiteBridge& bridge)
        {
            m_siteBridge = bridge;

            Accessibility_Initialize();
        }
#endif

        void SetBackroundOpacity(float backgroundOpacity)
        {
            m_backgroundOpacity = backgroundOpacity;
        }

        void SetColorIndex(std::uint32_t colorIndex)
        {
            m_currentColorIndex = std::clamp<std::uint32_t>(colorIndex, 0, _countof(s_colors) - 1);
            if (m_currentColorIndex >= 4)
            {
                m_backgroundBrushDefault = m_compositor.CreateColorBrush(s_colors[m_currentColorIndex]);

                m_backgroundVisual.Brush(m_backgroundBrushDefault);
            }
            Output_UpdateCurrentColorVisual();
        }

        // IInputKeyboardSourcePreTranslateHandler methods
        IFACEMETHOD(OnDirectMessage)(
            IInputPreTranslateKeyboardSourceInterop* source,
            const MSG* msg,
            UINT keyboardModifiers,
            _Inout_ bool* handled);

        IFACEMETHOD(OnTreeMessage)(
            IInputPreTranslateKeyboardSourceInterop* source,
            const MSG* msg,
            UINT keyboardModifiers,
            _Inout_ bool* handled);

    private:
#if TRUE
        void Accessibility_Initialize();

        void Accessibility_OnAutomationProviderRequested(
            const winrt::ContentIsland& island,
            const winrt::ContentIslandAutomationProviderRequestedEventArgs& args);

        void Accessibility_UpdateScreenCoordinates(
            const winrt::Visual& visual);

        void CreateUIAProviderForVisual();
#endif

        void EnqueueFromBackgroundThread();

        void EvaluateUseSystemBackdrop();

        winrt::Visual HitTestVisual(
            float2 const point);

        void Input_Initialize();

        bool Input_OnKeyDown(
            winrt::Windows::System::VirtualKey virtualKey);

        bool Input_AcceleratorKeyActivated(
            winrt::Windows::System::VirtualKey virtualKey);

        void Input_OnLeftButtonPressed(
            const winrt::PointerEventArgs& args);

        void Input_OnRightButtonPressed(
            const winrt::PointerEventArgs& args);

        void Input_OnPointerMoved(
            const winrt::PointerEventArgs& args);

        void Input_OnPointerReleased();

        void Island_OnStateChanged();

#if FALSE
        void Island_OnConnected();

        void Island_OnDisconnected();
#endif

        void Island_OnClosed();

        void LightDismiss_Initialize();

        void OnLeftClick(
            const float2 point,
            bool controlPressed);

        void OnRightClick(
            const float2 currentPoint);

        void Output_Initialize();

        void Output_AddVisual(
            const float2 point,
            bool halfTransparent);

        void Output_UpdateCurrentColorVisual();

#if FALSE
        void SystemBackdrop_Initialize();
#endif

        void SetLayoutDirectionForVisuals();

        void Window_Initialize();

        void Window_OnSettingChanged(
            const winrt::ContentEnvironmentSettingChangedEventArgs& args);

        void Window_OnStateChanged(winrt::ContentIslandEnvironment const& sender);

    private:
        static inline winrt::Color s_colors[] =
        {
            { 0xFF, 0x5B, 0x9B, 0xD5 },
            { 0xFF, 0xED, 0x7D, 0x31 },
            { 0xFF, 0x70, 0xAD, 0x47 },
            { 0xFF, 0xFF, 0xC0, 0x00 },
            { 0xFF, 0xFA, 0xEB, 0xD7 },
            { 0xFF, 0xFF, 0xFF, 0xFF },
            { 0xFF, 0xFF, 0xFA, 0xFA },
            { 0xFF, 0xFF, 0xC0, 0xCB },
            { 0xFF, 0xB0, 0xE0, 0xE6 },
            { 0xFF, 0x98, 0xFB, 0x98 },
            { 0xFF, 0x87, 0xCE, 0xFA },
        };

        static inline std::wstring s_colorNames[] =
        {
            L"Blue",
            L"Orange",
            L"Green",
            L"Yellow",
            L"AntiqueWhite",
            L"White",
            L"Snow",
            L"Pink",
            L"PowderBlue",
            L"PaleGreen",
            L"LightSkyBlue",
        };

#if TRUE
        winrt::com_ptr<NodeSimpleFragmentFactory> m_fragmentFactory{ nullptr };
        winrt::com_ptr<IslandFragmentRoot> m_fragmentRoot{ nullptr };
#endif

        winrt::Compositor m_compositor{ nullptr };
        winrt::ContentIsland m_island{ nullptr };
        winrt::InputKeyboardSource m_keyboardSource{ nullptr };
        winrt::InputPreTranslateKeyboardSource m_pretranslateSource{ nullptr };
        winrt::InputPointerSource m_pointerSource{ nullptr };
        winrt::InputFocusController m_focusController{ nullptr };

        // Background
        winrt::CompositionColorBrush m_backgroundBrushDefault{ nullptr };
        winrt::CompositionColorBrush m_backgroundBrushA{ nullptr };
        winrt::CompositionColorBrush m_backgroundBrushB{ nullptr };
        winrt::CompositionColorBrush m_backgroundBrushC{ nullptr };
        winrt::SpriteVisual m_backgroundVisual{ nullptr };
        winrt::RectangleClip m_backgroundClip{ nullptr };
        winrt::RectangleClip m_backdropClip{ nullptr };
#if FALSE
        winrt::ContentExternalBackdropLink m_backdropLink{ nullptr };
        winrt::ICompositionSupportsSystemBackdrop m_backdropTarget{ nullptr };
#endif

        // Drawing squares
        winrt::VisualCollection m_visuals{ nullptr };
        winrt::Visual m_selectedVisual{ nullptr };
        winrt::SpriteVisual m_currentColorVisual{ nullptr };
        float2 m_offset{};
        float m_backgroundOpacity = 0.5f;

        unsigned int m_currentColorIndex = 0;
        winrt::CompositionColorBrush m_colorBrushes[_countof(s_colors)]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        winrt::CompositionColorBrush m_halfTransparentColorBrushes[_countof(s_colors)]{ nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

#if FALSE
        // Popups
        // https://task.ms/32440118: Add ContentIsland.SiteBridge to avoid this workaround
        winrt::IContentSiteBridge m_siteBridge{ nullptr };
#endif

        boolean m_ignoreLeftButtonPressed = false;
        boolean m_useSystemBackdrop = false;
        float m_prevRasterizationScale = 0;
        winrt::ContentLayoutDirection m_prevLayout = winrt::ContentLayoutDirection::LeftToRight;

        std::shared_ptr<TextRenderer> m_textRenderer = std::make_shared<TextRenderer>();

#if TRUE
        std::map<winrt::Visual, winrt::com_ptr<NodeSimpleFragment>> m_visualToFragmentMap;
#endif
    };
}

namespace winrt::DrawingIslandComponents::factory_implementation
{
    struct DrawingIsland :
        DrawingIslandT<DrawingIsland, implementation::DrawingIsland>
    {
    };
}
