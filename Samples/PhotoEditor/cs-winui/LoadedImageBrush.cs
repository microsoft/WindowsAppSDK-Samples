// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using System;
using Microsoft.UI.Composition;
using Microsoft.UI.Xaml.Media;
using Windows.Storage.Streams;
using Microsoft.UI.Xaml;

namespace PhotoEditor
{
    class LoadedImageBrush : XamlCompositionBrushBase
    {
        private bool IsImageLoading = false;
        private LoadedImageSurface _surface;
        CompositionEffectBrush combinedBrush;
        ContrastEffect contrastEffect;
        ExposureEffect exposureEffect;
        TemperatureAndTintEffect temperatureAndTintEffect;
        GaussianBlurEffect graphicsEffect;
        SaturationEffect saturationEffect;

        public ICanvasImage Image
        {
            get
            {
                contrastEffect.Contrast = (float)ContrastAmount;
                exposureEffect.Exposure = (float)ExposureAmount;
                temperatureAndTintEffect.Tint = (float)TintAmount;
                temperatureAndTintEffect.Temperature = (float)TemperatureAmount;
                saturationEffect.Saturation = (float)SaturationAmount;
                graphicsEffect.BlurAmount = (float)BlurAmount;
                return graphicsEffect;
            }
        }

        public void SetSource(ICanvasImage source)
        {
            saturationEffect.Source = source;
        }

        public static readonly DependencyProperty BlurAmountProperty = DependencyProperty.Register("BlurAmount",typeof(double),typeof(LoadedImageBrush),new PropertyMetadata(0.0, new PropertyChangedCallback(OnBlurAmountChanged)));

        public double BlurAmount
        {
            get { return (double)GetValue(BlurAmountProperty); }
            set { SetValue(BlurAmountProperty, value); }
        }

        private static void OnBlurAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var brush = (LoadedImageBrush)d;
            // Unbox and set a new blur amount if the CompositionBrush exists.
            brush.CompositionBrush?.Properties.InsertScalar("Blur.BlurAmount", (float)(double)e.NewValue);
        }

        public static readonly DependencyProperty ContrastAmountProperty = DependencyProperty.Register("ContrastAmount",typeof(double),typeof(LoadedImageBrush),new PropertyMetadata(0.0, new PropertyChangedCallback(OnContrastAmountChanged)));

        public double ContrastAmount
        {
            get { return (double)GetValue(ContrastAmountProperty); }
            set { SetValue(ContrastAmountProperty, value); }
        }

        private static void OnContrastAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var brush = (LoadedImageBrush)d;
            // Unbox and set a new blur amount if the CompositionBrush exists.
            brush.CompositionBrush?.Properties.InsertScalar("ContrastEffect.Contrast", (float)(double)e.NewValue);
        }

        public static readonly DependencyProperty SaturationAmountProperty = DependencyProperty.Register("SaturationAmount",typeof(double),typeof(LoadedImageBrush),new PropertyMetadata(1.0, new PropertyChangedCallback(OnSaturationAmountChanged)));

        public double SaturationAmount
        {
            get { return (double)GetValue(SaturationAmountProperty); }
            set { SetValue(SaturationAmountProperty, value); }
        }

        private static void OnSaturationAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var brush = (LoadedImageBrush)d;
            // Unbox and set a new blur amount if the CompositionBrush exists.
            brush.CompositionBrush?.Properties.InsertScalar("SaturationEffect.Saturation", (float)(double)e.NewValue);
        }

        public static readonly DependencyProperty ExposureAmountProperty = DependencyProperty.Register("ExposureAmount",typeof(double),typeof(LoadedImageBrush),new PropertyMetadata(0.0, new PropertyChangedCallback(OnExposureAmountChanged)));

        public double ExposureAmount
        {
            get { return (double)GetValue(ExposureAmountProperty); }
            set { SetValue(ExposureAmountProperty, value); }
        }

        private static void OnExposureAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var brush = (LoadedImageBrush)d;
            // Unbox and set a new blur amount if the CompositionBrush exists.
            brush.CompositionBrush?.Properties.InsertScalar("ExposureEffect.Exposure", (float)(double)e.NewValue);
        }

        public static readonly DependencyProperty TintAmountProperty = DependencyProperty.Register("TintAmount",typeof(double),typeof(LoadedImageBrush),new PropertyMetadata(0.0, new PropertyChangedCallback(OnTintAmountChanged)));

        public double TintAmount
        {
            get { return (double)GetValue(TintAmountProperty); }
            set { SetValue(TintAmountProperty, value); }
        }

        private static void OnTintAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var brush = (LoadedImageBrush)d;
            // Unbox and set a new blur amount if the CompositionBrush exists.
            brush.CompositionBrush?.Properties.InsertScalar("TemperatureAndTintEffect.Tint", (float)(double)e.NewValue);
        }

        public static readonly DependencyProperty TemperatureAmountProperty = DependencyProperty.Register("TemperatureAmount",typeof(double),typeof(LoadedImageBrush),new PropertyMetadata(0.0, new PropertyChangedCallback(OnTemperatureAmountChanged)));

        public double TemperatureAmount
        {
            get { return (double)GetValue(TemperatureAmountProperty); }
            set { SetValue(TemperatureAmountProperty, value); }
        }

        private static void OnTemperatureAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
            var brush = (LoadedImageBrush)d;
            // Unbox and set a new blur amount if the CompositionBrush exists.
            brush.CompositionBrush?.Properties.InsertScalar("TemperatureAndTintEffect.Temperature", (float)(double)e.NewValue);
        }

        public LoadedImageBrush()
        {
        }

        public void LoadImageFromPath(string path)
        {
            var compositor = App.Window.Compositor;
            // Load image
            _surface = LoadedImageSurface.StartLoadFromUri(new Uri(path));
            _surface.LoadCompleted += Load_Completed;
        }

        public void LoadImageFromStream(IRandomAccessStream stream)
        {
            if (stream != null && IsImageLoading == false)
            {
                var compositor = App.Window.Compositor;
                // Load image
                IsImageLoading = true;
                _surface = LoadedImageSurface.StartLoadFromStream(stream);
                _surface.LoadCompleted += Load_Completed;
            }
        }

        private void Load_Completed(LoadedImageSurface sender, LoadedImageSourceLoadCompletedEventArgs e)
        {
            IsImageLoading = false;

            if (e.Status == LoadedImageSourceLoadStatus.Success)
            {
                var compositor = App.Window.Compositor;
                var brush = compositor.CreateSurfaceBrush(_surface);
                brush.Stretch = (Microsoft.UI.Composition.CompositionStretch)CompositionStretch.UniformToFill;

                // Create effects chain.
                saturationEffect = new SaturationEffect()
                {
                    Name = "SaturationEffect",
                    Saturation = (float)SaturationAmount,
                    Source = new CompositionEffectSourceParameter("image")
                };
                contrastEffect = new ContrastEffect()
                {
                    Name = "ContrastEffect",
                    Contrast = (float)ContrastAmount,
                    Source = saturationEffect
                };
                exposureEffect = new ExposureEffect()
                {
                    Name = "ExposureEffect",
                    Source = contrastEffect,
                    Exposure = (float)ExposureAmount,
                };
                temperatureAndTintEffect = new TemperatureAndTintEffect()
                {
                    Name = "TemperatureAndTintEffect",
                    Source = exposureEffect,
                    Temperature = (float)TemperatureAmount,
                    Tint = (float)TintAmount
                };
                graphicsEffect = new GaussianBlurEffect()
                {
                    Name = "Blur",
                    Source = temperatureAndTintEffect,
                    BlurAmount = (float)BlurAmount,
                    BorderMode = EffectBorderMode.Hard,
                };

                Microsoft.UI.Composition.CompositionEffectFactory graphicsEffectFactory = compositor.CreateEffectFactory(graphicsEffect, new[] {
                "SaturationEffect.Saturation", "ExposureEffect.Exposure", "Blur.BlurAmount",
                "TemperatureAndTintEffect.Temperature", "TemperatureAndTintEffect.Tint",
                "ContrastEffect.Contrast" });
                combinedBrush = graphicsEffectFactory.CreateBrush();
                combinedBrush.SetSourceParameter("image", brush);

                // Composition Brush is what is being applied to the UI Element.
                CompositionBrush = combinedBrush;
            }
            else
            {
                LoadImageFromPath("ms-appx:///Assets/StoreLogo.png");
            }
        }

        protected override void OnDisconnected()
        {
            if (_surface != null)
            {
                _surface.Dispose();
                _surface = null;
            }

            if (CompositionBrush != null)
            {
                CompositionBrush.Dispose();
                CompositionBrush = null;
            }
        }
    }
}
