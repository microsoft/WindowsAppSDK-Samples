# Squares sample

This sample illustrates how to use the Input, Composition, Accessibility and Content APIs to create 
an interactive user experience.

## Building & Running the sample

This sample uses experimental API's first published in WindowsAppSdk 1.2 Experimental release. 
You should just be able to build the sample in Visual Studio by opening Squares.sln. 
Since this is an unpackaged application, WindowsAppSdk 1.2 Experimental Runtime will need to be 
installed manually before running the sample. 

Here are the links for the installer packages for WindowsAppSdk 1.2 Experimental Runtime:

- [AMD64](https://aka.ms/windowsappsdk/1.2/1.2.220909.2-experimental2/windowsappruntimeinstall-x64.exe)
- [ARM64](https://aka.ms/windowsappsdk/1.2/1.2.220909.2-experimental2/windowsappruntimeinstall-arm64.exe)
- [X86](https://aka.ms/windowsappsdk/1.2/1.2.220909.2-experimental2/windowsappruntimeinstall-x86.exe)


## Conceptual elements

### Host

The host is represented by the `DesktopChildSiteBridge`, and allows hosting in a HWND based 
environment. It takes a parent HWND as a parameter on creation, and it will be a child of that 
parent HWND. 


### Content

The content is represented by the `ContentIsland` and is a self-contained set of output, input, 
state, and automation handling that provides a user experience. An application typically uses 
`ContentIslands` as modernized [WS_CHILD
style HWND](https://docs.microsoft.com/en-us/windows/win32/winmsg/window-features#child-windows)
equivalents to provide Composition-based experiences with animations, rendering effects, and
interactive manipulations.

## Connection

After `DesktopChildSiteBridge` and `ContentIsland` are created, they need to be connected so that 
the Content will show up in the Host. This is accomplished by calling 
DesktopChildSiteBridge.Connect(ContentIsland). Once this connection has been established, the
Content will inherit state from the HWND environment around it by default. For instance, if the 
parent HWND is hidden or resized, the Content will also follow that state.
