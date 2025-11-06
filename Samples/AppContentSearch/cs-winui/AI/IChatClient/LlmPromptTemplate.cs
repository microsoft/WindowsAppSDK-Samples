// Copyright (c) Microsoft Corporation. All rights reserved.

namespace Notes.AI;

internal class LlmPromptTemplate
{
    public string? System { get; init; }
    public string? User { get; init; }
    public string? Assistant { get; init; }
    public string[]? Stop { get; init; }
}