// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Threading.Tasks;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media.Animation;
using Microsoft.UI.Xaml.Media.Imaging;
using Microsoft.UI.Xaml.Navigation;
using Windows.ApplicationModel;
using Windows.Storage;
using Windows.Storage.Search;

namespace PhotoEditor;

public sealed partial class MainPage : Page, INotifyPropertyChanged
{
    public static MainPage Current { get; private set; } = null!;

    private ImageFileInfo? persistedItem;

    public ObservableCollection<ImageFileInfo> Images { get; } = new();

    public event PropertyChangedEventHandler? PropertyChanged;

    public MainPage()
    {
        InitializeComponent();
        Current = this;
    }

    // If the image is edited and saved in the details page, this method gets called
    // so that the back navigation connected animation uses the correct image.
    public void UpdatePersistedItem(ImageFileInfo item)
    {
        persistedItem = item;
    }

    protected override async void OnNavigatedTo(NavigationEventArgs e)
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
            ConnectedAnimation? animation =
                ConnectedAnimationService.GetForCurrentView().GetAnimation("backAnimation");
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

        Frame.Navigate(typeof(DetailPage), e.ClickedItem);
    }

    private async Task GetItemsAsync()
    {
        QueryOptions options = new()
        {
            FolderDepth = FolderDepth.Deep,
        };
        options.FileTypeFilter.Add(".jpg");
        options.FileTypeFilter.Add(".png");
        options.FileTypeFilter.Add(".gif");

        // Load the sample pictures that ship with the app. To browse the user's own
        // photos instead, request the 'Pictures Library' capability and use
        // KnownFolders.PicturesLibrary here.
        StorageFolder appInstalledFolder = Package.Current.InstalledLocation;
        StorageFolder picturesFolder = await appInstalledFolder.GetFolderAsync(@"Assets\Samples");

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

        if (unsupportedFilesFound)
        {
            ContentDialog unsupportedFilesDialog = new()
            {
                Title = "Unsupported images found",
                Content = "This sample app only supports images stored locally on the computer. We found files in your library that are stored in OneDrive or another network location. We didn't load those images.",
                CloseButtonText = "Ok",
                XamlRoot = XamlRoot,
            };

            await unsupportedFilesDialog.ShowAsync();
        }
    }

    public static async Task<ImageFileInfo> LoadImageInfo(StorageFile file)
    {
        var properties = await file.Properties.GetImagePropertiesAsync();
        ImageFileInfo info = new(properties, file, file.DisplayName, file.DisplayType);

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
            && FitScreenToggle.IsOn
            && ImageGridView != null
            && ZoomSlider != null)
        {
            // The 'margins' value represents the total of the margins around the
            // image in the grid item. 8 from the ItemTemplate root grid + 8 from
            // the ItemContainerStyle * (Right + Left). If those values change,
            // this value needs to be updated to match.
            int margins = (int)Resources["LargeItemMarginValue"] * 4;
            double gridWidth = ImageGridView.ActualWidth - (int)Resources["DefaultWindowSidePaddingValue"];
            double itemWidth = ZoomSlider.Value + margins;
            // We need at least 1 column.
            int columns = (int)Math.Max(gridWidth / itemWidth, 1);

            // Adjust the available grid width to account for margins around each item.
            double adjustedGridWidth = gridWidth - (columns * margins);

            ItemSize = adjustedGridWidth / columns;
        }
        else
        {
            // ZoomSlider lives in a Flyout, so it may not be realized yet the first
            // time SizeChanged fires; fall back to the slider's default value.
            ItemSize = ZoomSlider?.Value ?? 270;
        }
    }

    private void ImageGridView_ContainerContentChanging(ListViewBase sender, ContainerContentChangingEventArgs args)
    {
        if (args.InRecycleQueue)
        {
            var templateRoot = (Grid)args.ItemContainer.ContentTemplateRoot;
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
            var templateRoot = (Grid)args.ItemContainer.ContentTemplateRoot;
            var image = (Image)templateRoot.FindName("ItemImage");
            image.Opacity = 1;

            var item = (ImageFileInfo)args.Item;

            try
            {
                image.Source = await item.GetImageThumbnailAsync();
            }
            catch (Exception)
            {
                // File could be corrupt, or it might have an image file
                // extension, but not really be an image file.
                BitmapImage bitmapImage = new()
                {
                    UriSource = new Uri(image.BaseUri, "Assets/StoreLogo.png"),
                };
                image.Source = bitmapImage;
            }
        }
    }
}
