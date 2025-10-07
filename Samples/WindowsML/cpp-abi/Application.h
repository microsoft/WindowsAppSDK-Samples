// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

//
// Application Header
//
// Main application logic for the Windows ML ABI sample.
// Orchestrates the interaction between providers, catalog, and user interface.
//

#include "ExecutionProviderCatalog.h"
#include "ExecutionProvider.h"

namespace CppAbiEPEnumerationSample
{

    /// <summary>
    /// Main application class that orchestrates the sample functionality
    /// Handles the application lifecycle and user interaction flow
    /// </summary>
    class Application
    {
    public:
        void Run();

    private:
        void RunInteractiveSession();
        void ProcessProviderSelection(const ExecutionProvider& provider);
        void PrepareProvider(const ExecutionProvider& provider);
        void TryRegisterProvider(const ExecutionProvider& provider);
    };

} // namespace CppAbiEPEnumerationSample