---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: MauiWindowsCopilotRuntimeSample
description: Shows how to integrate the Windows Copilot Runtime APIs inside MAUI
urlFragment: cs-maui
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Running this sample does require a [Windows Copilot + PC](https://learn.microsoft.com/windows/ai/npu-devices/)
- This sample requires [Windows App SDK 1.7 Experimental3](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-17-experimental), which should be installed by Visual Studio when deploying or launching the app.

## Build and Run the sample
1. Clone the repository onto your Copilot+PC.
2. Open the solution file `MauiWindowsCopilotRuntimeSample.sln` in Visual Studio 2022.
3. Ensure the debug toolbar has "Windows Machine" set as the target device.
4. Press F5 or select "Start Debugging" from the Debug menu to run the sample.
Note: The sample can also be run without debugging by selecting "Start Without Debugging" from the Debug menu or Ctrl+F5. 

## Sample Overview

This sample demonstrates use of some Windows Copilot Runtime APIs, including `LanguageModel` for text
generation and `ImageScaler` for image super resolution to scale and sharpen images. Click one of the
"Scale" buttons to scale the image (or reshow the original, unscaled image), or enter a text prompt and
click the "Generate" button to generate a text response.

The changes from the ".NET MAUI App" template are split across four files:
- `MauiWindowsCopilotRuntimeSample.csproj`: Adds the required Windows App SDK package reference for the
  Windows Copilot Runtime APIs. This reference needs to be conditioned only when building for Windows
  (see [Additional Notes](#additional-notes) below for details). This file also sets the necessary
  TargetFramework for Windows.
- `Platforms/Windows/MainPage.cs`: Implements partial methods from the shared `MainPage` class to show and
  handle the text generation and image scaling functionality.
- `MainPage.xaml`: Defines controls to show text generation and image scaling.
- `MainPage.xaml.cs`: Defines partial methods which the Windows-specific `MainPage.cs` implements.

### Additional Notes

- To make the Windows App SDK package reference conditioned for only Windows build, it can be added to an
  `ItemGroup` with the right Condition, like this:
```xml
	<ItemGroup Condition="$([MSBuild]::GetTargetPlatformIdentifier('$(TargetFramework)')) == 'windows'">
		<PackageReference Include="Microsoft.WindowsAppSDK" Version="1.7.250127003-experimental3" />
	</ItemGroup>
```
- To access the Windows Copilot Runtime APIs, the project currently must set the TargetFramework for
  Windows to target 22621 or later.
- The folder paths were shortened to `MauiWCRSample` to avoid MAX_PATH issues, particularly when
  building the Android version.

## Related Links
- [Windows Copilot Runtime Overview](https://learn.microsoft.com/windows/ai/apis/)
