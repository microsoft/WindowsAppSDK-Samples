// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.Windows.Search.AppContentIndex;
using Notes.Controls;
using Notes.Pages;
using Notes.ViewModels;
using System;
using System.Collections.Specialized;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Foundation.Collections;

namespace Notes
{
    public sealed partial class MainWindow : Window
    {
        private static ChatSessionView? _chatSessionView;
        private static SearchView? _searchView;
        private static MainWindow? _instance;
        private static AppContentIndexer? _appContentIndexer;
        private static IndexCapabilitiesOfCurrentSystem? _systemCapabilities;

        public static ChatSessionView? ChatSessionView => _chatSessionView;
        public static SearchView? SearchView => _searchView;
        public static MainWindow? Instance => _instance;
        public static AppContentIndexer? AppContentIndexer => _appContentIndexer;
        public static IndexCapabilitiesOfCurrentSystem? SystemCapabilities => _systemCapabilities;

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

            this.Closed += (_, _) =>
            {
                _appContentIndexer?.Dispose();
                _appContentIndexer = null;
            };

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

        private static void CheckSystemCapabilities()
        {
            _systemCapabilities = AppContentIndexer.GetIndexCapabilitiesOfCurrentSystem();

            var capabilities = new[]
            {
                IndexCapability.TextLexical,
                IndexCapability.TextSemantic,
                IndexCapability.ImageOcr,
                IndexCapability.ImageSemantic
            };

            foreach (var capability in capabilities)
            {
                var status = _systemCapabilities.GetIndexCapabilityStatus(capability);
                Debug.WriteLine($"System capability {capability}: {status}");

                if (status == IndexCapabilityOfCurrentSystemStatus.DisabledByPolicy)
                {
                    Debug.WriteLine($"Warning: {capability} is disabled by policy.");
                }
                else if (status == IndexCapabilityOfCurrentSystemStatus.NotSupported)
                {
                    Debug.WriteLine($"Warning: {capability} is not supported on this system.");
                }
            }
        }

        private void SubscribeToIndexerListener()
        {
            if (_appContentIndexer is null)
            {
                return;
            }

            var listener = _appContentIndexer.Listener;

            listener.IndexCapabilitiesChanged += (sender, capabilities) =>
            {
                if (capabilities.HasCapabilitiesWithErrors)
                {
                    var errors = capabilities.GetCapabilitiesWithErrors();
                    foreach (var cap in errors)
                    {
                        var state = capabilities.GetCapabilityState(cap);
                        Debug.WriteLine($"Index capability error — {cap}: {state.InitializationStatus}, {state.ErrorMessage}");
                    }
                }
                else
                {
                    Debug.WriteLine("All index capabilities are healthy.");
                }
            };

            listener.IndexStatisticsChanged += (sender, stats) =>
            {
                Debug.WriteLine($"Index statistics changed — Items: {stats.ItemCount}, " +
                    $"Completed: {stats.CompletedCount}, InProgress: {stats.InProgressCount}, " +
                    $"NotStarted: {stats.NotStartedCount}, Errors: {stats.ErrorsCount}, " +
                    $"PendingDeletion: {stats.PendingDeletionCount}, " +
                    $"RequiringReindexing: {stats.RequiringReindexingCount}");

                DispatcherQueue.TryEnqueue(() =>
                {
                    if (stats.IndexingInProgress)
                    {
                        int total = stats.ItemCount;
                        int completed = stats.CompletedCount;
                        if (total > 0)
                        {
                            double percent = (double)completed / total * 100;
                            _searchView?.SetIndexProgressBar(percent);
                        }
                    }
                    else
                    {
                        _searchView?.SetSearchBoxIndexingCompleted();
                    }
                });
            };

            listener.ContentItemStatusChanged += (sender, statusMap) =>
            {
                foreach (var kvp in statusMap)
                {
                    Debug.WriteLine($"Content item '{kvp.Key}' status: {kvp.Value.Status}, Error: {kvp.Value.ErrorDetail}");
                }
            };
        }

        private static void LogIndexStatistics()
        {
            if (_appContentIndexer is null)
            {
                return;
            }

            var stats = _appContentIndexer.GetIndexStatistics();
            Debug.WriteLine($"Index statistics — Items: {stats.ItemCount}, " +
                $"Completed: {stats.CompletedCount}, InProgress: {stats.InProgressCount}, " +
                $"NotStarted: {stats.NotStartedCount}, Errors: {stats.ErrorsCount}, " +
                $"PendingDeletion: {stats.PendingDeletionCount}, " +
                $"RequiringReindexing: {stats.RequiringReindexingCount}");
        }

        private async Task InitializeAppContentIndexerAsync()
        {
            GetOrCreateIndexResult? getOrCreateResult = null;
            await Task.Run(() =>
            {
                // Pre-flight: check system-level ACI capabilities before creating an index.
                CheckSystemCapabilities();

                getOrCreateResult = AppContentIndexer.GetOrCreateIndex("NotesIndex");
                if (getOrCreateResult == null)
                {
                    throw new InvalidOperationException("GetOrCreateIndexResult is null");
                }
                if (!getOrCreateResult.Succeeded)
                {
                    throw getOrCreateResult.ExtendedError;
                }

                _appContentIndexer = getOrCreateResult.Indexer;

                LogIndexStatistics();
            });

            // Subscribe to live status updates from the indexer.
            SubscribeToIndexerListener();

            DispatcherQueue.TryEnqueue(() =>
            {
                ChatPaneToggleButton.IsEnabled = true;
                AppSearchView.SetSearchBoxInitializingCompleted();

                // Initialize query sessions for search-as-you-type.
                AppSearchView.InitializeQuerySessions();

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
            // OpenAttachmentView is async void: any exception that escapes here is unhandled
            // and will crash the app (no DispatcherUnhandledException handler is registered).
            // Guard against missing/stale files or other update failures.
            try
            {
                await AttachmentView.UpdateAttachment(attachment, attachmentText, boundingBox);
                AttachmentView.Show();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"OpenAttachmentView failed for attachment id={attachment?.Attachment?.Id}: {ex.Message}");
            }
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
