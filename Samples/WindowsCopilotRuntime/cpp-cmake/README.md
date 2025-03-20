# Using Windows Copilot Runtime in C++ with CMake

This sample shows how to use Windows App SDK and the Windows Copilot Runtime Generative AI APIs
from a C++ app built using CMake. You can use the CMakeLists and vcpkg ports it contains to build
your own apps.

While the sample is a simple console mode application, you can also use it with the UI framework
of your choice.

Topics and concepts in this example include:

* Referencing the Windows App SDK, WebView2, and C++/WinRT vcpkgs in CMake
* Accessing the Windows App SDK implementation from an unpackaged native (C++) app
* Calling the Microsoft.Windows.AI.Generative.LanguageModel API to generate some text

> **Special note:** The vcpkgs used for C++/WinRT, Windows App SDK, and WebView2 are unofficial as
> of 19th March 2025. We're working on making them widely available.

> **Special note:** Windows Copilot Runtime is currently an Experimental feature. Consult its license
> agreement to see how you can use it. The APIs are subject to change. Your customers will not have
> the [Windows App SDK 1.7-experimental3](https://learn.microsoft.com/windows/apps/windows-app-sdk/experimental-channel#version-17-experimental-170-experimental3)
> framework package for production use.

## Prerequisites

1. Install vcpkg [Microsoft vcpkg CMake integration](https://learn.microsoft.com/vcpkg/get_started/get-started)
to get vcpkg cloned and ready for use.
2. Install [CMake](https://cmake.org/download/). You can also `winget install Kitware.CMake`.
3. Clone this repo to your development environment.
4. Use a compiler toolchain supported by CMake and vcpkg. This example will use Visual Studio 2022. You can install the community edition with `winget install Microsoft.VisualStudio.2022.Community`.
5. A [Windows Copilot+ PC](https://learn.microsoft.com/windows/ai/npu-devices/) to run this sample
6. Install the [Windows App SDK 1.7 Experimental3](https://learn.microsoft.com/windows/apps/windows-app-sdk/downloads#windows-app-sdk-17-experimental) framework package on your Copilot+ PC (Note: installing the Experimental package will not affect your production apps).


After configuring the tools, open the sample directory in your build environment.

