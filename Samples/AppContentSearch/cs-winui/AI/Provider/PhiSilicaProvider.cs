// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.Extensions.AI;
using Notes.ViewModels;
using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using AppChatRole = Notes.ViewModels.ChatRole;
using ExtChatRole = Microsoft.Extensions.AI.ChatRole;

namespace Notes.AI;

public class PhiSilicaProvider : ILanguageModelProvider
{
    private PhiSilicaClient? _phiSilicaClient;
    private Task<PhiSilicaClient?>? _phiInitTask;

    private const string SystemPrimaryPrompt =
        "You are a helpful assistant. Your output should be ONLY in plain text. There should be NO markdown elements.";
    private const string SystemSecondaryPrompt =
        "This is the end of the document snippets.\r\nPlease answer the following user question:\r\n";

    public PhiSilicaProvider()
    {
        // Initialize the task when the provider is created
        if (IsAvailable)
        {
            _phiInitTask = PhiSilicaClient.CreateAsync();
        }
    }

    public string Name => "Phi Silica";

    public bool IsAvailable => PhiSilicaClient.IsAvailable();

    public async Task<bool> InitializeAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            await EnsurePhiReadyAsync(cancellationToken);
            return _phiSilicaClient != null;
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
            await EnsurePhiReadyAsync(cancellationToken);
        }
        catch (Exception ex)
        {
            initError = $"Phi model init failed: {ex.Message}";
        }

        if (initError != null)
        {
            yield return Append(entry, initError);
            yield break;
        }

        var chatMessages = BuildPhiChatMessages(context);

        IAsyncEnumerable<ChatResponseUpdate>? stream = null;
        string? startError = null;
        try
        {
            stream = _phiSilicaClient!.GetStreamingResponseAsync(chatMessages, options: null, cancellationToken);
        }
        catch (Exception ex)
        {
            startError = $"Generation start failed: {ex.Message}";
        }

        if (startError != null)
        {
            yield return Append(entry, startError);
            yield break;
        }

        bool canceled = false;
        Exception? streamEx = null;

        await using (var enumerator = stream!.GetAsyncEnumerator(cancellationToken))
        {
            while (true)
            {
                bool hasNext;
                try
                {
                    hasNext = await enumerator.MoveNextAsync();
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

                var update = enumerator.Current;
                if (!string.IsNullOrEmpty(update.Text))
                {
                    yield return Append(entry, update.Text, update);
                }
            }
        }

        if (canceled)
        {
            yield return Append(entry, "[Canceled]");
        }
        else if (streamEx != null)
        {
            yield return Append(entry, $"Phi streaming error: {streamEx.Message}");
        }
    }

    private async Task EnsurePhiReadyAsync(CancellationToken ct)
    {
        if (_phiSilicaClient != null) return;

        _phiSilicaClient = await (_phiInitTask ?? PhiSilicaClient.CreateAsync(ct));
        if (_phiSilicaClient == null)
        {
            throw new InvalidOperationException("PhiSilica client could not be initialized.");
        }
    }

    private ChatResponseUpdate Append(SessionEntryViewModel entry, string text, ChatResponseUpdate? original = null)
    {
        ChatSessionViewModel._dispatcherQueue?.TryEnqueue(() => entry.Message += text);
        return original ?? new ChatResponseUpdate { Role = ExtChatRole.Assistant, Text = text };
    }

    private IList<Microsoft.Extensions.AI.ChatMessage> BuildPhiChatMessages(ChatContext context)
    {
        var combinedSystemPrompt = $"{SystemPrimaryPrompt}\r\n{SystemSecondaryPrompt}";
        var list = new List<Microsoft.Extensions.AI.ChatMessage>
        {
            new(ExtChatRole.System, combinedSystemPrompt)
        };

        foreach (var sr in context.Data)
        {
            if (sr.ContentType == ContentType.Image && sr.ContentBytes != null)
            {
                var title = string.IsNullOrWhiteSpace(sr.Title) ? "image" : sr.Title;
                list.Add(new(ExtChatRole.User, $"[Image: {title}]"));
            }
            else if (!string.IsNullOrEmpty(sr.Content))
            {
                list.Add(new(ExtChatRole.User, sr.Content));
            }
        }

        foreach (var h in context.ChatHistory)
        {
            if (string.IsNullOrEmpty(h.Message)) continue;
            var role = h.Participant == AppChatRole.User ? ExtChatRole.User : ExtChatRole.Assistant;
            list.Add(new(role, h.Message));
        }

        return list;
    }
}