// Copyright (c) Microsoft Corporation. All rights reserved.

using CommunityToolkit.Mvvm.ComponentModel;
using Notes.Models;
using System.Diagnostics;

namespace Notes.ViewModels
{
    public partial class AttachmentViewModel : ObservableObject
    {
        public readonly Attachment Attachment;

        [ObservableProperty]
        private bool isProcessing;

        [ObservableProperty]
        private float processingProgress;

        public AttachmentViewModel(Attachment attachment)
        {
            Attachment = attachment;
            if (!attachment.IsProcessed)
            {
                AttachmentProcessor.AttachmentProcessed += AttachmentProcessor_AttachmentProcessed;
                ProcessingProgress = 0;
            }
            else
            {
                ProcessingProgress = 1;
            }
            IsProcessing = !attachment.IsProcessed;
        }

        private void AttachmentProcessor_AttachmentProcessed(object? sender, AttachmentProcessedEventArgs e)
        {
            if (e.AttachmentId == Attachment.Id && MainWindow.Instance != null)
            {
                MainWindow.Instance.DispatcherQueue.TryEnqueue(() =>
                {
                    IsProcessing = e.Progress < 1;
                    ProcessingProgress = e.Progress * 100;
                });

                Debug.WriteLine($"Attachment {Attachment.Id}, step: {e.ProcessingStep}, processed: {e.Progress * 100}%");
            }
        }
    }
}
