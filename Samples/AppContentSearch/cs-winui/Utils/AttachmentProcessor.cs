// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.Extensions.AI;
using Microsoft.Windows.AI.Search.Experimental.AppContentIndex;
using Notes.Models;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Storage;
using Windows.Storage.Streams;

namespace Notes
{
    public static class AttachmentProcessor
    {
        public static EventHandler<AttachmentProcessedEventArgs>? AttachmentProcessed;

        private static List<Attachment> _toBeProcessed = new();
        private static bool _isProcessing = false;

        

        public async static Task AddAttachment(Attachment attachment)
        {
            _toBeProcessed.Add(attachment);

            if (!_isProcessing)
            {
                try
                {
                    _isProcessing = true;
                    await Process();
                }
                catch (Exception ex)
                {
                    Debug.WriteLine($"Error processing attachment: {ex.Message}");
                }
                _isProcessing = false;
            }
        }

        public async static Task RemoveAttachment(Attachment attachment)
        {
            if (MainWindow.appContentIndexer != null)
            {
                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.Remove(attachment.Id.ToString());
                });
                Debug.WriteLine($"Deleted image from index: {attachment.Filename}");
            }
            else
            {
                Debug.WriteLine("appContentIndexer is null");
            }
        }

        private static async Task Process()
        {
            while (_toBeProcessed.Count > 0)
            {
                var attachment = _toBeProcessed[0];
                _toBeProcessed.RemoveAt(0);

                if (attachment.IsProcessed)
                {
                    continue;
                }

                if (attachment.Type == NoteAttachmentType.Image)
                {
                    await ProcessImage(attachment);
                }
                else if (attachment.Type == NoteAttachmentType.Audio || attachment.Type == NoteAttachmentType.Video)
                {
                    throw new NotSupportedException("audio and video files are not supported");
                }
            }
        }

        public static async Task ReIndexImage(Attachment attachment)
        {
            // get softwarebitmap from file
            var attachmentsFolder = await Utils.GetAttachmentsFolderAsync();
            var file = await attachmentsFolder.GetFileAsync(attachment.Filename);

            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                SoftwareBitmap softwareBitmap = await decoder.GetSoftwareBitmapAsync();
                IndexableAppContent imageContent = AppManagedIndexableAppContent.CreateFromBitmap(attachment.Id.ToString(), softwareBitmap);

                Debug.WriteLine($"Indexing image {attachment.Filename}");

                if (MainWindow.appContentIndexer != null)
                {
                    await Task.Run(() =>
                    {
                        MainWindow.appContentIndexer.AddOrUpdate(imageContent);
                    });
                    attachment.IsProcessed = true;
                    Debug.WriteLine($"Indexed image {attachment.Filename}");
                }
                else
                {
                    Debug.WriteLine("appContentIndexer is null");
                }
            }
        }

        private static async Task ProcessImage(Models.Attachment attachment, EventHandler<float>? progress = null)
        {
            // get softwarebitmap from file
            var attachmentsFolder = await Utils.GetAttachmentsFolderAsync();
            var file = await attachmentsFolder.GetFileAsync(attachment.Filename);

            using (IRandomAccessStream stream = await file.OpenAsync(FileAccessMode.Read))
            {
                BitmapDecoder decoder = await BitmapDecoder.CreateAsync(stream);
                SoftwareBitmap softwareBitmap = await decoder.GetSoftwareBitmapAsync();
                IndexableAppContent imageContent = AppManagedIndexableAppContent.CreateFromBitmap(attachment.Id.ToString(), softwareBitmap);

                Debug.WriteLine($"Indexing image {attachment.Filename}");

                if (MainWindow.appContentIndexer != null)
                {
                    await Task.Run(() =>
                    {
                        MainWindow.appContentIndexer.AddOrUpdate(imageContent);
                    });
                    attachment.IsProcessed = true;
                    Debug.WriteLine($"Indexed image {attachment.Filename}");
                }
                else
                {
                    Debug.WriteLine("appContentIndexer is null");
                }

                InvokeAttachmentProcessedComplete(attachment);

                var context = await AppDataContext.GetCurrentAsync();
                context.Update(attachment);
                await context.SaveChangesAsync();
            }
        }

        private async static Task<string> SaveTextToFileAsync(string text, string filename)
        {
            var stateAttachmentsFolder = await Utils.GetAttachmentsTranscriptsFolderAsync();

            var file = await stateAttachmentsFolder.CreateFileAsync(filename, CreationCollisionOption.GenerateUniqueName);
            await FileIO.WriteTextAsync(file, text);
            return file.Name;
        }

        private static void InvokeAttachmentProcessedComplete(Attachment attachment)
        {
            if (AttachmentProcessed != null)
            {
                AttachmentProcessed.Invoke(null, new AttachmentProcessedEventArgs
                {
                    AttachmentId = attachment.Id,
                    Progress = 1,
                    ProcessingStep = "Complete"
                });
            }
        }
    }

    public class AttachmentProcessedEventArgs
    {
        public int AttachmentId { get; set; }
        public float Progress { get; set; }
        public string? ProcessingStep { get; set; }
    }
}
