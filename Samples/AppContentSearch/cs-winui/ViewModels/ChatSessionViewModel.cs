// Copyright (c) Microsoft Corporation. All rights reserved.

using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;

namespace Notes.ViewModels;

public enum ChatRole
{
    System,
    User,
    Assistant
}

public struct IndexerResponse
{
    public IndexerResponse()
    {
        ResponseData = new List<string>();
        ReferencedFiles = new List<string>();
    }

    public IndexerResponse(List<string> responseData, List<string> referencedFiles)
    {
        ResponseData = responseData;
        ReferencedFiles = referencedFiles;
    }

    public List<string> ResponseData { get; set; }
    public List<string> ReferencedFiles { get; set; }
}

public struct ChatContext
{
    public ChatContext(List<SearchResult> data, ObservableCollection<SessionEntryViewModel> chatHistory)
    {
        Data = data;
        ChatHistory = chatHistory;
    }

    public List<SearchResult> Data { get; set; }
    public ObservableCollection<SessionEntryViewModel> ChatHistory { get; set; }
}

public class ChatSessionViewModel
{
    public ObservableCollection<SessionEntryViewModel> SessionEntryViewModels;

    public Dictionary<ChatRole, Brush> _backgroundBrush = new Dictionary<ChatRole, Brush>();
    public Dictionary<ChatRole, Brush> _foregroundBrush = new Dictionary<ChatRole, Brush>();

    public static Microsoft.UI.Dispatching.DispatcherQueue? _dispatcherQueue { get; set; }

    private LanguageModelManager _languageModelManager = new LanguageModelManager();

    public ChatSessionViewModel()
    {
        SessionEntryViewModels = new ObservableCollection<SessionEntryViewModel>();

        if (_dispatcherQueue == null)
        {
            // Get the DispatcherQueue for the UI thread
            _dispatcherQueue = Microsoft.UI.Dispatching.DispatcherQueue.GetForCurrentThread();
        }

        _backgroundBrush.Add(key: ChatRole.User, value: (SolidColorBrush)Application.Current.Resources["SubtleFillColorSecondaryBrush"]);
        _backgroundBrush.Add(key: ChatRole.Assistant, value: new SolidColorBrush(Colors.Transparent));

        _foregroundBrush.Add(key: ChatRole.User, value: (SolidColorBrush)Application.Current.Resources["TextFillColorPrimaryBrush"]);
        _foregroundBrush.Add(key: ChatRole.Assistant, value: (SolidColorBrush)Application.Current.Resources["TextFillColorPrimaryBrush"]);
    }

    public async Task<bool> SendRequest(string message)
    {
        if (MainWindow.appContentIndexer == null) return false;

        _dispatcherQueue?.TryEnqueue(() => SessionEntryViewModels.Add(
            new SessionEntryViewModel(message, ChatRole.User, _backgroundBrush[ChatRole.User], _foregroundBrush[ChatRole.User])));

        var foundSources = await Utils.SearchAsync(MainWindow.appContentIndexer, message, top: 5);
        if (foundSources == null) return true;

        var chatContext = new ChatContext(foundSources, SessionEntryViewModels);
        var referencedFiles = new ObservableCollection<SearchResult>(foundSources.DistinctBy(r => r.SourceId));

        var assistantEntry = new SessionEntryViewModel(
            string.Empty,
            ChatRole.Assistant,
            _backgroundBrush[ChatRole.Assistant],
            _foregroundBrush[ChatRole.Assistant],
            null);

        _dispatcherQueue?.TryEnqueue(() => SessionEntryViewModels.Add(assistantEntry));

        try
        {
            var delta = String.Empty;
            await foreach (var update in _languageModelManager.SendRequest(message, chatContext, assistantEntry))
            {
                delta = update?.Text;
                if (string.IsNullOrEmpty(delta) && update?.Contents != null)
                {
                    // Build only this chunk (avoid building the full accumulated string each iteration)
                    delta = string.Concat(update.Contents
                        .Select(c => c.RawRepresentation?.ToString())
                        .Where(t => !string.IsNullOrEmpty(t)));
                }
                if (string.IsNullOrEmpty(delta)) continue;

                _dispatcherQueue?.TryEnqueue(() => assistantEntry.Message += delta);
                delta = String.Empty;
            }
        }
        catch (Exception ex)
        {
            _dispatcherQueue?.TryEnqueue(() =>
                assistantEntry.Message = string.IsNullOrWhiteSpace(assistantEntry.Message)
                    ? $"[Error] {ex.Message}"
                    : $"{assistantEntry.Message}\n\n[Error] {ex.Message}");
        }
        _dispatcherQueue?.TryEnqueue(() =>
        {
            foreach (var file in referencedFiles)
            {
                assistantEntry.ReferencedFiles.Add(file);
            }
        });
        return true;
    }
    public Task ClearChatHistory()
    {
        SessionEntryViewModels.Clear();
        return Task.CompletedTask;
    }
}

public partial class SessionEntryViewModel : ObservableObject
{
    [ObservableProperty]
    private string message;

    public SessionEntryViewModel(
        string message,
        ChatRole participant,
        Brush backgroundBrush,
        Brush foregroundBrush,
        ObservableCollection<SearchResult>? referencedFiles = null)
    {
        this.message = message ?? string.Empty;
        Participant = participant;
        BackgroundBrush = backgroundBrush;
        ForegroundBrush = foregroundBrush;
        ReferencedFiles = referencedFiles ?? new ObservableCollection<SearchResult>();
        HorizontalAlignment = participant == ChatRole.User ? HorizontalAlignment.Right : HorizontalAlignment.Left;
        Margin = new Thickness(participant == ChatRole.User ? 64 : 0, 4, 0, 0);
    }

    public ChatRole Participant { get; }
    public Brush BackgroundBrush { get; }
    public Brush ForegroundBrush { get; }
    public HorizontalAlignment HorizontalAlignment { get; }
    public Thickness Margin { get; }
    public ObservableCollection<SearchResult> ReferencedFiles { get; }
}