// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

//
// Execution Provider Header
//
// Wrapper for Windows ML execution provider ABI interfaces.
// Provides a clean C++ interface for working with execution providers.
//

#include <wrl/client.h>
#include <string>
#include "Microsoft.Windows.AI.MachineLearning.h"

namespace CppAbiEPEnumerationSample
{

    /// <summary>
    /// Wrapper for an execution provider instance
    /// Encapsulates the complexity of ABI interface interactions
    /// </summary>
    class ExecutionProvider {
    public:
        // Constructor
        explicit ExecutionProvider(Microsoft::WRL::ComPtr<ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProvider> provider);

        // Provider information methods
        std::wstring GetName() const;
        std::wstring GetDescription() const;
        ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState GetReadyState() const;
        std::wstring GetReadyStateString(ABI::Microsoft::Windows::AI::MachineLearning::ExecutionProviderReadyState state) const;

        // Provider operations
        bool EnsureReadyAsync() const;
        bool TryRegister() const;

    private:
        Microsoft::WRL::ComPtr<ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProvider> m_provider;
    };

} // namespace CppAbiEPEnumerationSample