// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.Extensions.AI;
using Microsoft.Windows.AI.ContentSafety;
using Microsoft.Windows.AI.Text;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Foundation;

namespace Notes.AI;

internal class PhiSilicaClient : IChatClient
{
    // Search Options
    private const int DefaultTopK = 50;
    private const float DefaultTopP = 0.9f;
    private const float DefaultTemperature = 1;

    private LanguageModel? _languageModel;
    private LanguageModelContext? _languageModelContext;

    public ChatClientMetadata Metadata { get; }

    private PhiSilicaClient()
    {
        Metadata = new ChatClientMetadata("PhiSilica", new Uri($"file:///PhiSilica"));
    }

    private static ChatOptions GetDefaultChatOptions()
    {
        return new ChatOptions
        {
            Temperature = DefaultTemperature,
            TopP = DefaultTopP,
            TopK = DefaultTopK,
        };
    }

    public static async Task<PhiSilicaClient?> CreateAsync(CancellationToken cancellationToken = default)
    {
#pragma warning disable CA2000 // Dispose objects before losing scope
        var phiSilicaClient = new PhiSilicaClient();
#pragma warning restore CA2000 // Dispose objects before losing scope

        try
        {
            await phiSilicaClient.InitializeAsync(cancellationToken);
        }
        catch
        {
            return null;
        }

        return phiSilicaClient;
    }

    public Task<ChatResponse> GetResponseAsync(IList<ChatMessage> chatMessages, ChatOptions? options = null, CancellationToken cancellationToken = default) =>
        GetStreamingResponseAsync(chatMessages, options, cancellationToken).ToChatResponseAsync(cancellationToken: cancellationToken);

    public async IAsyncEnumerable<ChatResponseUpdate> GetStreamingResponseAsync(IList<ChatMessage> chatMessages, ChatOptions? options = null, [EnumeratorCancellation] CancellationToken cancellationToken = default)
    {
        if (_languageModel == null)
        {
            throw new InvalidOperationException("Language model is not loaded.");
        }

        var prompt = GetPrompt(chatMessages);

        await foreach (var part in GenerateStreamResponseAsync(prompt, options, cancellationToken))
        {
            yield return new ChatResponseUpdate
            {
                Role = ChatRole.Assistant,
                Text = part,
            };
        }
    }

    private (LanguageModelOptions? ModelOptions, ContentFilterOptions? FilterOptions) GetModelOptions(ChatOptions options)
    {
        if (options == null)
        {
            return (null, null);
        }

        var languageModelOptions = new LanguageModelOptions
        {
            Temperature = options.Temperature ?? DefaultTemperature,
            TopK = (uint)(options.TopK ?? DefaultTopK),
            TopP = (uint)(options.TopP ?? DefaultTopP),
        };

        var contentFilterOptions = new ContentFilterOptions();

        if (options?.AdditionalProperties?.TryGetValue("input_moderation", out SeverityLevel inputModeration) == true && inputModeration != SeverityLevel.Low)
        {
            contentFilterOptions.PromptMaxAllowedSeverityLevel = new TextContentFilterSeverity
            {
                Hate = inputModeration,
                Sexual = inputModeration,
                Violent = inputModeration,
                SelfHarm = inputModeration
            };
        }

        if (options?.AdditionalProperties?.TryGetValue("output_moderation", out SeverityLevel outputModeration) == true && outputModeration != SeverityLevel.Low)
        {
            contentFilterOptions.ResponseMaxAllowedSeverityLevel = new TextContentFilterSeverity
            {
                Hate = outputModeration,
                Sexual = outputModeration,
                Violent = outputModeration,
                SelfHarm = outputModeration
            };
        }

        return (languageModelOptions, contentFilterOptions);
    }

    private string GetPrompt(IEnumerable<ChatMessage> history)
    {
        if (!history.Any())
        {
            return string.Empty;
        }

        string prompt = string.Empty;

        var firstMessage = history.FirstOrDefault();

        _languageModelContext = firstMessage?.Role == ChatRole.System ?
            _languageModel?.CreateContext(firstMessage.Text, new ContentFilterOptions()) :
            _languageModel?.CreateContext();

        for (var i = 0; i < history.Count(); i++)
        {
            var message = history.ElementAt(i);
            if (message.Role == ChatRole.System)
            {
                if (i > 0)
                {
                    throw new ArgumentException("Only first message can be a system message");
                }
            }
            else if (message.Role == ChatRole.User)
            {
                string msgText = message.Text ?? string.Empty;
                prompt += msgText;
            }
            else if (message.Role == ChatRole.Assistant)
            {
                prompt += message.Text;
            }
        }

        return prompt;
    }

    public void Dispose()
    {
        _languageModel?.Dispose();
        _languageModel = null;
    }

    public object? GetService(Type serviceType, object? serviceKey = null)
    {
        return
            serviceKey is not null ? null :
            _languageModel is not null && serviceType?.IsInstanceOfType(_languageModel) is true ? _languageModel :
            serviceType?.IsInstanceOfType(this) is true ? this :
            serviceType?.IsInstanceOfType(typeof(ChatOptions)) is true ? GetDefaultChatOptions() :
            null;
    }

    public static bool IsAvailable()
    {
        try
        {
            return LanguageModel.GetReadyState() == Microsoft.Windows.AI.AIFeatureReadyState.Ready;
        }
        catch
        {
            return false;
        }
    }

    private async Task InitializeAsync(CancellationToken cancellationToken = default)
    {
        cancellationToken.ThrowIfCancellationRequested();

        if (!IsAvailable())
        {
            await LanguageModel.EnsureReadyAsync();
        }

        cancellationToken.ThrowIfCancellationRequested();

        _languageModel = await LanguageModel.CreateAsync();
    }

#pragma warning disable IDE0060 // Remove unused parameter
    public async IAsyncEnumerable<string> GenerateStreamResponseAsync(string prompt, ChatOptions? options = null, [EnumeratorCancellation] CancellationToken cancellationToken = default)
#pragma warning restore IDE0060 // Remove unused parameter
    {
        if (_languageModel == null)
        {
            throw new InvalidOperationException("Language model is not loaded.");
        }

        string currentResponse = string.Empty;
        using var newPartEvent = new ManualResetEventSlim(false);

        IAsyncOperationWithProgress<LanguageModelResponseResult, string>? progress;
        if (options == null)
        {
            progress = _languageModel.GenerateResponseAsync(_languageModelContext, prompt, new LanguageModelOptions());
        }
        else
        {
            var (modelOptions, filterOptions) = GetModelOptions(options);
            progress = _languageModel.GenerateResponseAsync(_languageModelContext, prompt, modelOptions);
        }

        progress.Progress = (result, value) =>
        {
            currentResponse = value;
            newPartEvent.Set();
            if (cancellationToken.IsCancellationRequested)
            {
                progress.Cancel();
            }
        };

        while (progress.Status != AsyncStatus.Completed)
        {
            await Task.CompletedTask.ConfigureAwait(ConfigureAwaitOptions.ForceYielding);

            if (newPartEvent.Wait(10, cancellationToken))
            {
                yield return currentResponse;
                newPartEvent.Reset();
            }
        }

        var response = await progress;

        yield return response?.Status switch
        {
            LanguageModelResponseStatus.BlockedByPolicy => "\nBlocked by policy",
            LanguageModelResponseStatus.PromptBlockedByContentModeration => "\nPrompt blocked by content moderation",
            LanguageModelResponseStatus.ResponseBlockedByContentModeration => "\nResponse blocked by content moderation",
            _ => string.Empty,
        };
    }
}