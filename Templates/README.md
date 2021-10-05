# Windows App SDK Sample Templates

The `Templates` folder contains WinUI3-based sample templates for C# and C++/WinRT. **WindowsAppSDKSampleVsix.sln** builds a VSIX file which installs project and item templates in Visual Studio 2022 for sample authors.

## Prerequisites

The following are prequisites to using the sample templates:

- Visual Studio 2022 Preview, with the following **Workloads** installed:
   - Universal Windows Platform development
   - Desktop development with C++
   - .NET Desktop Development
- Single-project MSIX Packaging tools for Visual Studio 2022: [VSIX extension](https://marketplace.visualstudio.com/items?itemName=ProjectReunion.MicrosoftSingleProjectMSIXPackagingToolsDev17)

## Usage

1. Download the Windows App SDK Sample VSIX. The VSIX will install the following templates:

    - Project Templates: Windows App SDK Sample C# WinUI, Windows App SDK Sample C++ WinUI
    - Item Templates:

2. Create a new project in Visual Studio 2022. In the New Project dialog, search for either *Windows App SDK C# Sample* or *Windows App SDK C++ Sample*.

    ![Create New Project](./readme-images/create-newproject.png)

    a. Replace the Project name with your feature name, for example *Windowing*. Check the box for "Place solution and project in same directory".
    ![Name Project](./readme-images/name-project.png)

3. To add a new Scenario Page, right-click on the project and select *Add* -> *New Item*. Select *WinUI* and *Sample C#/C++ WinUI Scenario Page*. Name the file with your Scenario name.

    ![Name Project](./readme-images/add-scenario.png)

    To add the new page to your sample's navigation view, add the page to the scenario List in *SampleConfiguration.cs* or *SampleConfiguration.cpp*

## Tips and Best Practices

* The VSIX comes preinstalled with a public version of the Windows App SDK package. To develop your samples with an internal package version, first configure your NuGet package sources and then modify the package version in your project.

* For C++ samples, consolidate all *.idl* code into the *Project.idl* file. If you add additional Scenario pages to the project, copy and paste the contents of the newly created *Scenario3_FeatureName.idl* file into *Project.idl*, and delete *Scenario3_FeatureName.idl*.

## Troubleshooting

* Deploying the single-project MSIX requires checking *Deploy* in *Build* -> *Configuration Manager* for each Platform/Configuration.
