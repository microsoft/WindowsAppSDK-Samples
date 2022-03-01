// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#include "pch.h"
#include "Photo.h"
#include "Photo.g.cpp"
#include <sstream>

using namespace std;
namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Media::Imaging;
    using namespace Windows::Foundation;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Streams;
}

namespace winrt::PhotoEditor::implementation
{
    IAsyncOperation<BitmapImage> Photo::GetImageThumbnailAsync() const
    {
        auto thumbnail = co_await m_imageFile.GetThumbnailAsync(FileProperties::ThumbnailMode::PicturesView);
        BitmapImage bitmapImage{};
        bitmapImage.SetSource(thumbnail);
        thumbnail.Close();
        co_return bitmapImage;
    }

    IAsyncOperation<BitmapImage> Photo::GetImageSourceAsync() const
    {
        IRandomAccessStream stream{ co_await ImageFile().OpenAsync(FileAccessMode::Read) };
        BitmapImage bitmap{};
        bitmap.SetSource(stream);
        co_return bitmap;
    }

    hstring Photo::ImageDimensions() const
    {
        wstringstream stringStream;
        stringStream << m_imageProperties.Width() << " x " << m_imageProperties.Height();
        wstring str = stringStream.str();
        return static_cast<hstring>(str);
    }

    void Photo::ImageTitle(hstring const& value)
    {
        if (m_imageProperties.Title() != value)
        {
            m_imageProperties.Title(value);
            auto ignoreResult = m_imageProperties.SavePropertiesAsync();
            RaisePropertyChanged(L"ImageTitle");
        }
    }
}
