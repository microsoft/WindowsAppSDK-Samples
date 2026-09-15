---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: WindowsAISample
description: Shows how to integrate the Windows AI APIs inside WinForms (unpackaged by default) with a packaging project
urlFragment: cs-winforms-pckg
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Running this sample does require a [Windows Copilot+ PC](https://learn.microsoft.com/windows/ai/npu-devices/)
- Running this sample also requires that the [Windows App SDK 1.8 Experimental2](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-18-experimental) framework package is installed on your Copilot+ PC.

## Build and Run the sample
1. Clone the repository onto your Copilot+ PC.
2. Open the solution file `WindowsAISample.sln` in Visual Studio 2022.
3. Change the Solution Platform to match the architecture of your Copilot+ PC.
4. Right-click on the solution in Solution Explorer and select "Build" to build solution.
5. Once the build is successful, right-click on the packaging project in Solution Explorer and select "Set as Startup Project".
6. Press F5 or select "Start Debugging" from the Debug menu to run the sample.
Note: The sample can also be run without debugging by selecting "Start Without Debugging" from the Debug menu or Ctrl+F5. 

## Sample Overview
The `MainForm` class in `MainForm.cs` is the main user interface for the Windows AI APIs Sample application. It demonstrates how to use the Windows AI APIs to perform text recognition and summarization on an image. The key functionalities include:

- **Select File**: Allows the user to select an image file from their file system and displays the selected image in a PictureBox.
- **Process Image**: Processes the selected image to extract text using Optical Character Recognition (OCR) and then summarizes the extracted text.

### Key Methods and Event Handlers

- **SelectFile_Click**: Opens a file dialog for the user to select an image file and displays the selected image.
- **ProcessButton_Click**: Handles the processing of the selected image, including loading AI models, performing text recognition, and summarizing the text.
- **LoadAIModels**: Loads the necessary AI models (`TextRecognizer` and `LanguageModel`) for text recognition and summarization.
- **PerformTextRecognition**: Uses the `TextRecognizer` to perform OCR on the selected image and extracts the text.
- **SummarizeImageText**: Uses the `LanguageModel` to generate a summary of the extracted text given a prompt.

## Related Links
- [Windows AI APIs Overview](https://learn.microsoft.com/windows/ai/apis/)
- [Package and deploy your WinForms app with MSIX](https://learn.microsoft.com/windows/apps/windows-app-sdk/migrate-to-windows-app-sdk/winforms-plus-winappsdk#package-and-deploy-your-winforms-app-with-msix)
