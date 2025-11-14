// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.AI.Foundry.Local;
using Microsoft.Extensions.AI;
using Notes.ViewModels;
using OpenAI;
using OpenAI.Chat;
using System;
using System.ClientModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;
using AppChatRole = Notes.ViewModels.ChatRole;
using ExtChatRole = Microsoft.Extensions.AI.ChatRole;

namespace Notes.AI;

public class FoundryAIProvider : ILanguageModelProvider
{
    private OpenAIClient? _foundryClient;
    private ChatClient? _foundryChatClient;
    private string? _foundryModelId;
    private Task? _foundryInitTask;
    private string? _foundryModelAliasRequested;

    private const string FoundryModelKey = "FoundryModelName";
    private const string SystemPrimaryPrompt =
        "You are a helpful assistant. Your output should be ONLY in plain text. There should be NO markdown elements.";
    private const string SystemSecondaryPrompt =
        "This is the end of the document snippets.\r\nPlease answer the following user question:\r\n";

    public FoundryAIProvider()
    {
        // Initialize the task when the provider is created
        if (IsAvailable)
        {
            _foundryInitTask = EnsureFoundryClientAsync(CancellationToken.None);
        }
    }

    public string Name => "Foundry AI Local";

    public bool IsAvailable
    {
        get
        {
            try
            {
                var settings = ApplicationData.Current.LocalSettings;
                string alias = (settings.Values[FoundryModelKey] as string)?.Trim() ?? "";
                return !string.IsNullOrWhiteSpace(alias) || HasCachedModels();
            }
            catch
            {
                return false;
            }
        }
    }

    public async Task<bool> InitializeAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            await (_foundryInitTask ?? EnsureFoundryClientAsync(cancellationToken));
            return _foundryClient != null && _foundryChatClient != null && !string.IsNullOrEmpty(_foundryModelId);
        }
        catch
        {
            return false;
        }
    }

    public async IAsyncEnumerable<ChatResponseUpdate> SendStreamingRequestAsync(
        ChatContext context,
        SessionEntryViewModel entry,
        [EnumeratorCancellation] CancellationToken cancellationToken = default)
    {
        string? initError = null;
        try
        {
            await (_foundryInitTask ?? EnsureFoundryClientAsync(cancellationToken));
        }
        catch (Exception ex)
        {
            initError = $"Foundry init error: {ex.Message}";
        }

        if (initError != null)
        {
            yield return Append(entry, initError);
            yield break;
        }

        if (_foundryClient == null || _foundryChatClient == null || string.IsNullOrEmpty(_foundryModelId))
        {
            yield return Append(entry, "Foundry client not ready.");
            yield break;
        }

        var chatMessages = BuildAzureChatMessages(context);

        IEnumerable<StreamingChatCompletionUpdate>? stream = null;
        string? startError = null;
        try
        {
            stream = _foundryChatClient.CompleteChatStreaming(chatMessages);
        }
        catch (Exception ex)
        {
            startError = $"Foundry start error: {ex.Message}";
        }

        if (startError != null)
        {
            yield return Append(entry, startError);
            yield break;
        }

        bool canceled = false;
        Exception? streamEx = null;

        using (var enumerator = stream!.GetEnumerator())
        {
            while (true)
            {
                bool hasNext;
                try
                {
                    hasNext = enumerator.MoveNext();
                }
                catch (OperationCanceledException)
                {
                    canceled = true;
                    break;
                }
                catch (Exception ex)
                {
                    streamEx = ex;
                    break;
                }

                if (!hasNext) break;

                if (cancellationToken.IsCancellationRequested)
                {
                    canceled = true;
                    break;
                }

                var update = enumerator.Current;
                foreach (var part in update.ContentUpdate)
                {
                    if (!string.IsNullOrEmpty(part.Text))
                    {
                        yield return Append(entry, part.Text);
                    }
                }

                await Task.Yield();
            }
        }

        if (canceled)
        {
            yield return Append(entry, "[Canceled]");
        }
        else if (streamEx != null)
        {
            yield return Append(entry, $"Foundry streaming error: {streamEx.Message}");
        }
    }

    private bool HasCachedModels()
    {
        try
        {
            var foundryManager = new FoundryLocalManager();
            var cached = foundryManager.ListCachedModelsAsync().GetAwaiter().GetResult();
            return cached.Count > 0;
        }
        catch
        {
            return false;
        }
    }

    private async Task EnsureFoundryClientAsync(CancellationToken ct)
    {
        if (_foundryClient != null && _foundryChatClient != null) return;

        var settings = ApplicationData.Current.LocalSettings;
        string alias = (settings.Values[FoundryModelKey] as string)?.Trim() ?? "";
        if (string.IsNullOrWhiteSpace(alias))
        {
            try
            {
                var foundryManager = new FoundryLocalManager();

                var cached = await foundryManager.ListCachedModelsAsync();
                if (cached.Count != 0)
                {
                    ModelInfo modelInfo = cached.First();
                    alias = modelInfo.Alias;
                }
                else
                {
                    throw new InvalidOperationException("Foundry model list could not be retrieved.");
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Foundry: Cached model discovery failed: {ex.Message}");
                throw;
            }
        }
        _foundryModelAliasRequested = alias;

        try
        {
            var manager = await FoundryLocalManager.StartModelAsync(aliasOrModelId: alias);
            var modelInfo = await manager.GetModelInfoAsync(aliasOrModelId: alias)
                ?? throw new InvalidOperationException("Foundry model info could not be retrieved.");

            _foundryModelId = modelInfo.ModelId;
            var key = new ApiKeyCredential(manager.ApiKey);
            _foundryClient = new OpenAI.OpenAIClient(key, new OpenAIClientOptions { Endpoint = manager.Endpoint });
            _foundryChatClient = _foundryClient.GetChatClient(_foundryModelId);
        }
        catch (Exception ex)
        {
            Debug.WriteLine("Foundry init failed: " + ex.Message);
            throw;
        }
    }

    private ChatResponseUpdate Append(SessionEntryViewModel entry, string text, ChatResponseUpdate? original = null)
    {
        ChatSessionViewModel._dispatcherQueue?.TryEnqueue(() => entry.Message += text);
        return original ?? new ChatResponseUpdate { Role = ExtChatRole.Assistant, Text = text };
    }

    private List<OpenAI.Chat.ChatMessage> BuildAzureChatMessages(ChatContext context)
    {
        var list = new List<OpenAI.Chat.ChatMessage> { new SystemChatMessage(SystemPrimaryPrompt) };

        foreach (var sr in context.Data)
        {
            if (sr.ContentType == ContentType.Image && sr.ContentBytes != null)
            {
                var mimeType = SearchResult.ContentSubTypeToMimeType(sr.ContentSubType);
                var imagePart = ChatMessageContentPart.CreateImagePart(new BinaryData(sr.ContentBytes), mimeType);
                list.Add(new UserChatMessage(imagePart));
            }
            else if (sr.Content != null)
            {
                list.Add(new UserChatMessage(sr.Content));
            }
        }

        list.Add(new SystemChatMessage(SystemSecondaryPrompt));

        foreach (var h in context.ChatHistory)
        {
            if (string.IsNullOrEmpty(h.Message)) continue;
            list.Add(h.Participant == AppChatRole.User
                ? new UserChatMessage(h.Message)
                : new AssistantChatMessage(h.Message));
        }

        return list;
    }
}