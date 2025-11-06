// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Shapes;
using Notes.ViewModels;
using System;
using System.Diagnostics;
using System.Linq;
using Windows.Foundation;
using Windows.Storage;

namespace Notes.Controls
{
    public sealed partial class SearchView : UserControl
    {
        public SearchViewModel ViewModel { get; } = new SearchViewModel();

        public SearchView()
        {
            this.InitializeComponent();
        }

        private void HideResultsPanel()
        {
            ViewModel.ShowResults = false;
        }

        private async void ResultsItemsView_ItemInvoked(ItemsView sender, ItemsViewItemInvokedEventArgs e)
        {
            var context = await AppDataContext.GetCurrentAsync();
            var item = e.InvokedItem as SearchResult;

            if (MainWindow.Instance != null && item != null)
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
                            await MainWindow.Instance.SelectNoteById(
                                note.Id,
                                attachment.Id,
                                item.MostRelevantSentence ?? null,
                                item.BoundingBox);
                        }
                    }
                }
            }

            HideResultsPanel();
        }

        private void Root_LostFocus(object sender, RoutedEventArgs e)
        {
            HideResultsPanel();
        }

        public void StartIndexProgressBar()
        {
            IndexProgressBar.IsIndeterminate = true;
            IndexProgressBar.Opacity = 1;
            IndexProgressBar.Visibility = Visibility.Visible;
        }

        private void SetSearchBoxState(bool isEnabled, Brush foreground, string glyph)
        {
            SearchBoxQueryIcon.Foreground = foreground;
            SearchBoxQueryIcon.Glyph = glyph;
            this.IsEnabled = isEnabled;
        }

        private void ShowProgressBar(bool isIndeterminate, double opacity, bool isVisible)
        {
            IndexProgressBar.IsIndeterminate = isIndeterminate;
            IndexProgressBar.Opacity = opacity;
            IndexProgressBar.Visibility = isVisible ? Visibility.Visible : Visibility.Collapsed;
        }

        public void SetSearchBoxDisabled()
        {
            SetSearchBoxState(false, GetThemeBrush("TextFillColorPrimaryBrush"), "\uE721");
            ShowProgressBar(false, 1, false);
        }

        public void SetSearchBoxInitializing()
        {
            SetSearchBoxState(false, GetThemeBrush("TextFillColorPrimaryBrush"), "\uE721");
            ShowProgressBar(true, 1, true);
        }

        public void SetSearchBoxInitializingCompleted()
        {
            SetSearchBoxState(true, GetThemeBrush("TextFillColorPrimaryBrush"), "\uE721");
            ShowProgressBar(false, 1, false);
        }

        public void SetSearchBoxIndexingCompleted()
        {
            SetSearchBoxState(true, GetThemeBrush("AIAccentGradientBrush"), "\uED37");
            ShowProgressBar(false, 1, false);
        }

        public void StartIndexProgressBarStaging()
        {
            ShowProgressBar(true, 0.5, true);
        }

        public void SetIndexProgressBar(double percent)
        {
            if (IndexProgressBar.Visibility == Visibility.Collapsed)
            {
                IndexProgressBar.Visibility = Visibility.Visible;
            }
            IndexProgressBar.Opacity = 1;
            IndexProgressBar.IsIndeterminate = false;
            IndexProgressBar.Value = percent;
        }

        private Brush GetThemeBrush(string key)
        {
            return Application.Current.Resources[key] as Brush ?? new SolidColorBrush(Colors.Gray);
        }

        private void SearchBox_QuerySubmitted(AutoSuggestBox sender, AutoSuggestBoxQuerySubmittedEventArgs args)
        {
            ViewModel.HandleQuerySubmitted(sender.Text);
        }

        private void OnItemContainerBringIntoViewRequested(UIElement sender, BringIntoViewRequestedEventArgs args)
        {
            // When the popup is being hidden we get a spurious BringIntoView request which the Repeater doesn't handle well.
            // We can avoid that by marking the event as handled when the results panel is not visible.
            if (!ViewModel.ShowResults)
            {
                args.Handled = true;
            }
        }

        private void OnResultImageOpened(object sender, RoutedEventArgs e)
        {
            if (sender is Image img && img.Parent is Grid g)
            {
                UpdateBoundingBoxVisibility(g);
            }
        }

        private void ImageHost_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (sender is Grid g)
            {
                UpdateBoundingBoxVisibility(g);
            }
        }

        private void UpdateBoundingBoxVisibility(Grid host)
        {
            if (host.Tag is not SearchResult sr || sr.BoundingBox is null)
            {
                HideBoundingBox(host);
                return;
            }

            // Read user preference (defaults to true if missing)
            bool showBoxes = true;
            var settings = ApplicationData.Current.LocalSettings;
            if (settings.Values.TryGetValue("ShowBoundingBoxes", out var val) && val is bool b)
            {
                showBoxes = b;
            }

            var overlayRect = host.Children.FirstOrDefault(c => c is Rectangle rect && rect.Name == "OverlayRectangle") as Rectangle;
            var img = host.Children.FirstOrDefault(c => c is Image image && image.Name == "ResultImage") as Image;

            if (overlayRect == null || img == null)
                return;

            if (showBoxes)
            {
                Rect bounds = GetBoundingBox(host, img, sr.BoundingBox.Value);
                ShowBoundingBox(overlayRect, bounds);
            }
            else
            {
                HideBoundingBox(host);
            }
        }

        private Rect GetBoundingBox(Grid host, Image img, Rect rawBoundingBox)
        {
            double hostW = host.ActualWidth;
            double hostH = host.ActualHeight;
            if (hostW <= 0 || hostH <= 0)
                return Rect.Empty;

            double pixelW = (img.Source as BitmapImage)?.PixelWidth ?? hostW;
            double pixelH = (img.Source as BitmapImage)?.PixelHeight ?? hostH;
            if (pixelW <= 0 || pixelH <= 0)
                return Rect.Empty;

            // Calculate rendered size with Uniform scaling
            double aspect = pixelW / pixelH;
            double drawW = hostW;
            double drawH = drawW / aspect;
            if (drawH > hostH)
            {
                drawH = hostH;
                drawW = drawH * aspect;
            }
            double offsetX = (hostW - drawW) / 2.0;
            double offsetY = (hostH - drawH) / 2.0;

            // Normalize if rectangle looks like pixel coordinates
            bool looksPixel = rawBoundingBox.X > 1 || rawBoundingBox.Y > 1 || rawBoundingBox.Width > 1 || rawBoundingBox.Height > 1;
            double normX = looksPixel ? rawBoundingBox.X / pixelW : rawBoundingBox.X;
            double normY = looksPixel ? rawBoundingBox.Y / pixelH : rawBoundingBox.Y;
            double normW = looksPixel ? rawBoundingBox.Width / pixelW : rawBoundingBox.Width;
            double normH = looksPixel ? rawBoundingBox.Height / pixelH : rawBoundingBox.Height;

            // Clamp normalized values for robustness
            normX = Math.Clamp(normX, 0, 1);
            normY = Math.Clamp(normY, 0, 1);
            normW = Math.Clamp(normW, 0, 1 - normX);
            normH = Math.Clamp(normH, 0, 1 - normY);

            // Convert to screen coordinates
            double x = offsetX + normX * drawW;
            double y = offsetY + normY * drawH;
            double w = normW * drawW;
            double h = normH * drawH;

            return new Rect(x, y, w, h);
        }

        private void ShowBoundingBox(Rectangle overlayRect, Rect bounds)
        {
            if (bounds.IsEmpty || bounds.Width <= 0 || bounds.Height <= 0)
            {
                overlayRect.Visibility = Visibility.Collapsed;
                return;
            }

            // Position and size the rectangle
            overlayRect.Width = bounds.Width;
            overlayRect.Height = bounds.Height;
            overlayRect.Margin = new Thickness(bounds.X, bounds.Y, 0, 0);
            overlayRect.HorizontalAlignment = HorizontalAlignment.Left;
            overlayRect.VerticalAlignment = VerticalAlignment.Top;

            // Make it visible
            overlayRect.Visibility = Visibility.Visible;

            Debug.WriteLine($"Shown bounding box rectangle (screen coords: {bounds.X:F1},{bounds.Y:F1},{bounds.Width:F1},{bounds.Height:F1})");
        }

        private void HideBoundingBox(Grid host)
        {
            var overlayRect = host.Children.FirstOrDefault(c => c is Rectangle rect && rect.Name == "OverlayRectangle") as Rectangle;
            if (overlayRect != null)
            {
                overlayRect.Visibility = Visibility.Collapsed;
            }
        }
    }
}