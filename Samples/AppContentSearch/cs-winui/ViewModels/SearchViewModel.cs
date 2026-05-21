// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using Microsoft.UI.Dispatching;
using Microsoft.Windows.Search.AppContentIndex;

namespace Notes.ViewModels
{
    public partial class SearchViewModel : ObservableObject, IDisposable
    {
        private const int MaxVisibleMatches = 5;

        [ObservableProperty]
        public bool showResults = false;

        [ObservableProperty]
        public bool showNoResults = false;

        [ObservableProperty]
        public bool showImageResults = false;

        [ObservableProperty]
        public bool showTextResults = false;

        [ObservableProperty]
        public bool showOcrResults = false;

        public ObservableCollection<SearchResult> TextResults { get; set; } = new();
        public ObservableCollection<SearchResult> ImageResults { get; set; } = new();
        public ObservableCollection<SearchResult> OcrResults { get; set; } = new();

        private string _searchText = string.Empty;
        private CancellationTokenSource? _currentSearchCancellation;
        private AppIndexTextQuerySession? _textQuerySession;
        private AppIndexImageQuerySession? _imageQuerySession;
        private DispatcherQueue? _dispatcherQueue;
        private bool _isDisposed;

        public SearchViewModel()
        {
        }

        public void HandleQuerySubmitted(string text)
        {
            _searchText = text;
            StartSearch();
        }

        public void HandleTextChanged(string text)
        {
            _searchText = text;
            StartSearch();
        }

        public void InitializeQuerySessions(AppContentIndexer appContentIndexer, DispatcherQueue dispatcherQueue)
        {
            if (_isDisposed || _textQuerySession != null || _imageQuerySession != null)
            {
                return;
            }

            _dispatcherQueue = dispatcherQueue;

            _textQuerySession = appContentIndexer.CreateTextQuerySession();
            _textQuerySession.DesiredMatchesPerResult = MaxVisibleMatches;
            _textQuerySession.ResultChanged += TextQuerySession_ResultChanged;
            _textQuerySession.Start();

            _imageQuerySession = appContentIndexer.CreateImageQuerySession();
            _imageQuerySession.DesiredMatchesPerResult = MaxVisibleMatches;
            _imageQuerySession.ResultChanged += ImageQuerySession_ResultChanged;
            _imageQuerySession.Start();

            if (!string.IsNullOrWhiteSpace(_searchText))
            {
                UpdateQuerySessions(_searchText);
            }
        }

        public void Reset()
        {
            TextResults.Clear();
            ImageResults.Clear();
            OcrResults.Clear();
            ShowResults = false;
            ShowNoResults = false;
            ShowImageResults = false;
            ShowTextResults = false;
            ShowOcrResults = false;
        }

        private void StartSearch()
        {
            if (_isDisposed)
            {
                return;
            }

            if (_textQuerySession != null && _imageQuerySession != null)
            {
                UpdateQuerySessions(_searchText);
                return;
            }

            _ = SearchAsync();
        }

        private void UpdateQuerySessions(string text)
        {
            CancelCurrentSearch();
            Reset();

            if (_textQuerySession is null || _imageQuerySession is null)
            {
                return;
            }

            _textQuerySession.UpdateQueryPhrase(text ?? string.Empty);
            _imageQuerySession.UpdateQueryPhrase(text ?? string.Empty);
        }

        private CancellationToken BeginCurrentSearch()
        {
            CancelCurrentSearch();
            _currentSearchCancellation = new CancellationTokenSource();
            return _currentSearchCancellation.Token;
        }

        private void CancelCurrentSearch()
        {
            _currentSearchCancellation?.Cancel();
            _currentSearchCancellation?.Dispose();
            _currentSearchCancellation = null;
        }

        private async Task SearchAsync()
        {
            Debug.WriteLine("searching");
            string searchText = _searchText;
            CancellationToken cancellationToken = BeginCurrentSearch();

            try
            {
                Reset();
                if (string.IsNullOrWhiteSpace(searchText))
                {
                    return;
                }

                if (MainWindow.AppContentIndexer == null)
                {
                    Debug.WriteLine("AppContentIndexer is null");
                    return;
                }

                var results = await Utils.SearchAsync(
                    MainWindow.AppContentIndexer,
                    searchText,
                    top: MaxVisibleMatches,
                    cancellationToken: cancellationToken);

                // Check if we were cancelled before updating UI
                if (cancellationToken.IsCancellationRequested ||
                    !string.Equals(searchText, _searchText, StringComparison.Ordinal))
                {
                    Debug.WriteLine("Search was cancelled, not updating results");
                    return;
                }

                ApplySearchResults(results, showNoResultsWhenEmpty: true);
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

        private void TextQuerySession_ResultChanged(AppIndexTextQuerySession sender, object args)
        {
            _ = SafeRefreshQuerySessionResultsAsync();
        }

        private void ImageQuerySession_ResultChanged(AppIndexImageQuerySession sender, object args)
        {
            _ = SafeRefreshQuerySessionResultsAsync();
        }

        // Observes both synchronous and asynchronous failures from
        // RefreshQuerySessionResultsAsync. The ResultChanged event handlers can't await the
        // refresh, and a bare `_ = RefreshQuerySessionResultsAsync()` would swallow any
        // exception thrown by the query session (e.g. after disposal or on an unsupported
        // system), causing search to silently stop working.
        private async Task SafeRefreshQuerySessionResultsAsync()
        {
            try
            {
                await RefreshQuerySessionResultsAsync();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Query session refresh failed: {ex.Message}");
            }
        }

        private async Task RefreshQuerySessionResultsAsync()
        {
            if (_isDisposed || _dispatcherQueue is null || _textQuerySession is null || _imageQuerySession is null)
            {
                return;
            }

            string searchText = _searchText;
            if (string.IsNullOrWhiteSpace(searchText))
            {
                return;
            }

            try
            {
                List<TextQueryMatch>? textMatches = null;
                List<ImageQueryMatch>? imageMatches = null;

                TextQuerySessionResult textResult = _textQuerySession.GetResult();
                bool hasValidTextResult =
                    textResult.IsValid &&
                    string.Equals(textResult.QueryPhrase, searchText, StringComparison.Ordinal);
                if (hasValidTextResult)
                {
                    textMatches = textResult.Matches.Take(MaxVisibleMatches).ToList();
                }

                ImageQuerySessionResult imageResult = _imageQuerySession.GetResult();
                bool hasValidImageResult =
                    imageResult.IsValid &&
                    string.Equals(imageResult.QueryPhrase, searchText, StringComparison.Ordinal);
                if (hasValidImageResult)
                {
                    imageMatches = imageResult.Matches.Take(MaxVisibleMatches).ToList();
                }

                if (!hasValidTextResult && !hasValidImageResult)
                {
                    return;
                }

                CancellationToken cancellationToken = BeginCurrentSearch();
                var results = await Utils.SearchMatchesAsync(textMatches, imageMatches, cancellationToken);

                if (_isDisposed ||
                    cancellationToken.IsCancellationRequested ||
                    !string.Equals(searchText, _searchText, StringComparison.Ordinal))
                {
                    return;
                }

                bool showNoResultsWhenEmpty = hasValidTextResult && hasValidImageResult;
                _dispatcherQueue.TryEnqueue(() =>
                {
                    if (_isDisposed ||
                        cancellationToken.IsCancellationRequested ||
                        !string.Equals(searchText, _searchText, StringComparison.Ordinal))
                    {
                        return;
                    }

                    ApplySearchResults(results, showNoResultsWhenEmpty);
                });
            }
            catch (OperationCanceledException)
            {
                Debug.WriteLine("Interactive search update was cancelled");
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Interactive search failed: {ex.Message}");
            }
        }

        private void ApplySearchResults(IEnumerable<SearchResult> results, bool showNoResultsWhenEmpty)
        {
            Reset();

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
                else if (result.ContentType == ContentType.OcrText)
                {
                    OcrResults.Add(result);
                }
            }

            ShowImageResults = ImageResults.Count > 0;
            ShowTextResults = TextResults.Count > 0;
            ShowOcrResults = OcrResults.Count > 0;

            bool hasResults = ShowImageResults || ShowTextResults || ShowOcrResults;
            ShowNoResults = !hasResults && showNoResultsWhenEmpty;
            ShowResults = hasResults || ShowNoResults;
        }

        public void Dispose()
        {
            if (_isDisposed)
            {
                return;
            }

            _isDisposed = true;
            CancelCurrentSearch();

            if (_textQuerySession != null)
            {
                _textQuerySession.ResultChanged -= TextQuerySession_ResultChanged;
                _textQuerySession.Dispose();
                _textQuerySession = null;
            }

            if (_imageQuerySession != null)
            {
                _imageQuerySession.ResultChanged -= ImageQuerySession_ResultChanged;
                _imageQuerySession.Dispose();
                _imageQuerySession = null;
            }

            _dispatcherQueue = null;
        }
    }
}
