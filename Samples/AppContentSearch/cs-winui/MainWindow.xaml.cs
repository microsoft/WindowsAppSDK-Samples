// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.AI.Search.Experimental.AppContentIndex;
using Notes.Controls;
using Notes.Pages;
using Notes.ViewModels;
using System;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;

namespace Notes
{
    public sealed partial class MainWindow : Window
    {
        private static ChatSessionView? _chatSessionView;
        private static SearchView? _searchView;
        private static MainWindow? _instance;
        private static AppContentIndexer? _appContentIndexer;

        public static ChatSessionView? ChatSessionView => _chatSessionView;
        public static SearchView? SearchView => _searchView;
        public static MainWindow? Instance => _instance;
        public static AppContentIndexer? AppContentIndexer => _appContentIndexer;

        public ViewModel VM;

        private readonly Task _initializeAppContentIndexerTask;

        public MainWindow()
        {
            VM = new ViewModel();
            this.InitializeComponent();

            this.ExtendsContentIntoTitleBar = true;
            this.SetTitleBar(AppTitleBar);

            this.Title = AppTitleBar.Title;
            this.AppWindow.SetIcon("Assets/ContosoNote.ico");

            _instance = this;
            _searchView = AppSearchView;
            _chatSessionView = AppChatSessionView;

            VM.Notes.CollectionChanged += Notes_CollectionChanged;

            _initializeAppContentIndexerTask = InitializeAppContentIndexerAsync();

            DispatcherQueue.TryEnqueue(async () =>
            {
                _searchView?.SetSearchBoxInitializing();

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
                NavView.SelectedItem = note;

                if (attachmentId.HasValue)
                {
                    var attachmentViewModel = note.Attachments.Where(a => a.Attachment.Id == attachmentId.Value).FirstOrDefault();
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
                getOrCreateResult = Microsoft.Windows.AI.Search.Experimental.AppContentIndex.AppContentIndexer.GetOrCreateIndex("NotesIndex");
                if (getOrCreateResult == null)
                {
                    throw new Exception("GetOrCreateIndexResult is null");
                }
                if (!getOrCreateResult.Succeeded)
                {
                    throw getOrCreateResult.ExtendedError;
                }

                _appContentIndexer = getOrCreateResult.Indexer;
            });

            DispatcherQueue.TryEnqueue(() =>
            {
                ChatPaneToggleButton.IsEnabled = true;
                AppSearchView.SetSearchBoxInitializingCompleted();

                var status = getOrCreateResult?.Status;

                switch (status)
                {
                    case GetOrCreateIndexStatus.CreatedNew:
                        _ = IndexAllAsync();
                        break;
                    case GetOrCreateIndexStatus.OpenedExisting:
                        AppSearchView.SetSearchBoxIndexingCompleted();
                        break;
                    default:
                        AppSearchView.SetSearchBoxInitializingCompleted();
                        break;
                }
            });
        }

        private void NavView_Loaded(object sender, RoutedEventArgs e)
        {
            if (NavView.MenuItems.Count > 0)
                NavView.SelectedItem = NavView.MenuItems[0];
        }

        private void Notes_CollectionChanged(object? sender, NotifyCollectionChangedEventArgs e)
        {
            if (NavView.SelectedItem == null && VM.Notes.Count > 0)
                NavView.SelectedItem = VM.Notes[0];
        }

        private async void NewButton_Click(object sender, RoutedEventArgs e)
        {
            var note = await VM.CreateNewNote();
            NavView.SelectedItem = note;
        }

        private void NavView_SelectionChanged(NavigationView sender, Microsoft.UI.Xaml.Controls.NavigationViewSelectionChangedEventArgs args)
        {
            if (args.IsSettingsSelected)
            {
                NavFrame.Navigate(typeof(Notes.Pages.SettingsPage));
                return;
            }
            if (args.SelectedItem is NoteViewModel note)
            {
                NavFrame.Navigate(typeof(NotesPage), note);
            }
        }

        private async void DeleteMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var note = (sender as FrameworkElement)?.Tag as NoteViewModel;

            if (VM != null && note != null)
            {
                await VM.RemoveNoteAsync(note);
            }

            if (NavFrame.CurrentSourcePageType == typeof(NotesPage) && NavFrame.CanGoBack)
            {
                NavFrame.GoBack();
            }
        }

        private void ToggleChatPane()
        {
            AppChatSessionView.Visibility =
                AppChatSessionView.Visibility == Visibility.Visible ?
                Visibility.Collapsed :
                Visibility.Visible;
        }

        private void ChatPaneToggleButton_Click(object sender, RoutedEventArgs e)
        {
            ToggleChatPane();

            if (_chatSessionView?.ChatSessionViewModel == null)
            {
                _chatSessionView?.InitializeChatSessionViewModel();
            }
        }

        public async void OpenAttachmentView(AttachmentViewModel attachment, string? attachmentText = null, Windows.Foundation.Rect? boundingBox = null)
        {
            await AttachmentView.UpdateAttachment(attachment, attachmentText, boundingBox);
            AttachmentView.Show();
        }

        private async Task IndexAllAsync()
        {
            _searchView?.SetSearchBoxInitializingCompleted();

            var notes = this.VM.Notes;
            int total = notes.Count;

            var progress = new Progress<double>(percent =>
            {
                _searchView?.SetIndexProgressBar(percent);
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
            _searchView?.StartIndexProgressBarStaging();

            if (_appContentIndexer != null)
            {
                await _appContentIndexer.WaitForIndexingIdleAsync(TimeSpan.MaxValue);
            }

            // Indexing is fully completed.
            _searchView?.SetSearchBoxIndexingCompleted();
        }

        private async void IndexButton_Click(object sender, RoutedEventArgs e)
        {
            if (_appContentIndexer != null)
            {
                await IndexAllAsync();
            }
        }

        private async void DeleteIndexButton_Click(object sender, RoutedEventArgs e)
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
