// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.AI.Foundry.Local;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.AI.Search.Experimental.AppContentIndex;
using Notes.Controls;
using Notes.Pages;
using Notes.ViewModels;
using OpenAI;
using System;
using System.ClientModel;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;

namespace Notes
{
    public sealed partial class MainWindow : Window
    {
        public static ChatSessionView? ChatSessionView;
        public static SearchView? SearchView;
        public static MainWindow? Instance;
        public static AppContentIndexer? appContentIndexer;
        public ViewModel VM;

        private Task _initializeAppContentIndexerTask;

        public MainWindow()
        {
            VM = new ViewModel();
            this.InitializeComponent();

            this.ExtendsContentIntoTitleBar = true;
            this.SetTitleBar(AppTitleBar);

            this.Title = AppTitleBar.Title;
            this.AppWindow.SetIcon("Assets/TextPad.ico");

            Instance = this;
            SearchView = searchView;
            ChatSessionView = chatSessionView;

            VM.Notes.CollectionChanged += Notes_CollectionChanged;

            _initializeAppContentIndexerTask = InitializeAppContentIndexerAsync();

            DispatcherQueue.TryEnqueue(async () =>
            {
                SearchView?.SetSearchBoxInitializing();

                try
                {
                    await _initializeAppContentIndexerTask;
                }
                catch (Exception ex)
                {
                    Debug.WriteLine("Indexer init failed: " + ex);
                    // Inspect ex.HResult, Message, InnerException
                }
            });
        }

        public async Task SelectNoteById(int id, int? attachmentId = null, string? attachmentText = null, Windows.Foundation.Rect? boundingBox = null)
        {
            var note = VM.Notes.Where(n => n.Note.Id == id).FirstOrDefault();

            if (note != null)
            {
                navView.SelectedItem = note;

                if (attachmentId.HasValue)
                {
                    var attachmentViewModel = note.Attachments.Where(a => a.Attachment.Id == attachmentId).FirstOrDefault();
                    if (attachmentViewModel == null)
                    {
                        var context = await AppDataContext.GetCurrentAsync();
                        var attachment = context.Attachments.Where(a => a.Id == attachmentId.Value).FirstOrDefault();
                        if (attachment == null)
                        {
                            return;
                        }

                        attachmentViewModel = new AttachmentViewModel(attachment);
                    }

                    OpenAttachmentView(attachmentViewModel, attachmentText, boundingBox);
                }
            }
        }

        private async Task InitializeAppContentIndexerAsync()
        {
            GetOrCreateIndexResult? getOrCreateResult = null;
            await Task.Run(() =>
            {
                getOrCreateResult = AppContentIndexer.GetOrCreateIndex("NotesIndex");
                if (getOrCreateResult == null)
                {
                    throw new Exception("GetOrCreateIndexResult is null");
                }
                if (!getOrCreateResult.Succeeded)
                {
                    throw getOrCreateResult.ExtendedError;
                }

                appContentIndexer = getOrCreateResult.Indexer;
            });

            DispatcherQueue.TryEnqueue(() =>
            {
                ChatPaneToggleButton.IsEnabled = true;
                searchView.SetSearchBoxInitializingCompleted();

                var status = getOrCreateResult?.Status;

                switch (status)
                {
                    case GetOrCreateIndexStatus.CreatedNew:
                        _ = indexAll();
                        break;
                    case GetOrCreateIndexStatus.OpenedExisting:
                        searchView.SetSearchBoxIndexingCompleted();
                        break;
                    default:
                        searchView.SetSearchBoxInitializingCompleted();
                        break;
                }
            });
        }

        private void NavView_Loaded(object sender, RoutedEventArgs e)
        {
            if (navView.MenuItems.Count > 0)
                navView.SelectedItem = navView.MenuItems[0];
        }

        private void Notes_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            if (navView.SelectedItem == null && VM.Notes.Count > 0)
                navView.SelectedItem = VM.Notes[0];
        }

        private async void NewButton_Click(object sender, RoutedEventArgs e)
        {
            var note = await VM.CreateNewNote();
            navView.SelectedItem = note;
        }

        private void NavView_SelectionChanged(NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                navFrame.Navigate(typeof(Notes.Pages.SettingsPage));
                return;
            }
            if (args.SelectedItem is NoteViewModel note)
            {
                navFrame.Navigate(typeof(NotesPage), note);
            }
        }

        private async void DeleteMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var note = (sender as FrameworkElement)?.Tag as NoteViewModel;

            if (VM != null && note != null)
            {
                await VM.RemoveNoteAsync(note);
            }

            if (navFrame.CurrentSourcePageType == typeof(NotesPage) && navFrame.CanGoBack)
            {
                navFrame.GoBack();
            }
        }

        private void ToggleChatPane()
        {
            chatSessionView.Visibility =
                chatSessionView.Visibility == Visibility.Visible ?
                Visibility.Collapsed :
                Visibility.Visible;
        }

        private void AskMyNotesClicked(object sender, RoutedEventArgs e)
        {
            ToggleChatPane();

            if (ChatSessionView?.ChatSessionViewModel == null)
            {
                ChatSessionView?.InitializeChatSessionViewModel();
            }
        }

        public async void OpenAttachmentView(AttachmentViewModel attachment, string? attachmentText = null, Windows.Foundation.Rect? boundingBox = null)
        {
            await attachmentView.UpdateAttachment(attachment, attachmentText, boundingBox);
            attachmentView.Show();
        }

        private async Task indexAll()
        {
            SearchView?.SetSearchBoxInitializingCompleted();

            var notes = this.VM.Notes;
            int total = notes.Count;

            var progress = new Progress<double>(percent =>
            {
                SearchView?.SetIndexProgressBar(percent);
                Debug.WriteLine($"Indexing progress: {percent:F2}%");
            });

            // Add all the files to the staging phase of the index.
            int index = 0;
            foreach (var note in notes)
            {
                index++;
                double percent = (double)index / total * 100;
                ((IProgress<double>)progress).Report(percent);

                await note.ReindexAsync();
            }

            // After adding all files to the staging phase of the index, we wait until the index fully completes.
            // Results may be partial during the staging phase.
            SearchView?.StartIndexProgressBarStaging();

            if (appContentIndexer != null)
            {
                await appContentIndexer.WaitForIndexingIdleAsync(int.MaxValue);
            }

            // Indexing is fully completed.
            SearchView?.SetSearchBoxIndexingCompleted();
        }

        private async void indexButton_Click(object sender, RoutedEventArgs e)
        {
            if (appContentIndexer != null)
            {
                await indexAll();
            }
        }

        private async void deleteIndexButton_Click(object sender, RoutedEventArgs e)
        {
            await NoteViewModel.ManualDeleteIndex();
        }
    }

    internal class MenuItemTemplateSelector : DataTemplateSelector
    {
        public DataTemplate? NoteTemplate { get; set; }
        public DataTemplate? DefaultTemplate { get; set; }

        protected override DataTemplate? SelectTemplateCore(object item)
        {
            return item is NoteViewModel ? NoteTemplate : DefaultTemplate;
        }
    }
}
