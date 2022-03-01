// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading.Tasks;
using Windows.ApplicationModel;
using Windows.Storage;
using Windows.Storage.Search;
using Windows.Storage.Streams;
using Windows.System.Profile;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Animation;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using Windows.Foundation.Metadata;

namespace PhotoEditor
{
    public sealed partial class MainPage : Page, INotifyPropertyChanged
    {
        public static MainPage Current;
        private ImageFileInfo persistedItem;

        public ObservableCollection<ImageFileInfo> Images { get; } = new ObservableCollection<ImageFileInfo>();
        public event PropertyChangedEventHandler PropertyChanged;

        public MainPage()
        {
            this.InitializeComponent();
            Current = this;
        }

        // If the image is edited and saved in the details page, this method gets called
        // so that the back navigation connected animation uses the correct image.
        public void UpdatePersistedItem(ImageFileInfo item)
        {
            persistedItem = item;
        }

        protected async override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (Images.Count == 0)
            {
                await GetItemsAsync();
            }

            base.OnNavigatedTo(e);
        }

        // Called by the Loaded event of the ImageGridView.
        private async void StartConnectedAnimationForBackNavigation()
        {
            // Run the connected animation for navigation back to the main page from the detail page.
            if (persistedItem != null)
            {
                ImageGridView.ScrollIntoView(persistedItem);
                ConnectedAnimation animation = ConnectedAnimationService.GetForCurrentView().GetAnimation("backAnimation");
                if (animation != null)
                {
                    await ImageGridView.TryStartConnectedAnimationAsync(animation, persistedItem, "ItemImage");
                }
            }
        }

        private void ImageGridView_ItemClick(object sender, ItemClickEventArgs e)
        {
            // Prepare the connected animation for navigation to the detail page.
            persistedItem = e.ClickedItem as ImageFileInfo;
            ImageGridView.PrepareConnectedAnimation("itemAnimation", e.ClickedItem, "ItemImage");

            this.Frame.Navigate(typeof(DetailPage), e.ClickedItem);
        }

        private async Task GetItemsAsync()
        {
            QueryOptions options = new QueryOptions();
            options.FolderDepth = FolderDepth.Deep;
            options.FileTypeFilter.Add(".jpg");
            options.FileTypeFilter.Add(".png");
            options.FileTypeFilter.Add(".gif");

            // Get the Pictures library. (Requires 'Pictures Library' capability.)
            //Windows.Storage.StorageFolder picturesFolder = Windows.Storage.KnownFolders.PicturesLibrary;
            // OR
            // Get the Sample pictures.
            StorageFolder appInstalledFolder = Package.Current.InstalledLocation;
            StorageFolder picturesFolder = await appInstalledFolder.GetFolderAsync("Assets\\Samples");

            var result = picturesFolder.CreateFileQueryWithOptions(options);

            IReadOnlyList<StorageFile> imageFiles = await result.GetFilesAsync();
            bool unsupportedFilesFound = false;
            foreach (StorageFile file in imageFiles)
            {
                // Only files on the local computer are supported. 
                // Files on OneDrive or a network location are excluded.
                if (file.Provider.Id == "computer")
                {
                    Images.Add(await LoadImageInfo(file));
                }
                else
                {
                    unsupportedFilesFound = true;
                }
            }

            if (unsupportedFilesFound == true)
            {
                ContentDialog unsupportedFilesDialog = new ContentDialog
                {
                    Title = "Unsupported images found",
                    Content = "This sample app only supports images stored locally on the computer. We found files in your library that are stored in OneDrive or another network location. We didn't load those images.",
                    CloseButtonText = "Ok"
                };

                if (ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 8))
                {
                    unsupportedFilesDialog.XamlRoot = ImageGridView.XamlRoot;
                }

                ContentDialogResult resultNotUsed = await unsupportedFilesDialog.ShowAsync();

                if (Windows.Foundation.Metadata.ApiInformation.IsApiContractPresent("Windows.Foundation.UniversalApiContract", 8))
                {
                    unsupportedFilesDialog.XamlRoot = this.Content.XamlRoot;
                }
            }
        }

        public async static Task<ImageFileInfo> LoadImageInfo(StorageFile file)
        {
            var properties = await file.Properties.GetImagePropertiesAsync();
            ImageFileInfo info = new ImageFileInfo(
                properties, file,
                file.DisplayName, file.DisplayType);

            return info;
        }

        public double ItemSize
        {
            get => _itemSize;
            set
            {
                if (_itemSize != value)
                {
                    _itemSize = value;
                    PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(nameof(ItemSize)));
                }
            }
        }
        private double _itemSize;

        private void DetermineItemSize()
        {
            if (FitScreenToggle != null
                && FitScreenToggle.IsOn == true
                && ImageGridView != null
                && ZoomSlider != null)
            {
                // The 'margins' value represents the total of the margins around the
                // image in the grid item. 8 from the ItemTemplate root grid + 8 from
                // the ItemContainerStyle * (Right + Left). If those values change,
                // this value needs to be updated to match.
                int margins = (int)this.Resources["LargeItemMarginValue"] * 4;
                double gridWidth = ImageGridView.ActualWidth - (int)this.Resources["DefaultWindowSidePaddingValue"];
                double ItemWidth = ZoomSlider.Value + margins;
                // We need at least 1 column.
                int columns = (int)Math.Max(gridWidth / ItemWidth, 1);

                // Adjust the available grid width to account for margins around each item.
                double adjustedGridWidth = gridWidth - (columns * margins);

                ItemSize = (adjustedGridWidth / columns);
            }
            else
            {
                ItemSize = ZoomSlider.Value;
            }
        }

        private void ImageGridView_ContainerContentChanging(ListViewBase sender, ContainerContentChangingEventArgs args)
        {
            if (args.InRecycleQueue)
            {
                var templateRoot = args.ItemContainer.ContentTemplateRoot as Grid;
                var image = (Image)templateRoot.FindName("ItemImage");

                image.Source = null;
            }

            if (args.Phase == 0)
            {
                args.RegisterUpdateCallback(ShowImage);
                args.Handled = true;
            }
        }

        private async void ShowImage(ListViewBase sender, ContainerContentChangingEventArgs args)
        {
            if (args.Phase == 1)
            {
                // It's phase 1, so show this item's image.
                var templateRoot = args.ItemContainer.ContentTemplateRoot as Grid;
                var image = (Image)templateRoot.FindName("ItemImage");
                image.Opacity = 100;

                var item = args.Item as ImageFileInfo;

                try
                {
                    image.Source = await item.GetImageThumbnailAsync();
                }
                catch (Exception)
                {
                    // File could be corrupt, or it might have an image file
                    // extension, but not really be an image file.
                    BitmapImage bitmapImage = new BitmapImage();
                    bitmapImage.UriSource = new Uri(image.BaseUri, "Assets/StoreLogo.png");
                    image.Source = bitmapImage;
                }
            }
        }
    }
}
