// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.Extensions.AI;
using Notes.ViewModels;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace Notes.AI;

public interface ILanguageModelProvider
{
    string Name { get; }
    bool IsAvailable { get; }
    Task<bool> InitializeAsync(CancellationToken cancellationToken = default);
    IAsyncEnumerable<ChatResponseUpdate> SendStreamingRequestAsync(
        ChatContext context,
        SessionEntryViewModel entry,
        CancellationToken cancellationToken = default);
}