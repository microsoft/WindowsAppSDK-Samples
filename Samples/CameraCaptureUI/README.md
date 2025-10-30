---
page_type: sample  
languages:  
- csharp  
- cppwinrt  
- cpp  
products:  
- windows  
- windows-app-sdk  
name: "CameraCaptureUI Sample"  
urlFragment: CameraCaptureUISample  
description: "Demonstrates how to capture photos and videos using the CameraCaptureUI API in WinAppSDK, showcasing how to integrate camera functionality into desktop apps."  
extendedZipContent:  
- path: LICENSE  
  target: LICENSE  
---  


# Camera Capture UI Sample Application

This sample demonstrates how to capture photos and videos using the **CameraCaptureUI** API in a **WinUI3** app. It shows how to use the CameraCaptureUI API to launch the camera for photo and video captures within a desktop application. The sample highlights setting custom camera capture options such as format and resolution, handling successful captures, and displaying the captured media.

> **Note**: This sample is targeted and tested for the **Windows App SDK 1.7 Experimental 1** and **Visual Studio 2022**.


## Features

This sample demonstrates:

- **Photo Capture**: Capturing images with optional settings, including format and resolution.
- **Video Capture**: Capturing videos, with options settings for format and maximum resolution.
- **BitmapImage Display**: Displaying captured photos using `BitmapImage` in XAML.
- **Media Playback**: Playing back captured videos using `MediaPlayerElement`.



## Prerequisites


* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
* The C# sample requires Visual Studio 2022 and .NET 6.

## Building and Running the Sample

* Open the solution file (`.sln`) in Visual Studio.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.


## Related Documentation and Code Samples
* [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
* [CameraCaptureUI API Spec Documentation](https://github.com/microsoft/WindowsAppSDK/blob/main/specs/CameraCaptureUI/CameraCaptureUI.md)
* [Windows Media Playback API](https://learn.microsoft.com/en-us/uwp/api/windows.ui.xaml.controls.mediaplayerelement?view=winrt-26100)
* [BitmapImage Class Documentation](https://learn.microsoft.com/en-us/dotnet/api/system.windows.media.imaging.bitmapimage?view=windowsdesktop-8.0)



