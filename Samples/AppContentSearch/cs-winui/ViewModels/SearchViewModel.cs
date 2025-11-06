// Copyright (c) Microsoft Corporation. All rights reserved.

using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.UI.Xaml;
using System;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Threading;
using System.Threading.Tasks;

namespace Notes.ViewModels
{
    public partial class SearchViewModel : ObservableObject
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
        private CancellationTokenSource? _currentSearchCancellation;

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
            _currentSearchCancellation?.Cancel();
            _currentSearchCancellation?.Dispose();
            
            // Create new cancellation token for this search
            _currentSearchCancellation = new CancellationTokenSource();
            var cancellationToken = _currentSearchCancellation.Token;
            
            try
            {
                Reset();
                if (string.IsNullOrWhiteSpace(_searchText))
                {
                    return;
                }

                if (MainWindow.appContentIndexer == null)
                {
                    Debug.WriteLine("appContentIndexer is null");
                    return;
                }

                var results = await Utils.SearchAsync(MainWindow.appContentIndexer, _searchText, cancellationToken: cancellationToken);

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
    }
}
