// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#include "Photo.g.h"

namespace winrt::PhotoEditor::implementation
{
    struct Photo : PhotoT<Photo>
    {
        Photo() = default;

        Photo(Windows::Storage::FileProperties::ImageProperties const& props,
            Windows::Storage::StorageFile const& imageFile,
            hstring const& name,
            hstring const& type) :
            m_imageProperties(props),
            m_imageName(name),
            m_imageFileType(type),
            m_imageFile(imageFile)
        {
            if (m_imageProperties.Title() != L"")
            {
                m_imageName = m_imageProperties.Title();
            }
        }

        // Gets the thumbnail of current image file (m_imageFile).
        Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Media::Imaging::BitmapImage> GetImageThumbnailAsync() const;

        // Gets the full image of the current image file (m_imageFile).
        Windows::Foundation::IAsyncOperation<Microsoft::UI::Xaml::Media::Imaging::BitmapImage> GetImageSourceAsync() const;

        // File and information properties.
        Windows::Storage::StorageFile ImageFile() const
        {
            return m_imageFile;
        }

        Windows::Storage::FileProperties::ImageProperties ImageProperties() const
        {
            return m_imageProperties;
        }

        hstring ImageName() const
        {
            return m_imageName;
        }

        hstring ImageFileType() const
        {
            return m_imageFileType;
        }

        // Gets or sets the image title.
        hstring ImageTitle() const
        {
            return m_imageName;
        }

        void ImageTitle(hstring const& value);

        hstring ImageDimensions() const;

        // Exposure property for light effect.
        float Exposure() const
        {
            return m_exposure;
        }

        void Exposure(float value)
        {
            UpdateValue(L"Exposure", m_exposure, value);
        }

        // Temperature property for color effect.
        float Temperature() const
        {
            return m_temperature;
        }

        void Temperature(float value)
        {
            UpdateValue(L"Temperature", m_temperature, value);
        }

        // Tint property for tint and contrast effect.
        float Tint() const
        {
            return m_tint;
        }

        void Tint(float value)
        {
            UpdateValue(L"Tint", m_tint, value);
        }

        // Contrast property for tin and contrast effect.
        float Contrast() const
        {
            return m_contrast;
        }

        void Contrast(float value)
        {
            UpdateValue(L"Contrast", m_contrast, value);
        }

        // Satruration property for saturation effect.
        float Saturation() const
        {
            return m_saturation;
        }

        void Saturation(float value)
        {
            UpdateValue(L"Saturation", m_saturation, value);
        }

        // Blur amount for Gaussian blur effect.
        float BlurAmount() const
        {
            return m_blur;
        }

        void BlurAmount(float value)
        {
            UpdateValue<float>(L"BlurAmount", m_blur, value);
        }

        // Intensity for sepia effect.
        float Intensity() const
        {
            return m_sepiaIntensity;
        }

        void Intensity(float value)
        {
            UpdateValue<float>(L"Intensity", m_sepiaIntensity, value);
        }

        // Property changed notifications.
        event_token PropertyChanged(Microsoft::UI::Xaml::Data::PropertyChangedEventHandler const& value)
        {
            return m_propertyChanged.add(value);
        }

        void PropertyChanged(event_token const& token)
        {
            m_propertyChanged.remove(token);
        }

    private:
        // File and information fields.
        Windows::Storage::FileProperties::ImageProperties m_imageProperties{ nullptr };
        Windows::Storage::StorageFile m_imageFile{ nullptr };
        hstring m_imageName;
        hstring m_imageFileType;
        hstring m_imageTitle;

        // Fields for image effects.
        float m_exposure{ 0 };
        float m_temperature{ 0 };
        float m_tint{ 0 };
        float m_contrast{ 0 };
        float m_saturation{ 1 };
        float m_blur{ 0 };
        float m_sepiaIntensity{ .5f };

        // Size field for image tile size on MainPage.
        double m_size{ 250 };

        // Property changed notification.
        event<Microsoft::UI::Xaml::Data::PropertyChangedEventHandler> m_propertyChanged;

        template <class T>
        void UpdateValue(hstring const& propertyName, T & var, T value)
        {
            if (var != value)
            {
                var = value;
                RaisePropertyChanged(propertyName);
            }
        }

        void RaisePropertyChanged(hstring const& propertyName)
        {
            m_propertyChanged(*this, Microsoft::UI::Xaml::Data::PropertyChangedEventArgs(propertyName));
        }
    };
}

namespace winrt::PhotoEditor::factory_implementation
{
    struct Photo : PhotoT<Photo, implementation::Photo>
    {
    };
}