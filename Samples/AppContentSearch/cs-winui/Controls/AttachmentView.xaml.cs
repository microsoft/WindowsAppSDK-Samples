// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.UI.Dispatching;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media.Imaging;
using Notes.Models;
using Notes.ViewModels;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.Foundation;
using Windows.Storage;

namespace Notes.Controls
{
    public sealed partial class AttachmentView : UserControl
    {
        private readonly DispatcherQueue? _dispatcher;
        private Rect? _boundingBox;

        public AttachmentViewModel? AttachmentVM { get; set; }
        public bool AutoScrollEnabled { get; set; } = true;

        public AttachmentView()
        {
            this.InitializeComponent();
            this.Visibility = Visibility.Collapsed;
            this._dispatcher = DispatcherQueue.GetForCurrentThread();
            AttachmentImage.ImageOpened += AttachmentImage_ImageOpened;
            SizeChanged += AttachmentView_SizeChanged;
        }

        public void Show()
        {
            this.Visibility = Visibility.Visible;
            TryDrawBoundingBox();
        }

        public void Hide()
        {
            AttachmentImage.Source = null;
            AttachmentImageTextRect.Visibility = Visibility.Collapsed;
            _boundingBox = null;
            this.Visibility = Visibility.Collapsed;
        }

        private void Root_Tapped(object sender, TappedRoutedEventArgs e)
        {
            // hide the search view only when the background was tapped but not any of the content inside
            if (e.OriginalSource as Grid == Root)
                this.Hide();
        }

        public async Task UpdateAttachment(AttachmentViewModel attachment, string? attachmentText = null, Rect? boundingBox = null)
        {
            AttachmentImageTextRect.Visibility = Visibility.Collapsed;
            _boundingBox = boundingBox;

            AttachmentVM = attachment;

            string? filename = attachment.Attachment.Filename;
            if (string.IsNullOrWhiteSpace(filename))
            {
                Debug.WriteLine($"AttachmentView: attachment id={attachment.Attachment.Id} has no filename; skipping.");
                AttachmentImage.Source = null;
                return;
            }

            StorageFolder attachmentsFolder = await Utils.GetAttachmentsFolderAsync();

            // Use TryGetItemAsync so a missing/stale file (e.g. deleted on disk but still in the
            // search index) does not throw FileNotFoundException and crash the app.
            StorageFile? attachmentFile = await attachmentsFolder.TryGetItemAsync(filename) as StorageFile;
            if (attachmentFile == null)
            {
                Debug.WriteLine($"AttachmentView: file '{filename}' not found in attachments folder; skipping.");
                AttachmentImage.Source = null;
                return;
            }

            switch (AttachmentVM.Attachment.Type)
            {
                case NoteAttachmentType.Image:
                    ImageGrid.Visibility = Visibility.Visible;
                    AttachmentImage.Source = new BitmapImage(new Uri(attachmentFile.Path));
                    break;
                case NoteAttachmentType.Video:
                case NoteAttachmentType.Audio:
                    throw new NotSupportedException("audio and video files are not supported");
            }
        }

        private void CloseButton_Click(object sender, RoutedEventArgs e)
        {
            this.Hide();
        }

        private void AttachmentImage_ImageOpened(object sender, RoutedEventArgs e)
        {
            TryDrawBoundingBox();
        }

        private void AttachmentView_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // Recalculate if layout changes (zoom / window size)
            TryDrawBoundingBox();
        }

        private void TryDrawBoundingBox()
        {
            AttachmentImageTextRect.Visibility = Visibility.Collapsed;

            if (_boundingBox is null)
            {
                return;
            }

            // Respect user preference
            var settings = Windows.Storage.ApplicationData.Current.LocalSettings;
            if (settings.Values.TryGetValue("ShowBoundingBoxes", out var val) && val is bool showBoxes && !showBoxes)
            {
                return;
            }

            if (AttachmentImage.Source is not BitmapImage bmp)
            {
                return;
            }

            double imageW = AttachmentImage.ActualWidth;
            double imageH = AttachmentImage.ActualHeight;
            if (imageW <= 0 || imageH <= 0)
            {
                return;
            }

            double pixelW = bmp.PixelWidth > 0 ? bmp.PixelWidth : imageW;
            double pixelH = bmp.PixelHeight > 0 ? bmp.PixelHeight : imageH;

            var raw = _boundingBox.Value;

            double normX = raw.X / pixelW;
            double normY = raw.Y / pixelH;
            double normW = raw.Width / pixelW;
            double normH = raw.Height / pixelH;

            normX = Math.Clamp(normX, 0, 1);
            normY = Math.Clamp(normY, 0, 1);
            normW = Math.Clamp(normW, 0, 1 - normX);
            normH = Math.Clamp(normH, 0, 1 - normY);

            double rectW = normW * imageW;
            double rectH = normH * imageH;
            double rectX = normX * imageW;
            double rectY = normY * imageH;

            if (rectW <= 0 || rectH <= 0)
                return;

            // Set Rectangle properties
            AttachmentImageTextRect.Width = rectW;
            AttachmentImageTextRect.Height = rectH;
            AttachmentImageTextRect.HorizontalAlignment = HorizontalAlignment.Left;
            AttachmentImageTextRect.VerticalAlignment = VerticalAlignment.Top;
            AttachmentImageTextRect.Margin = new Thickness(rectX, rectY, 0, 0);
            AttachmentImageTextRect.Stroke = new Microsoft.UI.Xaml.Media.SolidColorBrush(Windows.UI.Color.FromArgb(255, 255, 80, 0));
            AttachmentImageTextRect.StrokeThickness = 3;
            AttachmentImageTextRect.Fill = new Microsoft.UI.Xaml.Media.SolidColorBrush(Windows.UI.Color.FromArgb(40, 255, 80, 0));
            AttachmentImageTextRect.RadiusX = 4;
            AttachmentImageTextRect.RadiusY = 4;
            AttachmentImageTextRect.Visibility = Visibility.Visible;
        }
    }
}