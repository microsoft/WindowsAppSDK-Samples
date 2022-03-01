// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;
using Windows.Storage;
using Windows.Storage.FileProperties;
using Windows.Storage.Streams;
using Microsoft.UI.Xaml.Media.Imaging;

namespace PhotoEditor
{
    public class ImageFileInfo : INotifyPropertyChanged
    {
        public ImageFileInfo(ImageProperties properties, StorageFile imageFile, string name, string type)
        {
            ImageProperties = properties;
            ImageName = name;
            ImageFileType = type;
            ImageFile = imageFile;
            var rating = (int)properties.Rating;
            var random = new Random();
            ImageRating = rating == 0 ? random.Next(1, 5) : rating;
        }

        public StorageFile ImageFile { get; }

        public ImageProperties ImageProperties { get; }

        public async Task<BitmapImage> GetImageSourceAsync()
        {
            using (IRandomAccessStream fileStream = await ImageFile.OpenReadAsync())
            {
                // Create a bitmap to be the image source.
                BitmapImage bitmapImage = new BitmapImage();
                bitmapImage.SetSource(fileStream);

                return bitmapImage;
            }
        }

        public async Task<BitmapImage> GetImageThumbnailAsync()
        {
            var thumbnail = await ImageFile.GetThumbnailAsync(ThumbnailMode.PicturesView);
            // Create a bitmap to be the image source.
            BitmapImage bitmapImage = new BitmapImage();
            bitmapImage.SetSource(thumbnail);
            thumbnail.Dispose();

            return bitmapImage;
        }

        public string ImageName { get; }

        public string ImageFileType { get; }

        public string ImageDimensions => $"{ImageProperties.Width} x {ImageProperties.Height}";

        public string ImageTitle
        {
            get => String.IsNullOrEmpty(ImageProperties.Title) ? ImageName : ImageProperties.Title;
            set
            {
                if (ImageProperties.Title != value)
                {
                    ImageProperties.Title = value;
                    var ignoreResult = ImageProperties.SavePropertiesAsync();
                    OnPropertyChanged();
                }
            }
        }

        public int ImageRating
        {
            get => (int)ImageProperties.Rating;
            set
            {
                if (ImageProperties.Rating != value)
                {
                    ImageProperties.Rating = (uint)value;
                    var ignoreResult = ImageProperties.SavePropertiesAsync();
                    OnPropertyChanged();
                }
            }
        }

        private float _exposure = 0;
        public float Exposure
        {
            get => _exposure;
            set => SetEditingProperty(ref _exposure, value);
        }

        private float _temperature = 0;
        public float Temperature
        {
            get => _temperature;
            set => SetEditingProperty(ref _temperature, value);
        }

        private float _tint = 0;
        public float Tint
        {
            get => _tint;
            set => SetEditingProperty(ref _tint, value);
        }

        private float _contrast = 0;
        public float Contrast
        {
            get => _contrast;
            set => SetEditingProperty(ref _contrast, value);
        }

        private float _saturation = 1;
        public float Saturation
        {
            get => _saturation;
            set => SetEditingProperty(ref _saturation, value);
        }

        private float _blur = 0;
        public float Blur
        {
            get => _blur;
            set => SetEditingProperty(ref _blur, value);
        }

        private bool _needsSaved = false;
        public bool NeedsSaved
        {
            get => _needsSaved;
            set => SetProperty(ref _needsSaved, value);
        }

        public event PropertyChangedEventHandler PropertyChanged;

        protected void OnPropertyChanged([CallerMemberName] string propertyName = null) =>
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));

        protected bool SetEditingProperty<T>(ref T storage, T value, [CallerMemberName] String propertyName = null)
        {
            if (SetProperty(ref storage, value, propertyName))
            {
                if (Exposure != 0 || Temperature != 0 || Tint != 0 || Contrast != 0 || Saturation != 1 || Blur != 0)
                {
                    NeedsSaved = true;
                }
                else
                {
                    NeedsSaved = false;
                }
                return true;
            }
            else
            {
                return false;
            }
        }

        protected bool SetProperty<T>(ref T storage, T value, [CallerMemberName] String propertyName = null)
        {
            if (object.Equals(storage, value))
            {
                return false;
            }
            else
            {
                storage = value;
                OnPropertyChanged(propertyName);
                return true;
            }
        }
    }
}
