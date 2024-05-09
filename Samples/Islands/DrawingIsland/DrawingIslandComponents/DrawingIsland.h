// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "DrawingIsland.g.h"
#include "IslandFragmentRoot.h"
#include "TextRenderer.h"
#include "DeviceLostHelper.h"
#include "TextItem.h"

namespace winrt::DrawingIslandComponents::implementation
{
    struct DrawingIsland :
        public DrawingIslandT<
        DrawingIsland,
        Microsoft::UI::Input::IInputPreTranslateKeyboardSourceHandler>,
        IAutomationCallbackHandler
    {
    public:
        DrawingIsland(
            const winrt::Compositor& compositor);

        ~DrawingIsland();

        // IClosable methods
        void Close();

        // Properties 
        // TODO: Enable Mica on Win 11
#if FALSE
        boolean UseSystemBackdrop()
        {
            return m_useSystemBackdrop;
        }

        void UseSystemBackdrop(
            boolean value);
#endif

        winrt::ContentIsland Island() const
        {
            return m_island;
        }

        // Methods

        void LeftClickAndRelease(
            const float2 currentPoint);

        void RightClickAndRelease(
            const float2 currentPoint);

        void SetBackroundOpacity(float backgroundOpacity)
        {
            m_background.Opacity = backgroundOpacity;
        }

        void SetColorIndex(std::uint32_t colorIndex)
        {
            m_output.CurrentColorIndex = std::clamp<std::uint32_t>(colorIndex, 0, _countof(s_colors) - 1);
            if (m_output.CurrentColorIndex >= 4)
            {
                m_background.BrushDefault =
                    m_output.Compositor.CreateColorBrush(s_colors[m_output.CurrentColorIndex]);

                m_background.Visual.Brush(m_background.BrushDefault);
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

        // IAutomationCallbackHandler overrides.
        winrt::Windows::Graphics::RectInt32 GetScreenBoundsForAutomationFragment(
            _In_::IUnknown const* const sender) const override;

        winrt::com_ptr<IRawElementProviderFragment> GetFragmentFromPoint(
            _In_ double x,
            _In_ double y) const override;

        winrt::com_ptr<IRawElementProviderFragment> GetFragmentInFocus() const override;

    private:
        Item* HitTestItem(float2 const& point) const;

        void Accessibility_Initialize();

        void Accessibility_CreateItemFragment(
            const winrt::Visual& itemVisual);

        void Accessibility_OnAutomationProviderRequested(
            const winrt::ContentIsland& island,
            const winrt::ContentIslandAutomationProviderRequestedEventArgs& args);

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

        void OnLeftClick(
            const float2 point,
            bool controlPressed);

        void OnRightClick(
            const float2 currentPoint);

        void Island_OnClosed();

        void Island_OnStateChanged();

        void OnDirect3DDeviceLost(
            DeviceLostHelper const* /* sender */,
            DeviceLostEventArgs const& /* args */);

        void Output_Initialize();

        void Output_AddVisual(
            const float2 point,
            bool halfTransparent);

        void Output_UpdateCurrentColorVisual();

        // TODO: Enable Mica on Win 11
#if FALSE
        void SystemBackdrop_Initialize();
#endif
        void SystemBackdrop_EvaluateUsage();

        void Environment_Initialize();

        void Environment_OnSettingChanged(
            const winrt::ContentEnvironmentSettingChangedEventArgs& args);

        void Environment_OnStateChanged(winrt::ContentIslandEnvironment const& sender);

        void Environment_SetLayoutDirection();

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

        // The underlying Island holding everything together.
        winrt::ContentIsland m_island{ nullptr };

        // Output
        struct
        {
            winrt::Compositor Compositor{ nullptr };

            std::shared_ptr<TextRenderer> TextRenderer;
            DeviceLostHelper DeviceLostHelper;

            // Current color used for new items
            unsigned int CurrentColorIndex = 0;

            // Cached brushes for items
            winrt::CompositionColorBrush ColorBrushes[_countof(s_colors)]
            { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

            winrt::CompositionColorBrush HalfTransparentColorBrushes[_countof(s_colors)]
            { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        } m_output;

        // Input handling
        struct
        {
            // Keyboard input handling
            winrt::InputKeyboardSource KeyboardSource{ nullptr };

            // Message loop integration
            winrt::InputPreTranslateKeyboardSource PretranslateSource{ nullptr };

            // Spatial input handling (mouse, pen, touch, etc.)
            winrt::InputPointerSource PointerSource{ nullptr };

            // Focus navigation handling
            winrt::InputFocusController FocusController{ nullptr };
        } m_input;

        // Background behind the items
        struct Background
        {
            winrt::CompositionColorBrush BrushDefault{ nullptr };
            winrt::CompositionColorBrush BrushA{ nullptr };
            winrt::CompositionColorBrush BrushB{ nullptr };
            winrt::CompositionColorBrush BrushC{ nullptr };
            winrt::SpriteVisual Visual{ nullptr };
            winrt::RectangleClip Clip{ nullptr };
            float Opacity = 0.5f;
        } m_background;

        // TODO: Enable Mica on Win 11
#if FALSE
        winrt::ContentExternalBackdropLink m_backdropLink{ nullptr };
        winrt::ICompositionSupportsSystemBackdrop m_backdropTarget{ nullptr };
        boolean m_useSystemBackdrop = false;
#endif

        // Drawing items being manipulated.
        struct 
        {
            // The container visual's Children collection. Each Item's visual must be
            // added to this collection to be rendered.
            winrt::VisualCollection Visuals{ nullptr };

            // Vector of Item objects representing items that can be manipulated, in
            // back-to-front order. Each item has an associated Visual.
            std::vector<std::unique_ptr<Item>> Items;

            // Pointer to the currently-selected item, if any.
            Item* SelectedItem{ nullptr };

            // Offset from the top-left corner of the selected item to the pointer.
            float2 Offset{};

            // Visual that shows the current color.
            winrt::SpriteVisual CurrentColorVisual{ nullptr };
        } m_items;

        struct
        {
            float RasterizationScale = 0;

            winrt::ContentLayoutDirection LayoutDirection =
                winrt::ContentLayoutDirection::LeftToRight;
        } m_prevState;
            
        struct AutomationPeer
        {
            explicit AutomationPeer(
                winrt::Visual const& visual,
                winrt::com_ptr<NodeSimpleFragment> const& automationProvider) :
                _visual{ visual },
                _automationProvider{ automationProvider }
            {

            }

            winrt::Visual const& GetVisual() const
            {
                return _visual;
            }

            winrt::com_ptr<NodeSimpleFragment> const& GetAutomationProvider() const
            {
                return _automationProvider;
            }

            bool Match(winrt::Visual const& visual) const noexcept
            {
                return visual == _visual;
            }

            bool Match(::IUnknown const* const automationProviderAsIUnknown) const noexcept
            {
                return automationProviderAsIUnknown == _automationProvider.try_as<::IUnknown>().get();
            }

        private:
            winrt::Visual _visual{ nullptr };
            winrt::com_ptr<NodeSimpleFragment> _automationProvider{ nullptr };
        };

        struct
        {
            // The UIA parent Root for this Island that contains the fragment children.
            winrt::com_ptr<IslandFragmentRoot> FragmentRoot{ nullptr };

            // Map a given square (Visual) to its UIA fragment object.
            std::vector<AutomationPeer> AutomationPeers{};
        } m_uia;
    };
}

namespace winrt::DrawingIslandComponents::factory_implementation
{
    struct DrawingIsland :
        DrawingIslandT<DrawingIsland, implementation::DrawingIsland>
    {
    };
}
