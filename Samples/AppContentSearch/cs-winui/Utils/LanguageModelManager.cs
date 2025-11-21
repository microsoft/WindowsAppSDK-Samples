// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.Extensions.AI;
using Microsoft.UI.Xaml.Controls;
using Notes.AI;
using Notes.ViewModels;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;

namespace Notes;

public enum LanguageModelType
{
    Phi,
    Azure,
    Foundry
}

public class LanguageModelManager
{
    private readonly Dictionary<LanguageModelType, ILanguageModelProvider> _providers;
    private ILanguageModelProvider? _currentProvider;

    private const string ModelSourceKey = "ModelSource";          // "phi" | "azure" | "foundry"

    public LanguageModelManager()
    {
        _providers = new Dictionary<LanguageModelType, ILanguageModelProvider> {};

        var modelType = LoadConfiguredModelType();
        _currentProvider = _providers[modelType];

        // Initialize the current provider asynchronously (fire and forget for backwards compatibility)
        Task.Run(async () =>
        {
            try
            {
                await _currentProvider.InitializeAsync();
            }
            catch (Exception ex)
            {
                // Initialization failures will be handled when SendRequest is called
                Debug.WriteLine($"Initialization failed: {ex}");
            }
        });
    }

    public LanguageModelType LoadConfiguredModelType()
    {
        var localSettings = ApplicationData.Current.LocalSettings;
        string source = localSettings.Values[ModelSourceKey] as string ?? "phi";

        LanguageModelType modelType = source switch
        {
            "azure" => LanguageModelType.Azure,
            "foundry" => LanguageModelType.Foundry,
            "phi" => LanguageModelType.Phi,
            _ => LanguageModelType.Phi
        };

        // Ensure provider exists in _providers
        if (!_providers.ContainsKey(modelType))
        {
            ILanguageModelProvider provider = modelType switch
            {
                LanguageModelType.Phi => new PhiSilicaProvider(),
                LanguageModelType.Azure => new AzureOpenAIProvider(),
                LanguageModelType.Foundry => new FoundryAIProvider(),
                _ => new PhiSilicaProvider()
            };
            _providers[modelType] = provider;
        }

        return modelType;
    }

    public IAsyncEnumerable<ChatResponseUpdate> SendRequest(
        string message,
        ChatContext context,
        SessionEntryViewModel entry,
        CancellationToken cancellationToken = default)
    {
        if (string.IsNullOrWhiteSpace(message))
        {
            return SingleUpdateStream("SendRequest - Invalid message.", entry);
        }

        var modelType = LoadConfiguredModelType();
        _currentProvider = _providers[modelType];

        return _currentProvider.SendStreamingRequestAsync(context, entry, cancellationToken);
    }

    public IEnumerable<ILanguageModelProvider> GetAvailableProviders()
    {
        return _providers.Values.Where(p => p.IsAvailable);
    }

    public async Task<bool> SwitchToProviderAsync(LanguageModelType modelType, CancellationToken cancellationToken = default)
    {
        if (!_providers.ContainsKey(modelType))
            return false;

        var provider = _providers[modelType];
        if (!provider.IsAvailable)
            return false;

        var initialized = await provider.InitializeAsync(cancellationToken);
        if (initialized)
        {
            _currentProvider = provider;
        }

        return initialized;
    }

    private async IAsyncEnumerable<ChatResponseUpdate> SingleUpdateStream(string text, SessionEntryViewModel entry)
    {
        ChatSessionViewModel._dispatcherQueue?.TryEnqueue(() => entry.Message += text);
        yield return new ChatResponseUpdate { Role = Microsoft.Extensions.AI.ChatRole.Assistant, Text = text };
        await Task.CompletedTask;
    }
}