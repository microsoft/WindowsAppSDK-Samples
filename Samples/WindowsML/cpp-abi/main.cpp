// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

//
// Windows ML ABI Sample Entry Point
//
// This sample demonstrates Windows ML API usage through direct ABI/COM interfaces.
// The implementation is factored into modular components for easy understanding and reuse:
//
// - AbiUtils.h: Common RAII wrappers for Windows Runtime types
// - AsyncHandlers.h/.cpp: Event-driven async operation handlers
// - ExecutionProvider.h/.cpp: Wrapper for IExecutionProvider ABI interface
// - ExecutionProviderCatalog.h/.cpp: Wrapper for IExecutionProviderCatalog interface
// - ConsoleUI.h/.cpp: User interface utilities
// - Application.h/.cpp: Main application logic and orchestration
//
// Each component can be easily copied and reused in other projects.
//

#include "Application.h"

//
// Application Entry Point
//
int main()
{
    CppAbiEPEnumerationSample::Application app;
    app.Run();
    return 0;
}