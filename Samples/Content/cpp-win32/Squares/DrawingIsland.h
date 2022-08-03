// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

namespace Squares
{
    class NodeSimpleFragment;
    class NodeSimpleFragmentFactory;
    class IslandFragmentRoot;

    struct DrawingIsland :
        public winrt::implements<DrawingIsland, winrt::IClosable>
    {
    public:
        DrawingIsland(
            const winrt::Compositor& compositor);

        ~DrawingIsland();

        // IClosable methods
        void Close();

        // DrawingIsland methods
        winrt::ContentIsland Island() const
        {
            return m_island;
        }

    private:
        void Output_Initialize();

        void Output_AddVisual(
            const winrt::float2 point,
            bool halfTransparent);

        void Input_Initialize();

        bool Input_OnKeyDown(
            winrt::Windows::System::VirtualKey virtualKey);

        void Input_OnLeftButtonPressed(
            const winrt::PointerEventArgs& args);

        void Input_OnPointerMoved(
            const winrt::PointerEventArgs& args);

        void Input_OnPointerReleased();

        void OnLeftClick(
            const winrt::float2 point,
            bool controlPressed);

        void Accessibility_Initialize();

        void Accessibility_OnAutomationProviderRequested(
            const winrt::ContentIsland& island,
            const winrt::ContentIslandAutomationProviderRequestedEventArgs& args);

        void Accessibility_UpdateScreenCoordinates(
            winrt::Visual visual);

        void CreateUIAProviderForVisual();

        void Window_Initialize();

        void Window_StateChanged();

        winrt::Visual HitTestVisual(
            winrt::float2 const point);

    private:
        static inline winrt::Color s_colors[] =
        {
            { 0xFF, 0x5B, 0x9B, 0xD5 },
            { 0xFF, 0xED, 0x7D, 0x31 },
            { 0xFF, 0x70, 0xAD, 0x47 },
            { 0xFF, 0xFF, 0xC0, 0x00 }
        };

        static inline std::wstring s_colorNames[] =
        {
            L"Blue",
            L"Orange",
            L"Green",
            L"Yellow"
        };

        winrt::com_ptr<NodeSimpleFragmentFactory> m_fragmentFactory{ nullptr };
        winrt::com_ptr<IslandFragmentRoot> m_fragmentRoot{ nullptr };
        winrt::Compositor m_compositor{ nullptr };
        winrt::ContentIsland m_island{ nullptr };
        winrt::InputKeyboardSource m_keyboardSource{ nullptr };

        // Background
        winrt::CompositionColorBrush m_backgroundBrushDefault { nullptr };
        winrt::CompositionColorBrush m_backgroundBrushA { nullptr };
        winrt::CompositionColorBrush m_backgroundBrushB { nullptr };
        winrt::CompositionColorBrush m_backgroundBrushC { nullptr };
        winrt::SpriteVisual m_backgroundVisual { nullptr };

        // Drawing squares
        winrt::VisualCollection m_visuals{ nullptr };
        winrt::Visual m_selectedVisual{ nullptr };
        winrt::SpriteVisual m_currentColorVisual{ nullptr };
        winrt::float2 m_offset{};
        
        unsigned int m_currentColorIndex = 0;
        winrt::CompositionColorBrush m_colorBrushes[_countof(s_colors)]{ nullptr, nullptr, nullptr, nullptr };
        winrt::CompositionColorBrush m_halfTransparentColorBrushes[_countof(s_colors)]{ nullptr, nullptr, nullptr, nullptr };

        std::map<winrt::Visual, winrt::com_ptr<NodeSimpleFragment>> m_visualToFragmentMap;
    };
}
