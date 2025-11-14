// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Notes.ViewModels;
using System.Linq;
using System.Threading.Tasks;
using Windows.System;

namespace Notes.Controls;

public sealed partial class ChatSessionView : UserControl
{
    public ChatSessionViewModel? ChatSessionViewModel { get; private set; }

    public ChatSessionView()
    {
        this.InitializeComponent();

        siSend.Click += SiSend_Click;
        txtRequest.KeyDown += TxtRequest_KeyDown;
    }

    public void InitializeChatSessionViewModel()
    {   
        if (ChatSessionViewModel == null)
        {
            ChatSessionViewModel = new ChatSessionViewModel();
        }
    }   

    private void SiSend_Click(object sender, RoutedEventArgs e)
    {
        var message = txtRequest.Text;
        txtRequest.Text = string.Empty;

        Task.Run(() =>
        {
            _ = SendRequest(message);
        });
    }

    private void TxtRequest_KeyDown(object sender, KeyRoutedEventArgs e)
    {
        if (e.Key == VirtualKey.Enter)
        {
            var message = txtRequest.Text;
            txtRequest.Text = string.Empty;

            Task.Run(() =>
            {
                _ = SendRequest(message);
            });
        }
    }

    public async Task ClearChatHistory()
    {
        if (ChatSessionViewModel != null)
        {
            await ChatSessionViewModel.ClearChatHistory();
        }
    }

    private async void clearChatButton_Click(object sender, RoutedEventArgs e)
    {
        await ClearChatHistory();
    }

    private async Task<bool> SendRequest(string message)
    {
        if (!string.IsNullOrEmpty(message) && ChatSessionViewModel != null)
        {
            return await ChatSessionViewModel.SendRequest(message);
        }

        return false;
    }

    private async void OnReferenceClick(object sender, RoutedEventArgs e)
    {
        var context = await AppDataContext.GetCurrentAsync();
        var button = sender as Button;

        if (button?.Tag is SearchResult item && MainWindow.Instance != null)
        {
            if (item.ContentType == ContentType.Note)
            {
                await MainWindow.Instance.SelectNoteById(item.SourceId);
            }
            else
            {
                var attachment = context.Attachments.Where(a => a.Id == item.SourceId).FirstOrDefault();

                if (attachment != null)
                {
                    var note = context.Notes.Where(n => n.Id == attachment.NoteId).FirstOrDefault();

                    if (note != null)
                    {
                        await MainWindow.Instance.SelectNoteById(note.Id, attachment.Id, item.MostRelevantSentence);
                    }
                }
            }
        }
    }
}
