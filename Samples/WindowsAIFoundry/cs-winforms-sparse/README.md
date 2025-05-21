---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: cs-winforms-sparse
description: Shows how to integrate the Windows AI APIs inside WinForms (with package identity)
urlFragment: cs-winforms-sparse
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Running this sample does require a [Windows Copilot+ PC](https://learn.microsoft.com/windows/ai/npu-devices/)
- Running this sample also requires that the [Windows App SDK 1.8 Experimental2](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-18-experimental) framework package is installed on your Copilot+ PC.

### Suggested Environment

Use "Developer Command Prompt for VS2022" as your command prompt environment

## Build and Run the sample

This folder contains the `cs-winforms-sparse` sample, which demonstrates the use of a sparse package for a WinForms application using Windows AI APIs. A sparse package enables unpackaged Win32 applications to
utilize package identity and access APIs requiring package identity without converting to a full MSIX package.


BuildSparsePackage.ps1 is provided for the user which closely follows the steps from the link below

https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps#add-the-package-identity-metadata-to-your-desktop-application-manifest

BuildSparsePackage.ps1 does the following:
1) Build the solution in the desired platform and configuration provided in the parameters
 
2) Run MakeAppx with the /nv option on the folder containing the AppxManifest
    (cs-winforms-sparse\WinForms\AppxManifest.xml) 
    
    - The /nv flag is required to bypass validation of referenced file paths in the manifest. 
    
    - The output folder is set to the output of the binaries when we build the solution regularly
    (bin\$Platform\$Configuration\net8.0-windows10.0.22621.0) so that the MSIX will be side by side
    to the executable built in step 1. This placement is necessary because the exe expects the external location of
    the MSIX to be the same as it's file location.

3) Run SignTool to sign the MSIX

Please have the WindowsAppSDK runtime installed using the installer for 1.7.250127003-experimental3
at https://learn.microsoft.com/en-us/windows/apps/windows-app-sdk/downloads

Please install the cert in the .user folder to LocalMachine TrustedPeople root. You can either
launch from the start menu after installing the MSIX in the bin directory or you can directly run
the exe. 

Note: The first run of the exe will require a longer startup time because of the time that is
needed to register the package. Upon launch, the package information will be visible before the main
form is shown.

## Sample Overview
`Program.cs` serves as the application's entry point. Its primary responsibilities include registering the sparse package to provide the executable with package identity and activating the application.
It also ensures that the executable without package identity does not run concurrently.

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
- [WPF Sparse Package Sample (with Windows AI APIs)](https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/experimental/Samples/WindowsAIFoundry/cs-wpf-sparse)

