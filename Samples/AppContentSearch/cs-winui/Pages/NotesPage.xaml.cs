// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using Notes.ViewModels;
using System;
using System.Diagnostics;
using System.Threading.Tasks;
using Windows.ApplicationModel.DataTransfer;
using Windows.Graphics.Imaging;
using Windows.Storage;

namespace Notes.Pages
{
    public class RelativePathConverter : IValueConverter
    {
        public object? Convert(object value, Type targetType, object parameter, string language)
        {
            if (value is string relativePath)
            {
                return new BitmapImage(new Uri($"{NotesPage.AttachmentsFolderPath}//{relativePath}"));
            }

            return null;
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            throw new NotImplementedException();
        }
    }

    public sealed partial class NotesPage : Page
    {
        public static string AttachmentsFolderPath = "";

        private NoteViewModel? ViewModel;

        public NotesPage()
        {
            this.InitializeComponent();
        }

        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            if (e.Parameter != null)
            {
                ViewModel = (NoteViewModel)e.Parameter;
                ViewModel.DispatcherQueue = DispatcherQueue;

                await ViewModel.LoadContentAsync();

                var paragraphFormat = ContentsRichEditBox.Document.GetDefaultParagraphFormat();
                paragraphFormat.SetLineSpacing(Microsoft.UI.Text.LineSpacingRule.OneAndHalf, 2f);
                ContentsRichEditBox.Document.SetDefaultParagraphFormat(paragraphFormat);

                ContentsRichEditBox.Document.SetText(Microsoft.UI.Text.TextSetOptions.None, ViewModel.Content);
                ContentsRichEditBox.IsEnabled = true;
                ContentsRichEditBox.Focus(FocusState.Programmatic);
            }
        }

        private async void AttachmentsItemsView_Loaded(object sender, RoutedEventArgs e)
        {
            var attachmentsFolder = await Utils.GetAttachmentsFolderAsync();
            AttachmentsFolderPath = attachmentsFolder.Path;
        }

        private void ContentsRichEditBox_TextChanged(object sender, RoutedEventArgs e)
        {
            if (!ContentsRichEditBox.IsEnabled)
            {
                return;
            }

            ContentsRichEditBox.Document.GetText(Microsoft.UI.Text.TextGetOptions.None, out var content);

            if (ViewModel != null && ViewModel.Content.Trim() != content.Trim())
            {
                Debug.WriteLine("Text changed");
                ViewModel.Content = content.Trim();
            }
        }

        private async void ContentsRichEditBox_Paste(object sender, TextControlPasteEventArgs e)
        {
            var dataPackage = Clipboard.GetContent();

            if (dataPackage.Contains(StandardDataFormats.Text) ||
                dataPackage.Contains(StandardDataFormats.Rtf))
            {
                return;
            }

            e.Handled = true;

            await HandleDataPackage(dataPackage);
        }

        private async Task HandleDataPackage(DataPackageView dataPackage)
        {
            if (ViewModel != null)
            {
                MainWindow.SearchView?.StartIndexProgressBarStaging();

                if (dataPackage.Contains(StandardDataFormats.Bitmap))
                {
                    var imageStreamReference = await dataPackage.GetBitmapAsync();
                    var imageStream = await imageStreamReference.OpenReadAsync();
                    BitmapDecoder decoder = await BitmapDecoder.CreateAsync(imageStream);

                    await ViewModel.AddAttachmentAsync(await decoder.GetSoftwareBitmapAsync());
                }
                else if (dataPackage.Contains(StandardDataFormats.StorageItems))
                {
                    var storageItems = await dataPackage.GetStorageItemsAsync();

                    foreach (var storageItem in storageItems)
                    {
                        if (storageItem is StorageFile storageFile)
                        {
                            await ViewModel.AddAttachmentAsync(storageFile);
                        }
                    }
                }

                MainWindow.SearchView?.SetSearchBoxIndexingCompleted();
            }
        }

        private async void DeleteMenuItem_Click(object sender, RoutedEventArgs e)
        {
            var attachment = (sender as FrameworkElement)?.Tag as AttachmentViewModel;

            if (ViewModel != null && attachment != null)
            {
                await ViewModel.RemoveAttachmentAsync(attachment);
            }
        }

        private void AttachmentsItemsView_ItemInvoked(ItemsView sender, ItemsViewItemInvokedEventArgs e)
        {
            AttachmentViewModel attachmentViewModel = (AttachmentViewModel)e.InvokedItem;
            ((Application.Current as App)?.Window as MainWindow)?.OpenAttachmentView(attachmentViewModel);
        }

        private void Grid_DragOver(object sender, DragEventArgs e)
        {
            e.AcceptedOperation = DataPackageOperation.Copy;
        }

        private async void Grid_Drop(object sender, DragEventArgs e)
        {
            await HandleDataPackage(e.DataView);
        }

        private async void IndexManual_Click(object sender, RoutedEventArgs e)
        {
            if (ViewModel != null)
            {
                await ViewModel.ManualSaveAndIndex();
            }
        }
    }
}
