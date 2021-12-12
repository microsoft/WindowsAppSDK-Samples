// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once
#include "MainPage.g.h"

namespace winrt::PhotoEditor::implementation
{
    struct MainPage : MainPageT<MainPage>
    {
        MainPage();

        // Retreives collection of Photo objects for thumbnail view.
        Windows::Foundation::Collections::IVector<Windows::Foundation::IInspectable> Photos() const
        {
            return m_photos;
        }

        // Event handlers for loading and rendering images.
        Windows::Foundation::IAsyncAction OnNavigatedTo(Microsoft::UI::Xaml::Navigation::NavigationEventArgs);
        Windows::Foundation::IAsyncAction OnContainerContentChanging(Microsoft::UI::Xaml::Controls::ListViewBase, Microsoft::UI::Xaml::Controls::ContainerContentChangingEventArgs);

        // Animation for navigation back from DetailPage view.
        Windows::Foundation::IAsyncAction StartConnectedAnimationForBackNavigation();

        // Property changed notifications.
        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const&);
        void PropertyChanged(event_token const&);

        // Event handler.
        void ImageGridView_ItemClick(Windows::Foundation::IInspectable const, Microsoft::UI::Xaml::Controls::ItemClickEventArgs const);

    private:
        // Functions for image loading and animation.
        Windows::Foundation::IAsyncAction GetItemsAsync();
        Microsoft::UI::Composition::CompositionAnimationGroup CreateOffsetAnimation();
        Windows::Foundation::IAsyncOperation<PhotoEditor::Photo> LoadImageInfoAsync(Windows::Storage::StorageFile);

        // Backing field for Photo collection.
        Windows::Foundation::Collections::IVector<IInspectable> m_photos{ nullptr };

        // Field to store selected Photo for later back navigation.
        PhotoEditor::Photo m_persistedItem{ nullptr };

        // Collection of animations for element visuals for reorder animation.
        Microsoft::UI::Composition::ImplicitAnimationCollection m_elementImplicitAnimation{ nullptr };

        // Field to store page Compositor for creation of types in the Windows.UI.Composition namespace.
        Microsoft::UI::Composition::Compositor m_compositor{ nullptr };

        // Event
        event<Windows::Foundation::TypedEventHandler<Microsoft::UI::Xaml::Controls::ListViewBase, Microsoft::UI::Xaml::Controls::ContainerContentChangingEventArgs>> m_handler;

        // Property changed notifications.
        void RaisePropertyChanged(hstring const&);
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

    };
}

namespace winrt::PhotoEditor::factory_implementation
{
    struct MainPage : MainPageT<MainPage, implementation::MainPage>
    {
    };
}