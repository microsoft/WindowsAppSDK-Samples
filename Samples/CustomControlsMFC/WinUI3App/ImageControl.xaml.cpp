#include "pch.h"
#include "ImageControl.xaml.h"
#if __has_include("ImageControl.g.cpp")
#include "ImageControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
    ImageControl::ImageControl()
    {
        InitializeComponent();
    }

    int32_t ImageControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void ImageControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void ImageControl::imageLink(const winrt::hstring& str)
    {
      //TODO
      /*Windows::UI::Xaml::Controls::Image img;
      Windows::UI::Xaml::Media::Imaging::BitmapImage bitmapImage;
      Windows::Foundation::Uri uri{ str };
      bitmapImage.UriSource(uri);
      img.Source(bitmapImage);*/

    }

    winrt::hstring ImageControl::imageLink()
    {
      return ImageWidget().Source().as<winrt::hstring>();
    }

}
