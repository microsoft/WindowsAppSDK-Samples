// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;

namespace Notes.ViewModels
{
    public partial class SearchViewModel : ObservableObject, IDisposable
    {
        [ObservableProperty]
        public bool showResults = false;

        [ObservableProperty]
        public bool showNoResults = false;

        [ObservableProperty]
        public bool showImageResults = false;

        [ObservableProperty]
        public bool showTextResults = false;
        public ObservableCollection<SearchResult> TextResults { get; set; } = new();
        public ObservableCollection<SearchResult> ImageResults { get; set; } = new();

        private string _searchText = string.Empty;
        private CancellationTokenSource? currentSearchCancellation;

        public SearchViewModel()
        {
        }
        public async void HandleQuerySubmitted(string text)
        {
            _searchText = text;
            await Search();
        }

        public void Reset()
        {
            TextResults.Clear();
            ImageResults.Clear();
            ShowResults = false;
            ShowNoResults = false;
            ShowImageResults = false;
            ShowTextResults = false;
        }

        private async Task Search()
        {
            Debug.WriteLine("searching");

            // Cancel any existing search
            currentSearchCancellation?.Cancel();
            currentSearchCancellation?.Dispose();

            // Create new cancellation token for this search
            currentSearchCancellation = new CancellationTokenSource();
            var cancellationToken = currentSearchCancellation.Token;

            try
            {
                Reset();
                if (string.IsNullOrWhiteSpace(_searchText))
                {
                    return;
                }

                if (MainWindow.AppContentIndexer == null)
                {
                    Debug.WriteLine("AppContentIndexer is null");
                    return;
                }

                var results = await Utils.SearchAsync(MainWindow.AppContentIndexer, _searchText, cancellationToken: cancellationToken);

                // Check if we were cancelled before updating UI
                if (cancellationToken.IsCancellationRequested)
                {
                    Debug.WriteLine("Search was cancelled, not updating results");
                    return;
                }

                foreach (var result in results)
                {
                    if (result.ContentType == ContentType.Image)
                    {
                        ImageResults.Add(result);
                    }
                    else if (result.ContentType == ContentType.Note)
                    {
                        TextResults.Add(result);
                    }
                }

                if (ImageResults.Count > 0)
                {
                    ShowImageResults = true;
                }

                if (TextResults.Count > 0)
                {
                    ShowTextResults = true;
                }

                if (!ShowImageResults && !ShowTextResults)
                {
                    ShowNoResults = true;
                }

                ShowResults = true;
            }
            catch (OperationCanceledException)
            {
                Debug.WriteLine("Search operation was cancelled");
                // Don't update UI when cancelled - let the new search handle it
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Search failed: {ex.Message}");
                // Handle other exceptions as needed
            }
        }

        public void Dispose()
        {
            currentSearchCancellation?.Cancel();
            currentSearchCancellation?.Dispose();
            currentSearchCancellation = null;
        }
    }
}
