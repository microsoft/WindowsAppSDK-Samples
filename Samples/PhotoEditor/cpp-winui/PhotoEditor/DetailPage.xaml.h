// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "DetailPage.g.h"
#include <variant>

namespace winrt::PhotoEditor::implementation
{
    struct DetailPage : DetailPageT<DetailPage>, std::enable_shared_from_this<DetailPage>
    {
        DetailPage();

        // Retrieves Photo object.
        PhotoEditor::Photo Item() const
        {
            return m_item;
        }

        // Sets Photo object
        void Item(PhotoEditor::Photo const& value)
        {
            m_item = value;
        }

        // Resizes image to fit screen.
        void FitToScreen();

        // Resizes image to its actual size.
        void ShowActualSize();

        // Resizes image after tap.
        void UpdateZoomState();

        // Updates image brush based on set effect and its value.
        void UpdateEffectBrush(hstring const&);

        // Clears effects back to default.
        void ResetEffects();

        // Resets the color effects.
        void ResetColorEffects()
        {
            Item().Tint(0);
            Item().Temperature(0);
            Item().Saturation(1);
        }

        // Resets the light effects.
        void ResetLightEffects()
        {
            Item().Contrast(0);
            Item().Exposure(0);
        }

        // Resets the blur effects.
        void ResetBlurEffects()
        {
            Item().BlurAmount(0);
        }

        // Resets the sepia effects.
        void ResetSepiaEffects()
        {
            Item().Intensity(.5f);
        }

        // Event handler for navigation to DetailPage view.
        Windows::Foundation::IAsyncAction OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);

        // Prepares animation for navigation back to MainPage view.
        void OnNavigatingFrom(Microsoft::UI::Xaml::Navigation::NavigatingCancelEventArgs const&);

        // Event handlers
        void BackButton_ItemClick(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ZoomSlider_ValueChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::Primitives::RangeBaseValueChangedEventArgs const&);
        void MainImageScroller_ViewChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::ScrollViewerViewChangedEventArgs const&);
        void Effects_SelectionChanged(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Controls::SelectionChangedEventArgs const&);
        void EditButton_Check(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void EditButton_Uncheck(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void TextBlock_Tapped(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::Input::TappedRoutedEventArgs const&);
        void RemoveAllEffectsButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        Windows::Foundation::IAsyncAction SaveButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void SelectEffectsButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void ApplyEffectsButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);
        void CancelEffectsButton_Click(Windows::Foundation::IInspectable const&, Microsoft::UI::Xaml::RoutedEventArgs const&);

    private:
        // Initializes all image effects.
        void InitializeEffects();

        // Generate preview of effects for effect selection UI.
        void InitializeEffectPreviews();
        Windows::Foundation::IAsyncAction InitializeEffectPreview(Windows::Foundation::IInspectable, Microsoft::UI::Xaml::Controls::Image);

        // Creates the effects graph based on the selected effects.
        void CreateEffectsGraph();

        // Configure and generate resources for rendering.
        void UpdateMainImageBrush();
        void UpdatePanelState();
        void PrepareSelectedEffects();
        void ApplyEffects();
        void UpdateButtonImageBrush();

        // Backing field for Photo object.
        PhotoEditor::Photo m_item{ nullptr };
        
        // Indicates whether the effects controls are shown.
        bool m_showControls = false;

        event_revoker<Microsoft::UI::Xaml::Data::INotifyPropertyChanged> m_propertyChangedToken;

        // Field to store page Compositor for creation of types in the Windows.UI.Composition namespace.
        Microsoft::UI::Composition::Compositor m_compositor{ nullptr };

        // Fields for image effects, animation registration, and collection for effects graph.
        Microsoft::Graphics::Canvas::Effects::ContrastEffect m_contrastEffect{};
        Microsoft::Graphics::Canvas::Effects::ExposureEffect m_exposureEffect{};
        Microsoft::Graphics::Canvas::Effects::TemperatureAndTintEffect m_temperatureAndTintEffect{};
        Microsoft::Graphics::Canvas::Effects::GaussianBlurEffect m_blurEffect{};
        Microsoft::Graphics::Canvas::Effects::SaturationEffect m_saturationEffect{};
        Microsoft::Graphics::Canvas::Effects::SepiaEffect m_sepiaEffect{};
        Microsoft::Graphics::Canvas::Effects::GrayscaleEffect m_grayscaleEffect{};
        Microsoft::Graphics::Canvas::Effects::InvertEffect m_invertEffect{};
        Microsoft::Graphics::Canvas::Effects::CompositeEffect m_graphicsEffect{};
        Microsoft::UI::Composition::CompositionEffectBrush m_combinedBrush{ nullptr };

        std::vector<Windows::Foundation::IInspectable> m_selectedEffectsTemp{};
        std::vector<hstring> m_animatablePropertiesList{};

        // The effects do not inherit from a common interface that contracts the Source property, 
        // so we need to use a std::variant.
        std::vector<std::variant<Microsoft::Graphics::Canvas::Effects::ContrastEffect,
            Microsoft::Graphics::Canvas::Effects::ExposureEffect,
            Microsoft::Graphics::Canvas::Effects::TemperatureAndTintEffect,
            Microsoft::Graphics::Canvas::Effects::GaussianBlurEffect,
            Microsoft::Graphics::Canvas::Effects::SaturationEffect,
            Microsoft::Graphics::Canvas::Effects::SepiaEffect,
            Microsoft::Graphics::Canvas::Effects::GrayscaleEffect,
            Microsoft::Graphics::Canvas::Effects::InvertEffect,
            Microsoft::Graphics::Canvas::Effects::CompositeEffect>> m_effectsList{};

        // Photo image
        Microsoft::UI::Xaml::Media::Imaging::BitmapImage m_imageSource{ nullptr };

     };
}

namespace winrt::PhotoEditor::factory_implementation
{
    struct DetailPage : DetailPageT<DetailPage, implementation::DetailPage>
    {
    };
}