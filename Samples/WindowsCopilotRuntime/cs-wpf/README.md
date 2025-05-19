---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: cs-wpf
description: Windows Copilot Runtime APIs in a WPF with Wapproj MSIX package
urlFragment: cs-wpf
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Running this sample does require a [Windows Copilot + PC](https://learn.microsoft.com/windows/ai/npu-devices/)
- Running this sample also requires that the [Windows App SDK 1.8 Experimental2](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-18-experimental) framework package is installed on your Copilot+ PC.
- This sample requires .NET and .NET CLI [Install .NET on Windows](https://learn.microsoft.com/en-us/dotnet/core/install/windows)

## Sample Overview

The `MainWindow` class in `MainWindow.xaml.cs` is the main user interface for the Windows Copilot Runtime Sample application. It demonstrates how to use the Windows Copilot Runtime API to perform text recognition and summarization on an image. The key functionalities include:

- **Select File**: Allows the user to select an image file from their file system and displays the selected image in a PictureBox.
- **Process Image**: Processes the selected image to extract text using Optical Character Recognition (OCR) and then summarizes the extracted text.

### Key Methods and Event Handlers

- **SelectFile_Click**: Opens a file dialog for the user to select an image file and displays the selected image.
- **ProcessButton_Click**: Handles the processing of the selected image, including loading AI models, performing text recognition, and summarizing the text.
- **LoadAIModels**: Loads the necessary AI models (`TextRecognizer` and `LanguageModel`) for text recognition and summarization.
- **PerformTextRecognition**: Uses the `TextRecognizer` to perform OCR on the selected image and extracts the text.
- **SummarizeImageText**: Uses the `LanguageModel` to generate a summary of the extracted text given a prompt.

### Additional Notes

Running this sample successfully requires identity. 

You must generate an App Package following the steps outlined in
[Generate an app package upload file for Store submission](https://learn.microsoft.com/en-us/windows/msix/package/packaging-uwp-apps)

## Related Links
- [Windows Copilot Runtime Overview](https://learn.microsoft.com/windows/ai/apis/)
- [single-project-msix]https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/single-project-msix?tabs=csharp
- [Install .NET on Windows](https://learn.microsoft.com/en-us/dotnet/core/install/windows)