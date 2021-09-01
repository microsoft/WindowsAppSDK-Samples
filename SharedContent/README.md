#  Windows App SDK Sample Templates

The `SharedContents` folder contains sample templates for C# and C++ (using WinUI 3). To use the current templates, sample authors should follow these steps:

1. Clone the Windows App SDK samples repo and then copy all the files under the appropriate template folder to your feature sample directory. For example, create a new folder for your sample called `Samples/MyFeatureName/cs-winui/`, and copy all the contents under `SharedContent/cs-winui-template` to this new folder.

2. Rename the sample solution and project files to your feature name. For example, rename all instances of *WinUI3TemplateCs* to *MyFeatureName*.

3. Rename the Scenario pages to a short description of the API functionality you are demonstrating. For example, rename *Scenario1_ShortName* to *Scenario1_CopyImage*. Add your implementation in the scenario XAML and code-behind files. If more scenario pages are needed, you can make a copy of the scenario-specific pages and add another entry to the `SampleConfiguration.cs` or `SampleConfiguration.cpp` file.
