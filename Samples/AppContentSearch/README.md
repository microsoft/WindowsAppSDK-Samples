---
page_type: sample  
languages:  
- csharp  
products:  
- windows  
- windows-app-sdk  
name: "AppContentSearch Sample"  
urlFragment: AppContentSearchSample  
description: "Demonstrates how to use the AppContentSearch APIs in Windows App SDK to index and semantically search text content and images in a WinUI3 notes application."  
extendedZipContent:  
- path: LICENSE  
  target: LICENSE  
---  


# AppContentSearch Sample Application

This sample demonstrates how to use App Content Search's **AppContentIndex APIs** in a **WinUI3** notes application. It shows how to create, manage, and semantically search through the index that includes both text content and images. It also shows how to use use the search results to enable retrieval augmented genaration (RAG) scenarios with language models.

> **Note**: This sample is targeted and tested for **Windows App SDK 2.0 Experimental2** and **Visual Studio 2022**. The AppContentSearch APIs are experimental and available in Windows App SDK 2.0 experimental2.


## Features

This sample demonstrates:

- **Creating Index**: Create an index with optional settings.
- **Indexing Content**: Add, update, and remove content from the index
- **Text Content Search**: Query the index for text-based results.
- **Image Content Search**: Query the index for image-based results.
- **Search Results Display**: Display both text and image search results with relevance highlighting and bounding boxes for image matches
- **Retrieval Augmented Generation (RAG)**: Use query search results with language models for retrieval augmented generation (RAG) scenarios.


## Prerequisites

* See [System requirements for Windows app development](https://docs.microsoft.com/windows/apps/windows-app-sdk/system-requirements).
* Make sure that your development environment is set up correctly&mdash;see [Install tools for developing apps for Windows 10 and Windows 11](https://docs.microsoft.com/windows/apps/windows-app-sdk/set-up-your-development-environment).
* This sample requires Visual Studio 2022 and .NET 9.


## Building and Running the Sample

* Open the solution file (`AppContentSearch.sln`) in Visual Studio.
* Press Ctrl+Shift+B, or select **Build** \> **Build Solution**.
* Run the application to see the Notes app with integrated search functionality.


## Related Documentation and Code Samples

* [Windows App SDK](https://docs.microsoft.com/windows/apps/windows-app-sdk/)
* [AppContentSearch API Documentation](https://learn.microsoft.com/en-us/windows/ai/apis/app-content-search)