---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: cs-wpf-dotnet
description: Shows how "dotnet build" command can be used to integrate the Windows Copilot Runtime APIs in a SingleProject MSIX package
urlFragment: cs-wpf-dotnet
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Running this sample does require a [Windows Copilot + PC](https://learn.microsoft.com/windows/ai/npu-devices/)
- This sample requires [Windows App SDK 1.7 Experimental3](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-17-experimental), which should be installed by Visual Studio when deploying or launching the app.
- This sample requires .NET and .NET CLI [Install .NET on Windows](https://learn.microsoft.com/en-us/dotnet/core/install/windows)

### Suggested Environment

Use "Developer Command Prompt for VS2022" as your command prompt environment

## Build and Run the sample
In the command line, cd to the root of the solution and run
```
runDotNetBuild.cmd or runDotNetBuild.ps1
```

runDotNetBuild.ps1 will generate the MSIX with a self-signed cert which you will need to install to
LocalMachine TrustedPeople Certificate Store.

It will then run dotnet build on 

## Sample Overview

The `MainForm` class in `MainForm.cs` is the main user interface for the Windows Copilot Runtime Sample application. It demonstrates how to use the Windows Copilot Runtime API to perform text recognition and summarization on an image. The key functionalities include:

- **Select File**: Allows the user to select an image file from their file system and displays the selected image in a PictureBox.
- **Process Image**: Processes the selected image to extract text using Optical Character Recognition (OCR) and then summarizes the extracted text.

### Key Methods and Event Handlers

- **SelectFile_Click**: Opens a file dialog for the user to select an image file and displays the selected image.
- **ProcessButton_Click**: Handles the processing of the selected image, including loading AI models, performing text recognition, and summarizing the text.
- **LoadAIModels**: Loads the necessary AI models (`TextRecognizer` and `LanguageModel`) for text recognition and summarization.
- **PerformTextRecognition**: Uses the `TextRecognizer` to perform OCR on the selected image and extracts the text.
- **SummarizeImageText**: Uses the `LanguageModel` to generate a summary of the extracted text given a prompt.

### WAPProj Conversion to SingleProjectMSIX

Because .NET CLI is incompatible with WAPProj, we must first convert the WAPProj to a
SingleProjectMSIX This project was converted from a WAPProj to SingleProjectMSIX using the steps
below https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/single-project-msix?tabs=csharp

For the original WCRforWPF project with warpproj, please see:
Samples\WindowsCopilotRuntime\cs-wpf

Summary of the steps done:
- Add EnableMsixTooling and PublishProfile properties to WCRforWPF.csproj
    ```
    <EnableMsixTooling>true</EnableMsixTooling>
    <PublishProfile>Properties\PublishProfiles\win10-$(Platform).pubxml</PublishProfile>
    <GenerateAppxPackageOnBuild>true</GenerateAppxPackageOnBuild>
    ```

- Add WCRforWPF\Properties\launchSettings.json
  This configures on how an application starts during development
  
- Add /Properties/PublishProfiles/win10-$(platform).pubxml 
  This defines the publishing configuration for each platform is necessary for each platform you
  want to publish and deploy on. 

- Move Package.appxmanifest and files under images from the WAPProj to WCRforWPF.csproj and remove
  the WAPProj.

- In configuration manager, check the deploy box in each of the configuration/platform you want to
  deploy on

### Additional Notes

## Related Links
- [Windows Copilot Runtime Overview](https://learn.microsoft.com/windows/ai/apis/)
- [single-project-msix]https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/single-project-msix?tabs=csharp
- [Install .NET on Windows](https://learn.microsoft.com/en-us/dotnet/core/install/windows)