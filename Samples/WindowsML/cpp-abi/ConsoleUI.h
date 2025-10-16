// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

//
// Console UI Header
//
// User interface utilities for the Windows ML sample application.
// Provides clean console interaction patterns.
//

#include <vector>
#include "ExecutionProvider.h"

namespace CppAbiEPEnumerationSample
{

    /// <summary>
    /// Console UI utilities for the sample application
    /// Handles user interaction and display formatting
    /// </summary>
    class ConsoleUI
    {
    public:
        static void PrintHeader();
        static void PrintProviders(const std::vector<ExecutionProvider>& providers);
        static int GetUserSelection(int maxOptions);
        static void PrintProgress(double progress);
        static void PrintResult(bool success);
        static void WaitForKeypress();
    };

} // namespace CppAbiEPEnumerationSample