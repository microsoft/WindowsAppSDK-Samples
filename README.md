# Windows App SDK Samples

This repository hosts samples for the [Windows App SDK](https://github.com/microsoft/WindowsAppSDK). Samples for various features shipping in the Windows App SDK will be added to this repository. For more information about the Windows App SDK, visit the [Windows App SDK Documentation](https://docs.microsoft.com/windows/apps/windows-app-sdk/). To learn more about the Windows App SDK design or to contribute to the project, make feature proposals, or start discussions, visit the [Windows App SDK GitHub page](https://github.com/microsoft/WindowsAppSDK).

## List of samples

#### App Lifecycle and System Services
- [App Lifecycle](Samples/AppLifecycle): These samples demonstrate app instancing using the AppLifecycle APIs. They include the following features:
    - [Activation](Samples/AppLifecycle/Activation): These samples demonstrate support for rich activation kinds.
    - [Instancing](Samples/AppLifecycle/Instancing): These samples demonstrate support for single and selective multi-instancing.
    - [Power Notifications](Samples/AppLifecycle/StateNotifications): These samples demonstrate the power/system state notifications for managing app workload.
    - [Restart](Samples/AppLifecycle/Restart): These samples demonstrate synchronously restarting an app with command-line restart arguments.

#### Data and Files
- [Resource Management](Samples/ResourceManagement): These samples demonstrates app resource management using the MRT Core APIs.

#### Deployment
- [Deployment Manager](Samples/DeploymentManager): This sample demonstrates how to initialize and access deployment information for the Windows App SDK runtime.
- [Installer](Samples/Installer): This sample demonstrates how to launch the Windows App SDK installer without a console window.
- [Unpackaged](Samples/Unpackaged): These samples demonstrate auto-initialization with the Windows App SDK package from non-MSIX (unpackaged) application.

#### Dynamic Dependencies
- [DirectX](DynamicDependenciesSample/DynamicDependencies): This sample demonstrates how to use the Dynamic Dependencies API to dynamically load the DirectX Runtime MSIX framework package.

#### Graphics
- [Text Rendering](Samples/TextRendering): This sample is a gallery of the DWriteCore APIs demonstrating text rendering.

#### Notifications
- [Push Notifications](Samples/Notifications/Push): This is a sample app that showcases Push Notifications.
- [App Notifications](Samples/Notifications/App): This is a sample app that showcases App Notifications.

#### Runtime Components
- [Custom Controls](Samples/CustomControls): This sample shows how to author a Windows Runtime Component in C# with custom WinUI controls.

#### User Interface and Input
- [Windowing](Samples/Windowing): This sample demonstrates how to manage app windows using the Windowing APIs.
- [Windows Input and Composition Gallery](https://github.com/microsoft/WindowsCompositionSamples): This collection of samples showcases Microsoft.UI.Composition and Microsoft.UI.Input APIs.
- [XAML Controls Gallery](https://github.com/microsoft/Xaml-Controls-Gallery/tree/winui3): This is a sample app that showcases all of the WinUI 3 controls in action.

## Requirements

Thes Windows App SDK samples have the following system requirements:

- Windows 10, version 1809 (build 17763) or later.

- [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/) or Visual Studio 2019 version 16.9 or later, with the following workloads and components:

    - Universal Windows Platform development
    - .NET Desktop Development (needed even if you're only building C++ Win32 apps)
    - Desktop development with C++ (needed even if you're only building .NET apps)
    - Windows SDK version 2004 (build 19041) or later. This is installed with Visual Studio by default.

- Building .NET C# samples with Windows App SDK 1.1 also requires one of the following .NET SDK versions or later:
    - 6.0.202
    - 6.0.104
    - 5.0.407
    - 5.0.213

Refer to the docs on [system requirements](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements) and [tools for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment) for a detalied list of requirements for developing apps with the Windows App SDK.

## Using the samples

To use the samples with Git, clone the WindowsAppSDK-Samples repository by running `git clone https://github.com/microsoft/WindowsAppSDK-Samples.git` from a command prompt. You can then navigate to the directory of a sample and open the solution file in Visual Studio.

The easiest way to use these samples without Git is to download the ZIP file. Select *Download ZIP* from the download dropdown, unzip the entire archive and open the samples in Visual Studio.

### Samples versioning

The samples will be updated with GA releases of the Windows App SDK. The `main` branch includes the latest samples, which may also include experimental or preview features. Specific release branches are named `release/x.y` and contain samples that reference the corresponding GA version of the Windows App SDK. This repository currently includes the following release branches:

- [release/1.0](https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/1.0)
- [release/0.8](https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/0.8)
- [release/0.5](https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/0.5)

## Contributing

These samples are provided by feature teams and we welcome your input on issues and suggestions for new samples. We encourage you to [file a new issue](https://github.com/microsoft/WindowsAppSDK-Samples/issues/new) for any feedback or questions!

Sample authors should follow the [samples guidelines](docs/samples-guidelines.md) to begin developing their samples. For WinUI-based Windows App SDK samples, use the [sample templates](Templates/README.md). The VSIX file is available for download in the Github releases page [here](https://github.com/microsoft/WindowsAppSDK-Samples/releases).

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/). For more information, see the Code of Conduct FAQ or contact opencode@microsoft.com with any additional questions or comments.

Once a new sample app has been added, Microsoft developers shall consider updating the internal Aggregator pipeline to:
- Enable building of the new sample app regularly.
- Add a new test case to the existing SampleTests TAEF test driver to regularly verify successful launchng of the sample app against the latest Windows App SDK version.

Refer to the documentation on the internal Aggregator repository for instructions, in the BuildSampleApps and TestSampleApps Stages.
