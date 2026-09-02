---
page_type: sample  
languages:  
- csharp  
- cppwinrt  
- cpp  
products:  
- windows  
- windows-app-sdk  
name: "OAuth2Manager Sample"  
urlFragment: OAuth2ManagerSample  
description: "An API to support OAuth 2.0 in desktop apps using default browser launch."  
extendedZipContent:  
- path: LICENSE  
  target: LICENSE  
---  


# OAuth2Manager Sample Application

This sample demonstrates how to perform OAuth 2.0 using **OAuth2Manager** API in a **WinUI3** app. 

> **Note**: This sample is targeted and tested for the **Windows App SDK 1.7 Experimental 1** and **Visual Studio 2022**.


## Features

This sample demonstrates the use of OAuth 2.0 using:

- **Authorization code grant type**: Getting access token in two steps. See [RFC 6749](https://www.rfc-editor.org/rfc/rfc6749#section-4.1)
- **Implicit grant type**: Getting access token in one step. See [RFC 6749](https://www.rfc-editor.org/rfc/rfc6749#section-4.2)

## Prerequisites


* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
* The C# sample requires Visual Studio 2022 and .NET 6.

## Building and Running the Sample

* Open the solution file (`.sln`) in Visual Studio.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.


## Related Documentation and Code Samples
* [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
* [OAuth2Manager API Spec Documentation](https://github.com/microsoft/WindowsAppSDK/blob/main/specs/OAuth/OAuth2Manager.md)



