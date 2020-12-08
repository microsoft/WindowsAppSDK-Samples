---
title: "Load resources using MRT Core"
description: "Use MRT Core's ResourceLoader and ResourceManager to load resources from several resource files"
page_type: sample
languages: csharp
products: windows
---

# Load resources using MRT Core

MRT Core is the modern resource loading library used in WunUI 3 and Project Reunion. This sample demonstrates how to use MRT Core to load resources from multiple sample files, and how to select which version of a specific named resource to load. 

## Prerequisites

 - [Visual Studio 2019 Preview](https://visualstudio.microsoft.com/vs/preview) (any edition)

## Building and running the sample

This sample consists of two solutions, one is a required library for the other and must be manually built before the MRT Core sample will build or run. This is a temporary step that will be removed as development continues on Project Reunion and MRT Core.

1.  Open winui_class_lib\winui_class_lib.sln in Visual Studio 2019 Preview.
2. Build the solution using the same build configuration you intend to use for the MRT Core sample.
3. Close the winui_class_lib solution.

Now the MRT Core sample can be built and run. The sample project is configured to find the winui_class_lib build output automatically. If this fails, see troubleshooting below.

4. Open winui_desktop_packaged_app\winui_desktop_packaged_app.sln in Visual Studio 2019 Preview.
5. Build the solution.
6. Right click on the solution and click **Deploy Solution**
7. Debug the winui_desktop_packaged_app project.

## Troubleshooting

If winui_desktop_packaged_app cannot find the output from winui_class_lib, follow the following steps.

1. Right click the winui_desktop_packaged_app project in Solution Explorer and click **Unload Project**
2. Right click the winui_desktop_packaged_app (unloaded) project in Solution Explorer and click **Edit Project File**
3. In the csproj file, find the element `<Include="winui_class_lib">` (On or near line 34)
4. Update the value of the child `<HintPath>` element to point directly to the dll output of winui_class_lib.
5. Save the csproj file
6. Right click the winui_desktop_packaged_app (unloaded) project in Solution Explorer and click **Reload Project**.
7. Try building the solution again; the build should succeed.

## Related Links

- [Project Reunion]()
- [MRT Core]()
