# Windows App SDK Samples Guidelines

This document outlines the guidelines and best practices for Windows App SDK samples authors. The guidelines are organized into the following sections:

- [Workflow](#workflow)
- [Design Guidelines](#design-guidelines)
- [Coding Guidelines](#code-guidelines)
- [Sample Coverage](#sample-coverage)
- [Standardization and Naming](#standardization-and-naming)
- [Sample Templates](#sample-templates)
- [Repo Branching](#repo-branching)
- [Checklist](#checklist)

## Workflow

The following steps describe the workflow for the Windows App SDK Samples repo.

- Clone the Windows App SDK Samples repo.
- Create a topic branch for your sample.
- Go through the [sample checklist](#checklist).
- Create a PR and add your feature team as reviewers.

## Design Guidelines

If you have no strong opinion and if your feature allows for it, we recommend writing a scenario-based sample. See the guidelines below on how to write a scenario-based sample. If your feature would be better served as an end-to-end sample, you can proceed with that route. All samples should be complete but simple.

### Scenario-based

The sample is broken down into a number of scenarios. Each scenario covers one way of using the API being demonstrated.

A sample scenario is not a scratchpad. A scratchpad is a page full of buttons, checkboxes, text boxes, etc. where the code simply takes the values from the controls and calls the API and prints the result. This is useful for experimentation and functional testing, but it does not provide any insight into how the API should be used (indeed, such a program could be built automatically from API function signatures without any knowledge of the API at all). For example, a scratchpad Muffin program might have three buttons: *Start*, *Get Value*, and *Stop*:

```csharp
// Don't do this.
void Start_Clicked(object sender, RoutedEventArgs e)
{
    try
    {
        muffin.Start();
    }
    catch (Exception ex)
    {
        rootPage.NotifyMessage($"Start failed: {ex.Message}", NotifyType.ErrorMessage);
    }
}

// Don't do this.
void GetValue_Clicked(object sender, RoutedEventArgs e)
{
    try
    {
        int value = muffin.GetValue();
        rootPage.NotifyMessage($"Value is {value}", NotifyType.StatusMessage);
    }
    catch (Exception ex)
    {
        rootPage.NotifyMessage($"GetValue failed: {ex.Message}", NotifyType.ErrorMessage);
    }
}

// Don't do this.
void Stop_Clicked(object sender, RoutedEventArgs e)
{
    try
    {
        muffin.Stop();
    }
    catch (Exception ex)
    {
        rootPage.NotifyMessage($"Stop failed: {ex.Message}", NotifyType.ErrorMessage);
    }
}
```

The above scratchpad program does not provide any insight into the correct order of operations. It also implies that whenever you call the three methods, you always have to be prepared to catch unknown exceptions.

A scenario-based sample would walk through the correct order of operations:

```csharp
void DemoGetValue_Click(object sender, RoutedEventArgs e)
{
    muffin.Start();
    int value = muffin.GetValue();
    rootPage.NotifyMessage($"Value is {value}", NotifyType.StatusMessage);
    muffin.Stop();
}
```

This shows that the correct order of operations is *Start*, *Get Value*, *Stop*, and if you follow the correct order, then no exceptions will be raised.

The best practice of your API might be to avoid constantly starting and stopping the muffin. In that case, you can have three buttons *Start*, *Get Value*, and *Stop*, but enable and disable them to force the user through the correct order of operations.

```csharp
void ResetScenario()
{
    StartButton.IsEnabled = true;
    GetValueButton.IsEnabled = false;
    StopButton.IsEnabled = false;
}

void Start_Click(object sender, RoutedEventArgs e)
{
    muffin.Start();
    StartButton.IsEnabled = false; // cannot start twice
    GetValueButton.IsEnabled = true; // must start before getting the value
    StopButton.IsEnabled = true; // cannot stop unless started
}

void GetValue_Click(object sender, RoutedEventArgs e)
{
    int value = muffin.GetValue();
    rootPage.NotifyMessage($"Value is {value}", NotifyType.StatusMessage);
}

void Stop_Click(object sender, RoutedEventArgs e)
{
    muffin.Stop();
    GetValueButton.IsEnabled = false; // must start again before getting the value
    StopButton.IsEnabled = false; // cannot stop twice
    StartButton.IsEnabled = true; // you can start it again
}
```

### Completeness

A complete sample demonstrates correct usage, including all relevant error checking.

- Check for errors that apps would need to handle in production.
- Do not check for errors that indicate that the API was used incorrectly. Your sample should not use the API incorrectly.

### Simplicity

The developer reading the sample should be able to figure out how to use the API by reading the sample. Do not cloak the API inside layers of architectural beauty.

- Bad: Use a XAML control template to customize a dialing keypad that is in turn connected to a view model, where the Call button in the template is bound to a method in the View Model, which relays to another method in the View Model, which calls a method on a different object in the View Model and passes the value to a method on a helper class, which in turn calls the Dial method.
- Better: Have a text box called "Phone number" and a button called "Dial".

```csharp
void DialButton_Click(object sender, RoutedEventArgs e)
{
    // Perform simple cleanup of the phone number.
    var phoneNumber = PhoneNumber.Text.Trim();
    if (!string.IsNullOrEmpty(phoneNumber))
    {
        currentPhoneLine.Dial(phoneNumber, phoneNumber);
    }
}
```

It is okay for scenarios to be dependent upon each other. For example, Scenario 2 might say "This requires that you have preheated the oven as shown in Scenario 1." If a prerequisite is not met, show an appropriate message and disable the scenario.

It is okay for scenarios to hard-code sample data. Make sure to adhere to the guidelines for fictitious data.

Avoid helper classes unless they contribute to better understanding of the sample. For example instead of a helper class that calls `deferral.Complete()` automatically, call `deferral.Complete()` manually at the appropriate point in the sample. Developers reading the sample will see the correct point of completion and can create their own wrapper when porting to their app.

**Exception**: In C#, it is acceptable to use the `using` statement to automatically close/complete objects, since this is how production apps will close/complete them, too.

## Code Guidelines

The Windows App SDK Samples coding guidelines generally follow industry standards, with some additional rules specific to the samples environment.

### Principles

Samples should demonstrate correct usage and best practices for the Windows Runtime.

If you write samples in more than one language, they should use the same algorithms and design. This avoids confusion when someone compares the samples, sees that they use different algorithms, and can't tell which is the correct algorithm. This applies to small things, too: If the C++ sample checks for null, but the C# sample does not, then readers will not be sure whether checking for null is necessary. If the C++ sample demonstrates a feature but the C# sample does not, then readers may conclude that the feature is not supported in C#.

If the recommended algorithm is different in different languages, then it's okay for them to diverge.

### Language-specific guidelines

#### C++ samples

- New samples should be developed with C++/WinRT.

- Prefer C++ standard features over C-style Windows-specific ones

    - Use `std::atomic<T>` instead of `InterlockedXxx`.
    - Use `uint8_t` instead of `byte`.

    C++/WinRT provides additional modern-style helpers:

    - Use `clock::now()` instead of `GetSystemTimeAsFileTime`. Use `clock::to_file_time` to convert to `FILETIME`.
    - Use `to_hstring(guid)` instead of `GUIDToString2`.
    - Use `to_hstring(ansiStringPointer)` to convert from ANSI to Unicode.

- Returning from a coroutine

    - Use `co_return` to return from a coroutine. Do not use `return`, which is a Microsoft nonstandard extension.

- Switching to the UI thread

    - Use `co_await resume_foreground(Dispatcher())` instead of `co_await Dispatcher()`. The two are equivalent, but the first version has an explicit function name that developers can search for to learn what is going on.

- to_hstring

    If you need to stringify enums, declare `to_hstring` overrides in *SampleConfiguration.h* and implement them in *SampleConfiguration.cpp*. `#include "SampleConfiguration.h"` in your scenario header.

    We follow this pattern in the samples for consistency. Putting them in *SampleConfiguration.h* is helpful because it is often the case that the overrides are used by more than one scenario.

    Here's a typical implementation:

    ```cpp
    // SampleConfiguration.h
    namespace winrt
    {
        hstring to_hstring(Windows::UI::ViewManagement::UserInteractionMode value);
    }

    // SampleConfiguration.cpp
    using namespace winrt;
    using namespace Windows::UI::ViewManagement;

    hstring winrt::to_hstring(UserInteractionMode value)
    {
        switch (value)
        {
        case UserInteractionMode::Mouse:
            return L"Mouse";

        case UserInteractionMode::Touch:
            return L"Touch";

        default:
            return to_hstring(get_abi(value));
        }
    }
    ```

    The default case uses `get_abi` to format unknown values as decimal integers.

- The conventional order of class members is:

    - Public methods.
    - Private data members (rootPage, then other members, and event tokens last)
    - Private methods.

#### C# samples

- Follow the [C# Coding Conventions](https://docs.microsoft.com/dotnet/csharp/fundamentals/coding-style/coding-conventions) for naming, layout and commenting conventions, as well as other general guidelines.

- Interpolated strings are permitted.

### Other

- Use 4 spaces instead of tabs, and trim trailing spaces.

### Runtime exceptions

The Windows Runtime has a policy that exceptions are raised only for unrecoverable errors, such as a programming logic error (using an object incorrectly) or if the system has run out of memory. Exceptions should not be raised for recoverable situations the developer cannot protect against, such as surprise device removal.

Proper sample code should not raise Windows Runtime exceptions. For example, if it is illegal to call Start twice, then disable the Start button after the first call. This shows people how to use the API correctly. Do not put the Start call inside a `try` block and catch the invalid operation exception.

If a `try/catch` is unavoidable due to the API design:

- Work with your API design representative to fix the design so that exceptions do not occur for recoverable conditions.
- Scope the `try/catch` as narrowly as possible. Don't put an entire function inside a `try/catch`. Put only the portion that can raise an exception inside the `try` block.
- Catch the most specific exception. Allow unhandled exceptions to propagate.

    C# provides a handy shorthand for catching specific COM exceptions and allowing other exceptions to propagate.

    ```csharp
    // C#
    readonly int E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED = unchecked((int)0x80070093);

    try
    {
    ... something ...
    }
    catch (Exception ex) when (ex.HResult == E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED)
    {
        ... handle specific exception ...
    }
    ```

    In C++, you have to do this manually.

    ```cpp
    // C++/WinRT
     try
    {
    ... something ...
    }
    catch (hresult_error const& ex)
    {
        if (ex.code() != E_BLUETOOTH_ATT_WRITE_NOT_PERMITTED)
        {
            throw;
        }
        ... handle specific exception ...
    }
    ```

- When you catch an exception, make sure it is clear in code or in a comment what the exception means and either perform recovery directly in the sample or describe how the app should recover. Do not just log the error to the screen. That conveys no information about what exceptions to expect and what should be done if they arise.

### Language features to use/avoid

**Async/await**

The use of the async and await keywords (co_await in C++) is permitted in all languages.

**Automatically-typed variables**

In C++ and C#, limit use of the `auto` and `var` keywords to the cases where the right hand side is anonymous (e.g., a lambda) or if the type of the right hand side can be inferred without any domain-specific knowledge.

- Construction (✔ Okay)
    ```csharp
    // C#: The result of "new X" is always "X"
    var hostName = new HostName("localhost");
    ```
    ```cpp
    // C++/WinRT: The result of "X()" is always "X"
    auto hostName = HostName(L"localhost");
    ```

- Constructor-like well-known pattern (✔ Okay)
    ```csharp
    // C#
    var override = BrightnessOverride.GetForCurrentView();
    var barometer = Barometer.GetDefault();
    ```
    ```cpp
    // C++/WinRT
    auto override = BrightnessOverride::GetForCurrentView();
    auto barometer = Barometer::GetDefault();
    ```

- Casts and explicit conversions (✔ Okay)
    ```csharp
    // C#
    var item = (ComboBoxItem)comboBox->SelectedItem;
    var item = comboBox->SelectedItem as ComboBoxItem;
    ```
    ```cpp
    // C++
    auto value = static_cast<int32_t>(floatValue);
    ```
    ```cpp
    // C++/WinRT
    auto item = comboBox.SelectedItem().as<ComboBoxItem>();
    ```

- Anonymous types (✔ Okay)
    ```csharp
    // C#
    var entry = new { Name = name, Id = id };
    ```
    ```cpp
    // C++
    auto callback = [=]() { /* ... */ };
    ```

- Requires domain-specific knowledge (❌ Not okay)
    ```csharp
    // C#
    var buffer = streamSource.AddSourceBuffer(mimeType);
    ```
    ```cpp
    // C++/WinRT
    auto buffer = streamSource.AddSourceBuffer(mimeType);
    ```

People often read sample code online, rather than loading it into an IDE, so they do not get any benefit of IntelliSense.

The rule of thumb is that for non-anonymous types, the resulting type should appear somewhere on the right-hand side. This makes it possible to search the repo for "HttpRequestHeaderCollection" to find the places where a HttpRequestHeaderCollection is used.

Well-known patterns for constructor-like methods are

- `T.GetDefault()`
- `T.GetForX()`
- `T.FromX(), T.CreateFromX()`

and their `-ForUser` and `-Async` variants.

## Sample Coverage

There are several options and possible combinations that Windows App SDK samples applications may encompass, including:

- Languages: C#, C++, Rust, JS, etc.

- UI Frameworks: WinUI, WPF, WinForms, Console, Win32

- Runtime: MSIX Packaged, Unpackaged

- Deployment Method: Framework Package, AppLocal

- Security: FullTrust, PartialTrust

The guidelines for sample coverage are as follows:

- We recommend having samples for both C# and C++ at a minimum.

- There is not a hard requirement to cover all the other combinations (e.g. having all UI frameworks, having both Unpackaged and Packaged samples, or both Framework Packaged and AppLocal samples). However, if there is something unique about the specific component, the samples should highlight and showcase these scenarios.

- In general, we want to give component owners the flexibility to decide what samples are most important to include.

## Standardization and Naming

### Directory Structure

Samples should be organized and named by app type and feature for discoverability. The WindowsAppSDK-Samples repo is organized in the following manner:

```
\Samples
     \<FeatureName>
          \<Language>-<UI Framework>
```

- `<FeatureName>`: Use the simple English name of the feature instead of the component name (for example, *TextRendering* instead of *DWriteCore*). This makes samples more discoverable for end developers and aligns with the organization of the [Windows App SDK documentation](https://docs.microsoft.com/windows/apps/windows-app-sdk/).
- `<Language>`: *cs | cpp*
- `<UI Framework>`:
    - For C# samples: *winui | wpf | winforms | console*
    - For C++ samples: *winui | win32 | console | directx*
- The default configuration assumption is that samples are MSIX Packaged and deployed using the Framework Package. If a sample differs from the default, further specify the folder name, for example: *cs-wpf-unpackaged*.
- It is recommended to place shared code in a separate folder under the `<FeatureName>` folder, e.g. *cs-shared*.

Here is an example illustrating the naming guidelines above:

```
\Samples
    \Activation
        \cs-wpf  (MSIX Packaged + Framework Packaged)
        \cs-wpf-unpackaged (Unpackaged + Framework Packaged)
        \cs-shared
        ...
        \cpp-console
```

### Samples README

All features need to include a README.md file with their samples. The purpose of the README is to provide important information and context to customers to get started with a sample. The content of the README should include a description of what the samples does, how to build and run the sample, and include relevant media, such as images, screenshots, and diagrams where applicable. You can reference existing samples in this repo and in the [Windows Universal Samples repo](https://github.com/microsoft/Windows-universal-samples) for examples of how to organize the README.

#### Metadata

In the README.md file of your sample, make sure to include the YAML front-matter metadata header like below to integrate samples with the [Docs Samples Browser](https://docs.microsoft.com/samples/browse/). Refer to existing samples in this repository and the snippet below for an example.

```md
---
page_type: sample
languages:
- csharp
- cppwinrt
- cpp
products:
- windows
- windows-app-sdk
name: "Photo Editor"
urlFragment: PhotoEditor
description: "Shows how to retrieve photos from the Pictures library, and then edit the selected image with assorted photo effects."
extendedZipContent:
- path: LICENSE
  target: LICENSE
---
```

## Sample Templates

Any new C#/C++ WinUI-based samples should use the [WinUI 3 sample templates](../Templates). You can download the template VSIX file under the Github releases [here](https://github.com/microsoft/WindowsAppSDK-Samples/releases) in this repo, under the "Assets" section. The templates provide a scenario-based structure and ensure consistent UI and coding standards across WinUI samples.

## Repo Branching

When a new GA release of the Windows App SDK is available, we will create a new branch (e.g. `release/X.Y`), allowing us to maintain samples for previous/supported versions of the Windows App SDK. Only samples on the latest GA release (not experimental-only features) will be included in the branches for specific releases.

The `main` branch represents the head of development, so it may include experimental features or preview versions of the Windows App SDK. The default branch for the repo will be set to the latest publicly available Windows App SDK release, for example `release/1.x`. When a new GA release is available (e.g. Windows App SDK vX.Y), we snap a `release/X.Y` branch, and update the default branch to the new `release/X.Y` branch. Whenever a new release branch is snapped, we will need to pick and choose which samples go into the new branch. Experimental samples should be tagged clearly in the sample’s README so that these samples are not included.

## Checklist

### Build

Samples should build on all supported platforms (x64, x86, ARM64) and configurations (Debug, Release). Samples should set the minimum version to Windows 10 version 1809.

### Onboarding Samples CI Pipeline for new feature
PRs with new features must be setup with a new CI pipeline specific to each feature. 

#### Steps:
##### Create a new yml file at the root named SamplesCI-`<FeatureName>`.yml. 
It should have the contents below:
```
name: $(BuildDefinitionName)_$(date:yyMM).$(date:dd)$(rev:rrr)
stages:
- template: WindowsAppSDK-SamplesCI.yml
  parameters:
    FeatureDirectory: "<FeatureName>"
```

Be sure to add the new `<FeatureName>` to the `FeatureAreas` parameter in `SamplesCI-All.yml`.

##### Creating the pipeline

Go to https://dev.azure.com/ms/ProjectReunion/_build
-> Click New Pipeline 

At the "Select your repository" page, you'll see a link near the bottom called "native Azure DevOps experience".
Click that link so you can set up your pipeline against a GitHub repo.

-> GitHub 
-> Select Repository "microsoft/WindowsAppSDK-Samples" 
-> Configure your pipeline: Select "Existing Azure Pipelines YAML File"
-> Branch: `<YourPRBranch>` and Path: "SamplesCI-`<FeatureName>`.yml" -> Continue
    (Branch will be set to main later but can't be done right now because it doesn't exist on main yet)
-> Click Run

Warning: On the first run, you might need to permit resources for the pipeline to run. If you don't have the permission, please ask the repo admins to do so. 

##### Configuring the settings on the pipeline

Once you have started your run on the previous step, you can click the three dots on the top right,
which will bring up a menu option with "Edit pipeline" and this is where you want to go to. 
This will bring you to the yml itself. Click the three dots on the top right again and select "Triggers"

Triggers must have these settings below

##### Continuous Integration
- Check "Override the YAML continuous integration trigger from here" 
    - Click the "Disable continuous integration" radio button
##### Pull request validation
- Check "Override the YAML pull request trigger from here"
    and click "Enable pull request validation"
    - Branch Filters
        - Include "refs/heads/main"
    - Path Filters
        - Include "Samples/`<FeatureName>`"
    - Forks
        - Check "Build pull requests from forks of this repository"
    - Comments
        - Check "Require a team member's comment before building a pull request"
            - Click the "On all pull requests" radio button

##### Lastly rename the pipeline to "SamplesCI-`<FeatureName>`". That can be done by clicking on the name on top of the page. 

##### Setup status checks
Navigate to: https://github.com/microsoft/WindowsAppSDK-Samples/settings/branches 

If you get a 404, you likely don't have the necessary permissions, you'll need to request access.

In Branch Protection rules

Click "Edit" on the main branch and add your new pipeline to the collection of other pipelines in "Require status checks to pass before merging"

Warning: it might take a while for your new pipeline to show up. Please don't just add the the check with the individual job with a specific config and platform. Make sure to add the full "SamplesCI-`<FeatureName>`"

If you do not have permissions to modify these settings, please ask the repo admins to help you do so. 


### Deploy and test

Deploy and test your samples on Desktop.

### README file

Make sure to include a README file with your sample. See the above section on [Samples README](#samples-readme).

### Copyright Headers

Microsoft-authored samples must include the appropriate copyright notice and license text added to the header of each file.

- For C#/C++ files (*h, *.cpp, *.cs, *.idl):
    ```csharp
    // Copyright (c) Microsoft Corporation.
    // Licensed under the MIT License.
    ```

- For XAML files (*.xaml):
    ```xml
    <!-- Copyright (c) Microsoft Corporation.
     Licensed under the MIT License. -->
    ```

### Pass certification

Run the Windows App Certification Kit (WACK) on a Release build. If your sample does something architecture-specific (e.g., uses different intrinsics on x86 vs ARM), then run WACK on applicable architectures. Otherwise, one architecture is sufficient.

### Perform code analysis

Make sure samples build clean with no warnings or errors.

### Verify accessibility

See the [accessibility checklist](https://docs.microsoft.com/windows/apps/design/accessibility/accessibility-checklist) and internal Windows Samples Accessibility Guidelines for further details.

### Content

Avoid culturally-sensitive content, such as flags, countries, and names of disputed geographic regions. Avoid slang and reference to popular culture, which may not be understood by all developers.

### Legal issues

Comply with all licensing requirements for any content included with your sample. If you include images, fonts, text excerpts, and other potentially-copyrightable material, include information in the pull request confirming that the sample complies with all licensing requirements. (For example, "The images were obtained from <links> and are redistributable under the Creative Commons BY license. Attribution is included in the README.")

Do not include proprietary intellectual property in samples.
