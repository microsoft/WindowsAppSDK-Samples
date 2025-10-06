// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

//
// Execution Provider Catalog Header
//
// Wrapper for Windows ML execution provider catalog ABI interfaces.
// Provides enumeration and discovery of available execution providers.
//

#include <wrl/client.h>
#include <vector>
#include "ExecutionProvider.h"
#include "Microsoft.Windows.AI.MachineLearning.h"

namespace CppAbiEPEnumerationSample
{

    /// <summary>
    /// Wrapper for the execution provider catalog
    /// Handles provider discovery and enumeration
    /// </summary>
    class ExecutionProviderCatalog
    {
    public:
        ExecutionProviderCatalog();
        std::vector<ExecutionProvider> FindAllProviders() const;

    private:
        Microsoft::WRL::ComPtr<ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProviderCatalog> m_catalog;
    };

} // namespace CppAbiEPEnumerationSample