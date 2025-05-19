---
page_type: sample
languages:
- csharp
products:
- windows
- windows-app-sdk
name: cs-wpf-sparse
description: Shows how to integrate the Windows Copilot Runtime APIs in a sparse WPF package
urlFragment: cs-wpf-sparse
extendedZipContent:
- path: LICENSE
  target: LICENSE
---

## Prerequisites
- For system requirements, see [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
- To ensure your development environment is set up correctly, see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
- Running this sample does require a [Windows Copilot + PC](https://learn.microsoft.com/windows/ai/npu-devices/)
- Running this sample also requires that the [Windows App SDK 1.8 Experimental2](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-18-experimental) framework package is installed on your Copilot+ PC.

### Suggested Environment

Use "Developer Command Prompt for VS2022" as your command prompt environment

## Build and Run the sample

This folder contains the cs-wpf-sparse sample which uses sparse package built on top of the WPF
platform using WCR. BuildSparsePackage.ps1 is provided for the user which closely follows the steps
from the link below

https://learn.microsoft.com/en-us/windows/apps/desktop/modernize/grant-identity-to-nonpackaged-apps#add-the-package-identity-metadata-to-your-desktop-application-manifest

BuildSparsePackage.ps1 does the following:
1) Build the solution in the desired platform and configuration provided in the parameters
 
2) Run MakeAppx with the /nv option on the folder containing the AppxManifest
    (cs-wpf-sparse\WCRforWPF\AppxManifest.xml) 
    
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

### Sparse Packaging

A sparse package allows unpackaged win32 apps to gain package identity without fully
adopting the MSIX packaging format. This is particularly useful for applications that are not yet
ready to have all their content inside an MSIX package but still need to use Windows extensibility
features that require package identity.

### Initialization code

Just doing the steps above does not yet grant the app package identity. The MSIX package still needs to
be registered with the external location. 

The RegisterSparsePackage() function in cs-wpf-sparse\WCRforWPF\App.xaml.cs does this for us. This
process is described in the documentation linked above. This will give the app process identity if
ran from the start menu. However, directly running the executable in the external location still
does not give the process package identity. This difference in behavior is not noted in the article
from above.

To allow the resulting app process from directly running the executable to gain identity requires
this workaround. 

The Startup() code has two different codepaths for whether the process is a packaged or a unpackaged
process (With or without identity).

If without Identity, RegisterSparsePackage() will be called which registers the package on the
external path if it hasn't been installed already. Afterwards, RunWithIdentity() will call
ActivateApplication on the package identity it registered earlier using the ActivationManager.

This will spawn a packaged process of itself which will have identity. The non-identity process will
then exit. The new packaged process in the startup will hit the codepath for when IsPackageProcess()
is true and launch MainWindow. 

Note: the first run of the exe run will require a longer startup time because of the time that is
needed to register the package. 

### Additional Notes

## Related Links
- [Windows Copilot Runtime Overview](https://learn.microsoft.com/windows/ai/apis/)
