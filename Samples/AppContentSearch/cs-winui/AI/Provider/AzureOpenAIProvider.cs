// Copyright (c) Microsoft Corporation. All rights reserved.

using Azure.AI.OpenAI;
using Microsoft.Extensions.AI;
using Notes.ViewModels;
using OpenAI;
using OpenAI.Chat;
using System;
using System.ClientModel;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Threading;
using System.Threading.Tasks;
using Windows.Storage;
using AppChatRole = Notes.ViewModels.ChatRole;
using ExtChatRole = Microsoft.Extensions.AI.ChatRole;

namespace Notes.AI;

public class AzureOpenAIProvider : ILanguageModelProvider
{
    private AzureOpenAIClient? _azureClient;
    private ChatClient? _azureChatClient;

    private const string SystemPrimaryPrompt =
        "You are a helpful assistant. Your output should be ONLY in plain text. There should be NO markdown elements.";
    private const string SystemSecondaryPrompt =
        "This is the end of the document snippets.\r\nPlease answer the following user question:\r\n";

    public string Name => "Azure OpenAI";

    public bool IsAvailable
    {
        get
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            string? apiKey = localSettings.Values["AzureOpenAIApiKey"] as string;
            string? endpoint = localSettings.Values["AzureOpenAIEndpointUri"] as string;
            string? deployment = localSettings.Values["AzureOpenAIDeploymentName"] as string;

            return !string.IsNullOrEmpty(apiKey) && 
                   !string.IsNullOrEmpty(endpoint) && 
                   !string.IsNullOrEmpty(deployment);
        }
    }

    public Task<bool> InitializeAsync(CancellationToken cancellationToken = default)
    {
        try
        {
            EnsureAzureClient();
            return Task.FromResult(_azureClient != null && _azureChatClient != null);
        }
        catch
        {
            return Task.FromResult(false);
        }
    }

    public async IAsyncEnumerable<ChatResponseUpdate> SendStreamingRequestAsync(
        ChatContext context,
        SessionEntryViewModel entry,
        [EnumeratorCancellation] CancellationToken cancellationToken = default)
    {
        EnsureAzureClient();

        if (_azureClient == null || _azureChatClient == null)
        {
            yield return Append(entry, "Error: Azure client is uninitialized. Ensure you set API keys.");
            yield break;
        }

        var chatMessages = BuildAzureChatMessages(context);

        IAsyncEnumerable<StreamingChatCompletionUpdate>? stream = null;
        string? startError = null;
        try
        {
            stream = _azureChatClient.CompleteChatStreamingAsync(chatMessages);
        }
        catch (Exception ex)
        {
            startError = $"Azure start error: {ex.Message}";
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
                foreach (var part in update.ContentUpdate)
                {
                    if (!string.IsNullOrEmpty(part.Text))
                    {
                        yield return Append(entry, part.Text);
                    }
                }
            }
        }

        if (canceled)
        {
            yield return Append(entry, "[Canceled]");
        }
        else if (streamEx != null)
        {
            yield return Append(entry, $"Azure error: {streamEx.Message}");
        }
    }

    private void EnsureAzureClient()
    {
        if (_azureClient != null && _azureChatClient != null) return;

        try
        {
            var localSettings = ApplicationData.Current.LocalSettings;
            string? apiKey = localSettings.Values["AzureOpenAIApiKey"] as string;
            string? endpoint = localSettings.Values["AzureOpenAIEndpointUri"] as string;
            string? deployment = localSettings.Values["AzureOpenAIDeploymentName"] as string;

            if (string.IsNullOrEmpty(apiKey) ||
                string.IsNullOrEmpty(endpoint) ||
                string.IsNullOrEmpty(deployment))
            {
                Debug.WriteLine("Azure init - missing settings.");
                return;
            }

            _azureClient ??= new AzureOpenAIClient(new Uri(endpoint), new ApiKeyCredential(apiKey));
            _azureChatClient ??= _azureClient.GetChatClient(deployment);
        }
        catch (Exception ex)
        {
            Debug.WriteLine("CreateAzureClient - " + ex.Message);
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