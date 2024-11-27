// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Scenario2_CaptureVideo.xaml.h"
#if __has_include("Scenario2_CaptureVideo.g.cpp")
#include "Scenario2_CaptureVideo.g.cpp"
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
    MainPage Scenario2_CaptureVideo::rootPage{ nullptr };

    Scenario2_CaptureVideo::Scenario2_CaptureVideo()
    {
        InitializeComponent();
        Scenario2_CaptureVideo::rootPage = MainPage::Current();
    }
    winrt::fire_and_forget Scenario2_CaptureVideo::CaptureVideo_Click(IInspectable const&, RoutedEventArgs const&)
    {
        auto parentWindow = ::GetForegroundWindow();
        winrt::Microsoft::UI::WindowId windowId{ reinterpret_cast<uint64_t>(parentWindow) };
        winrt::Microsoft::Windows::Media::Capture::CameraCaptureUI cameraUI(windowId);

        // Optionally, set video settings such as format, max resolution, trimming, etc.
        cameraUI.VideoSettings().Format(winrt::Microsoft::Windows::Media::Capture::CameraCaptureUIVideoFormat::Mp4);

        // -- Optional Parameters --
        //cameraUI.VideoSettings().MaxDurationInSeconds(30);
        //cameraUI.VideoSettings().MaxResolution(winrt::Microsoft::Windows::Media::Capture::CameraCaptureUIMaxVideoResolution::HighestAvailable);
        //cameraUI.VideoSettings().AllowTrimming(true);

        // -- Notes --
        //By default, AllowTrimming is true.
        //MaxDurationInSeconds must be a valid value(i.e., the duration should be in the range of 0 to UINT32_MAX).Specifying an invalid value will result in an invalid argument exception.
        //MaxDurationInSeconds cannot be set if AllowTrimming is false.Attempting to do so will result in an invalid argument exception.

        auto file = co_await cameraUI.CaptureFileAsync(winrt::Microsoft::Windows::Media::Capture::CameraCaptureUIMode::Video);

        // Check if a file was captured successfully
        if (file)
        {
            rootPage.NotifyUser(L"Video successfully captured!", InfoBarSeverity::Success);
            //// Get the file path of the captured image
            winrt::hstring filePath = file.Path();
            // Create a stream for the captured file
            auto fileStream = co_await file.OpenAsync(winrt::Windows::Storage::FileAccessMode::Read);
            winrt::Windows::Media::Core::MediaSource mediaSource = winrt::Windows::Media::Core::MediaSource::CreateFromStream(fileStream, file.ContentType());
            // Create a MediaPlayer
            Windows::Media::Playback::MediaPlayer mediaPlayer = Windows::Media::Playback::MediaPlayer();
            // Set the source for the MediaPlayer
            mediaPlayer.Source(mediaSource);
            // Set the MediaPlayer to the MediaPlayerElement
            CapturedVideo().SetMediaPlayer(mediaPlayer);
            // Start playing the video
            mediaPlayer.Play();
        }
        else
        {
            // If the capture failed, update the button content to "Photo Capture Failed"
            rootPage.NotifyUser(L"Video Capture Failed", InfoBarSeverity::Error);
        }
    }

    void Scenario2_CaptureVideo::ClearMessage_Click(IInspectable const&, RoutedEventArgs const&)
    {
        rootPage.NotifyUser(L"", InfoBarSeverity::Informational);
    }
}
