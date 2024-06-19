---
languages:
- csharp
products:
- windows-wpf
page_type: sample
name: "Calculator Demo"        
description: "This sample application is a simple calculator. It demonstrates layout, controls, and some simple animation."
---
# Calculator Demo
This sample application is a simple calculator. It demonstrates layout, controls, and some simple animation.

## Build the sample
The easiest way to use these samples without using Git is to download the zip file containing the current version (using the link below or by clicking the "Download ZIP" button on the [repo](https://github.com/microsoft/WPF-Samples?tab=readme-ov-file) page). You can then unzip the entire archive and use the samples in [Visual Studio 2022](https://www.visualstudio.com/wpf-vs).

[Download the samples ZIP](../../archive/main.zip)

### Deploying the sample
- Select Build > Deploy Solution. 

### Deploying and running the sample
- To debug the sample and then run it, press F5 or select Debug >  Start Debugging. To run the sample without debugging, press Ctrl+F5 or selectDebug > Start Without Debugging. 

# Calculator Demo update

This section shows how we updated the Calculator Demo to give it a Win11 look and feel.

## Commit 1: Use .net 9 preview 4 (calc_update_use_dotnet9)

DotNet9 hadn't yet shipped when we made this demo, so we had to use the .net9 preview 4 sdk installer from [here](https://github.com/dotnet/installer).
Also see that we had to add a dotnet9 entry to our nuget.config, we can remove this once preview 4 ships publicly.

To make the app build with .net 9, we change the csproj file to use net9.0 as the TargetFramework.

## Commit 2: Use the new WPF Fluent Resource Dictionary (calc_update_fluent)

For our app to use the new styles, we need to add the Fluent ResourceDictionary to our Application's merged ResourceDictionary.  See App.xaml.

## Commit 3: Now that the new styles are applied, make some tweaks to add polish (calc_update_tweaks)

We need to make a few tweaks, mostly for layout, to make things look nice:
* We make the default Button style set Horizontal and VerticalAlignment to stretch so that each button fills its grid cell.  We add a margin
too so that the buttons have a little space between them.
* We remove the hardcoded height of the Menu bar at the top so that the menu items are fully visible.
* We increase the default size of the Window, since the new styles tend to use larger text sizes.
* We define a "GrayButton" Style for all the gray buttons in the calculator UI.  We set a background that is gray and translucent so that
the mica background is visible through it.
* We give the "=" button the background and foreground of an accent button.
* On the TextBoxes, we made the text a little larger and the border more subtle.