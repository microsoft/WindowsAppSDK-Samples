// Copyright (c) Microsoft Corporation. All rights reserved.

using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.Extensions.AI;
using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.Windows.AI.Search.Experimental.AppContentIndex;
using Notes.Models;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Storage;

namespace Notes.ViewModels
{
    public partial class NoteViewModel : ObservableObject
    {
        public readonly Note Note;

        [ObservableProperty]
        private ObservableCollection<AttachmentViewModel> attachments = new();

        [ObservableProperty]
        private ObservableCollection<string> todos = new();

        [ObservableProperty]
        private bool todosLoading = false;

        [ObservableProperty]
        private string content = string.Empty;

        private DispatcherTimer _saveTimer;
        private bool _contentLoaded = false;

        public DispatcherQueue? DispatcherQueue { get; set; }

        public NoteViewModel(Note note)
        {
            Note = note;
            _saveTimer = new DispatcherTimer();
            _saveTimer.Interval = TimeSpan.FromSeconds(5);
            _saveTimer.Tick += SaveTimerTick;
        }

        public string? Title
        {
            get => Note.Title;
            set => SetProperty(Note.Title, value, Note, (note, value) =>
            {
                note.Title = value;
                _ = HandleTitleChanged(value);
            });
        }

        public DateTime Modified
        {
            get => Note.Modified;
            set => SetProperty(Note.Modified, value, Note, (note, value) => note.Modified = value);
        }

        private async Task HandleTitleChanged(string? value)
        {
            if (value != null)
            {
                var folder = await Utils.GetLocalFolderAsync();
                var file = await folder.GetFileAsync(Note.Filename);

                await file.RenameAsync(value.Trim() + Utils.FileExtension, NameCollisionOption.GenerateUniqueName);
                Note.Filename = file.Name;
                await AppDataContext.SaveCurrentAsync();

                _ = SaveContentToFileAndReIndex();
            }
        }

        public async Task LoadContentAsync()
        {
            if (_contentLoaded)
            {
                return;
            }

            _contentLoaded = true;

            var folder = await Utils.GetLocalFolderAsync();
            var file = await folder.GetFileAsync(Note.Filename);
            Content = await FileIO.ReadTextAsync(file);

            var context = await AppDataContext.GetCurrentAsync();
            var attachments = context.Attachments.Where(a => a.NoteId == Note.Id).ToList();
            foreach (var attachment in attachments)
            {
                Attachments.Add(new AttachmentViewModel(attachment));
            }
        }

        public static async Task<string> LoadTextContentByIdAsync(string contentId)
        { 
            var folder = await Utils.GetLocalFolderAsync();
            var file = await folder.GetFileAsync(contentId);
            var returnedContent = await FileIO.ReadTextAsync(file);

            return returnedContent;
        }

        public static async Task<byte[]> LoadImageContentByIdAsync(string contentId)
        {
            var folder = await Utils.GetAttachmentsFolderAsync();
            var file = await folder.GetFileAsync(contentId);
            byte[] imageBytes = System.IO.File.ReadAllBytes(file.Path);
            
            // FYI, the base64 string representation would be str = Convert.ToBase64String(imageBytes);
            return imageBytes;
        }

        partial void OnContentChanged(string value)
        {
            _saveTimer.Stop();
            _saveTimer.Start();
        }

        public async Task AddAttachmentAsync(StorageFile file)
        {
            var attachmentsFolder = await Utils.GetAttachmentsFolderAsync();
            bool shouldCopyFile = true;

            var attachment = new Attachment()
            {
                IsProcessed = false,
                Note = Note
            };

            if (new string[] { ".png", ".jpg", ".jpeg", ".bmp"}.Contains(file.FileType.ToLower()))
            {
                attachment.Type = NoteAttachmentType.Image;
            }
            else if (new string[] { ".mp3", ".wav", ".m4a", ".opus", ".waptt" }.Contains(file.FileType))
            {
                attachment.Type = NoteAttachmentType.Audio;
                shouldCopyFile = false;
                throw new NotSupportedException("audio files are not supported");                
            }
            else if (file.FileType == ".mp4")
            {
                attachment.Type = NoteAttachmentType.Video;
            }
            else
            {
                attachment.Type = NoteAttachmentType.Document;
            }

            if (shouldCopyFile && !file.Path.StartsWith(attachmentsFolder.Path))
            {
                file = await file.CopyAsync(attachmentsFolder, file.Name, NameCollisionOption.GenerateUniqueName);
            }

            attachment.Filename = file.Name;
            attachment.RelativePath = file.Name; // The relative path is attachmentsFolder.Path + file.Name

            Attachments.Add(new AttachmentViewModel(attachment));

            var context = await AppDataContext.GetCurrentAsync();

            await context.Attachments.AddAsync(attachment);
            await context.SaveChangesAsync();
            await AttachmentProcessor.AddAttachment(attachment);
        }

        public async Task AddAttachmentAsync(SoftwareBitmap bitmap)
        {
            // save bitmap to file
            var attachmentsFolder = await Utils.GetAttachmentsFolderAsync();
            var file = await attachmentsFolder.CreateFileAsync(Guid.NewGuid().ToString() + ".png", CreationCollisionOption.GenerateUniqueName);

            using (var stream = await file.OpenAsync(FileAccessMode.ReadWrite))
            {
                var encoder = await BitmapEncoder.CreateAsync(BitmapEncoder.PngEncoderId, stream);
                encoder.SetSoftwareBitmap(bitmap);
                await encoder.FlushAsync();
            }

            await AddAttachmentAsync(file);
        }

        public async Task RemoveAttachmentAsync(AttachmentViewModel attachmentViewModel)
        {
            Attachments.Remove(attachmentViewModel);

            var attachment = attachmentViewModel.Attachment;
            Note.Attachments.Remove(attachment);

            var attachmentsFolder = await Utils.GetAttachmentsFolderAsync();
            var file = await attachmentsFolder.GetFileAsync(attachment.Filename);
            await file.DeleteAsync();

            if (attachment.IsProcessed && !string.IsNullOrEmpty(attachment.FilenameForText))
            {
                var attachmentsTranscriptFolder = await Utils.GetAttachmentsTranscriptsFolderAsync();
                var transcriptFile = await attachmentsTranscriptFolder.GetFileAsync(attachment.FilenameForText);
                await transcriptFile.DeleteAsync();
            }

            var context = await AppDataContext.GetCurrentAsync();
            context.Attachments.Remove(attachment);            

            await context.SaveChangesAsync();
            await AttachmentProcessor.RemoveAttachment(attachment);
        }

        public async Task RemoveNoteFromIndexAsync()
        {
            if (MainWindow.appContentIndexer != null && Note != null)
            {
                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.Remove(Note.Id.ToString());
                });
                Debug.WriteLine($"Deleted note from index: {Note.Filename}");
            }
            else
            {
                Debug.Write("appContentIndexer is null");
            }
        }

        public async Task ManualSaveAndIndex()
        {
            if (MainWindow.SearchView != null)
            {
                MainWindow.SearchView.StartIndexProgressBarStaging();
                await SaveContentToFileDeleteAndReIndex();

                MainWindow.SearchView.SetSearchBoxIndexingCompleted();
            }
        }

        public async static Task ManualDeleteIndex()
        {
            if (MainWindow.appContentIndexer != null)
            {
                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.RemoveAll();
                });
                Debug.WriteLine($"Deleted Index");
            }
            else
            {
                throw new InvalidOperationException("AppContentIndexer is null");
            }
        }

        private void SaveTimerTick(object? sender, object e)
        {
            _saveTimer.Stop();
            _ = SaveContentToFileAndReIndex();
        }

        public async Task ReindexAsync()
        {
            var folder = await Utils.GetLocalFolderAsync();
            var file = await folder.GetFileAsync(Note.Filename);

            await this.LoadContentAsync();

            IndexableAppContent textContent = AppManagedIndexableAppContent.CreateFromString(Note.Id.ToString(), Title + Content);
            Debug.WriteLine($"Indexing note {Note.Title}");

            if (MainWindow.appContentIndexer != null)
            {
                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.AddOrUpdate(textContent);
                });
                Debug.WriteLine($"Indexed note {Note.Title}");
            }
            else
            {
                throw new InvalidOperationException("AppContentIndexer is null");
            }

            foreach (var attachment in this.Attachments)
            {
                Debug.WriteLine($"ReIndexing attachment {attachment.Attachment.Filename}");
                await AttachmentProcessor.ReIndexImage(attachment.Attachment);
            }
        }

        private async Task SaveContentToFileAndReIndex()
        {
            var folder = await Utils.GetLocalFolderAsync();
            var file = await folder.GetFileAsync(Note.Filename);

            Debug.WriteLine("Saving note " + Note.Title + " to filename " + Note.Filename);
            await FileIO.WriteTextAsync(file, Content);

            IndexableAppContent textContent = AppManagedIndexableAppContent.CreateFromString(Note.Id.ToString(), Title + Content);
            Debug.WriteLine($"Indexing note {Note.Title}");

            if (MainWindow.appContentIndexer != null)
            {
                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.AddOrUpdate(textContent);
                });
                Debug.WriteLine($"Indexed note {Note.Title}");
            }
            else
            {
                Debug.WriteLine("AppContentIndexer is null");
            }
        }

        private async Task SaveContentToFileDeleteAndReIndex()
        {
            if (MainWindow.appContentIndexer != null)
            {
                var folder = await Utils.GetLocalFolderAsync();
                var file = await folder.GetFileAsync(Note.Filename);

                Debug.WriteLine("Saving note " + Note.Title + " to filename " + Note.Filename);
                await FileIO.WriteTextAsync(file, Content);

                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.Remove(Note.Id.ToString());
                });
                Debug.WriteLine($"Deleted note {Note.Title}");

                IndexableAppContent textContent = AppManagedIndexableAppContent.CreateFromString(Note.Id.ToString(), Title + Content);
                Debug.WriteLine($"Indexing note {Note.Title}");

                await Task.Run(() =>
                {
                    MainWindow.appContentIndexer.AddOrUpdate(textContent);
                });
                Debug.WriteLine($"Indexed note {Note.Title}");

                foreach (var attachment in Attachments)
                {
                    Debug.WriteLine($"ReIndexing attachment {attachment.Attachment.Filename}");
                    await AttachmentProcessor.ReIndexImage(attachment.Attachment);
                }
            }
            else
            {
                Debug.WriteLine("AppContentIndexer is null");
            }
        }

    }
}
