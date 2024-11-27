#include "pch.h"
#include "Scenario1_CapturePhoto.xaml.h"
#if __has_include("Scenario1_CapturePhoto.g.cpp")
#include "Scenario1_CapturePhoto.g.cpp"
#endif

namespace winrt
{
    using namespace Microsoft::UI::Xaml;
    using namespace Microsoft::UI::Xaml::Controls;
    using namespace Windows::Foundation;
    using namespace Windows::Storage;
    using namespace Windows::Storage::Streams;
}

namespace winrt::cpp_winui::implementation
{
    MainPage Scenario1_CapturePhoto::rootPage{ nullptr };

    Scenario1_CapturePhoto::Scenario1_CapturePhoto()
    {
        InitializeComponent();
        Scenario1_CapturePhoto::rootPage = MainPage::Current();
    }

    winrt::fire_and_forget Scenario1_CapturePhoto::CapturePhoto_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto parentWindow = ::GetForegroundWindow();
        winrt::Microsoft::UI::WindowId windowId{ reinterpret_cast<uint64_t>(parentWindow) };
        winrt::Microsoft::Windows::Media::Capture::CameraCaptureUI cameraUI(windowId);

        // Optionally, set photo settings such as format, aspect ratio, etc.
        cameraUI.PhotoSettings().Format(winrt::Microsoft::Windows::Media::Capture::CameraCaptureUIPhotoFormat::Jpeg);

        // -- Optional Parameters --
        //cameraUI.PhotoSettings().MaxResolution(winrt::Microsoft::Windows::Media::Capture::CameraCaptureUIMaxPhotoResolution::HighestAvailable);
        //cameraUI.PhotoSettings().CroppedAspectRatio(winrt::Windows::Foundation::Size(4, 3));
        //cameraUI.PhotoSettings().CroppedSizeInPixels(winrt::Windows::Foundation::Size(200, 200));

        // -- Notes --
        // By default, AllowCropping is true.
        // Don't specify both size and aspect ratio in the PhotoSettings, doing so will cause an invalid argument exception.
        // PhotoSettings can't have a ratio or size specified with cropping disabled. Attempting to do so will result in an invalid argument exception.
        // Start the photo capture operation

        auto file = co_await cameraUI.CaptureFileAsync(winrt::Microsoft::Windows::Media::Capture::CameraCaptureUIMode::Photo);

        // Check if a file was captured successfully
        if (file)
        {
            rootPage.NotifyUser(L"Photo successfully captured!", InfoBarSeverity::Success);
            // Get the file path of the captured image
            winrt::hstring filePath = file.Path();
            // Create a Uri from the file path
            winrt::Uri fileUri(filePath);
            // Open the file and display it in the Image control
            auto fileStream = co_await file.OpenAsync(winrt::Windows::Storage::FileAccessMode::Read);
            // Create a BitmapImage and set its source to the captured file's stream
            winrt::Microsoft::UI::Xaml::Media::Imaging::BitmapImage bitmapImage;
            bitmapImage.UriSource(fileUri);
            CapturedImage().Source(bitmapImage);
        }
        else
        {
            // If the capture failed, update the button content to "Photo Capture Failed"
            rootPage.NotifyUser(L"Photo Capture Failed", InfoBarSeverity::Error);
        }
    }

    void Scenario1_CapturePhoto::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", InfoBarSeverity::Informational);
    }
}
