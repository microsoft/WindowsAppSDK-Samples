// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

//
// Execution Provider Catalog Implementation
//
// Implementation of Windows ML execution provider catalog ABI wrapper.
//

#include "ExecutionProviderCatalog.h"
#include <windows.h>
#include <roapi.h>
#include <combaseapi.h>
#include <wrl/wrappers/corewrappers.h>
#include <stdexcept>

namespace CppAbiEPEnumerationSample
{

    ExecutionProviderCatalog::ExecutionProviderCatalog()
    {
        // Get the catalog factory
        Microsoft::WRL::ComPtr<ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProviderCatalogStatics> factory;
        HSTRING className;
        HRESULT hr = WindowsCreateString(RuntimeClass_Microsoft_Windows_AI_MachineLearning_ExecutionProviderCatalog,
            static_cast<UINT32>(wcslen(RuntimeClass_Microsoft_Windows_AI_MachineLearning_ExecutionProviderCatalog)),
            &className);

        if (SUCCEEDED(hr))
        {
            hr = RoGetActivationFactory(className, IID_PPV_ARGS(&factory));
            WindowsDeleteString(className);
        }

        if (SUCCEEDED(hr))
        {
            hr = factory->GetDefault(&m_catalog);
        }

        if (FAILED(hr))
        {
            throw std::runtime_error("Failed to create ExecutionProviderCatalog");
        }
    }

    std::vector<ExecutionProvider> ExecutionProviderCatalog::FindAllProviders() const
    {
        std::vector<ExecutionProvider> providers;

        // The method returns a C-style array, not a vector
        UINT32 resultLength = 0;
        ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProvider** result = nullptr;

        HRESULT hr = m_catalog->FindAllProviders(&resultLength, &result);

        if (SUCCEEDED(hr) && result)
        {
            for (UINT32 i = 0; i < resultLength; i++)
            {
                Microsoft::WRL::ComPtr<ABI::Microsoft::Windows::AI::MachineLearning::IExecutionProvider> provider;
                provider.Attach(result[i]); // Take ownership without AddRef (already ref counted from array)
                providers.emplace_back(ExecutionProvider(provider));
            }

            // The array itself needs to be freed
            CoTaskMemFree(result);
        }

        return providers;
    }

} // namespace CppAbiEPEnumerationSample