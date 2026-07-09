// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System;
using Microsoft.Graphics.Canvas;
using Microsoft.Graphics.Canvas.Effects;
using Microsoft.UI.Composition;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Media;
using Windows.Storage.Streams;

namespace PhotoEditor;

internal class LoadedImageBrush : XamlCompositionBrushBase
{
    private bool _isImageLoading;
    private LoadedImageSurface? _surface;
    private CompositionEffectBrush? _combinedBrush;
    private ContrastEffect? _contrastEffect;
    private ExposureEffect? _exposureEffect;
    private TemperatureAndTintEffect? _temperatureAndTintEffect;
    private GaussianBlurEffect? _blurEffect;
    private SaturationEffect? _saturationEffect;

    public ICanvasImage? Image
    {
        get
        {
            if (_blurEffect == null)
            {
                return null;
            }

            _contrastEffect!.Contrast = (float)ContrastAmount;
            _exposureEffect!.Exposure = (float)ExposureAmount;
            _temperatureAndTintEffect!.Tint = (float)TintAmount;
            _temperatureAndTintEffect.Temperature = (float)TemperatureAmount;
            _saturationEffect!.Saturation = (float)SaturationAmount;
            _blurEffect.BlurAmount = (float)BlurAmount;
            return _blurEffect;
        }
    }

    public void SetSource(ICanvasImage source)
    {
        _saturationEffect!.Source = source;
    }

    public static readonly DependencyProperty BlurAmountProperty = DependencyProperty.Register(
        nameof(BlurAmount), typeof(double), typeof(LoadedImageBrush),
        new PropertyMetadata(0.0, OnBlurAmountChanged));

    public double BlurAmount
    {
        get => (double)GetValue(BlurAmountProperty);
        set => SetValue(BlurAmountProperty, value);
    }

    private static void OnBlurAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var brush = (LoadedImageBrush)d;
        brush.CompositionBrush?.Properties.InsertScalar("Blur.BlurAmount", (float)(double)e.NewValue);
    }

    public static readonly DependencyProperty ContrastAmountProperty = DependencyProperty.Register(
        nameof(ContrastAmount), typeof(double), typeof(LoadedImageBrush),
        new PropertyMetadata(0.0, OnContrastAmountChanged));

    public double ContrastAmount
    {
        get => (double)GetValue(ContrastAmountProperty);
        set => SetValue(ContrastAmountProperty, value);
    }

    private static void OnContrastAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var brush = (LoadedImageBrush)d;
        brush.CompositionBrush?.Properties.InsertScalar("ContrastEffect.Contrast", (float)(double)e.NewValue);
    }

    public static readonly DependencyProperty SaturationAmountProperty = DependencyProperty.Register(
        nameof(SaturationAmount), typeof(double), typeof(LoadedImageBrush),
        new PropertyMetadata(1.0, OnSaturationAmountChanged));

    public double SaturationAmount
    {
        get => (double)GetValue(SaturationAmountProperty);
        set => SetValue(SaturationAmountProperty, value);
    }

    private static void OnSaturationAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var brush = (LoadedImageBrush)d;
        brush.CompositionBrush?.Properties.InsertScalar("SaturationEffect.Saturation", (float)(double)e.NewValue);
    }

    public static readonly DependencyProperty ExposureAmountProperty = DependencyProperty.Register(
        nameof(ExposureAmount), typeof(double), typeof(LoadedImageBrush),
        new PropertyMetadata(0.0, OnExposureAmountChanged));

    public double ExposureAmount
    {
        get => (double)GetValue(ExposureAmountProperty);
        set => SetValue(ExposureAmountProperty, value);
    }

    private static void OnExposureAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var brush = (LoadedImageBrush)d;
        brush.CompositionBrush?.Properties.InsertScalar("ExposureEffect.Exposure", (float)(double)e.NewValue);
    }

    public static readonly DependencyProperty TintAmountProperty = DependencyProperty.Register(
        nameof(TintAmount), typeof(double), typeof(LoadedImageBrush),
        new PropertyMetadata(0.0, OnTintAmountChanged));

    public double TintAmount
    {
        get => (double)GetValue(TintAmountProperty);
        set => SetValue(TintAmountProperty, value);
    }

    private static void OnTintAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var brush = (LoadedImageBrush)d;
        brush.CompositionBrush?.Properties.InsertScalar("TemperatureAndTintEffect.Tint", (float)(double)e.NewValue);
    }

    public static readonly DependencyProperty TemperatureAmountProperty = DependencyProperty.Register(
        nameof(TemperatureAmount), typeof(double), typeof(LoadedImageBrush),
        new PropertyMetadata(0.0, OnTemperatureAmountChanged));

    public double TemperatureAmount
    {
        get => (double)GetValue(TemperatureAmountProperty);
        set => SetValue(TemperatureAmountProperty, value);
    }

    private static void OnTemperatureAmountChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
    {
        var brush = (LoadedImageBrush)d;
        brush.CompositionBrush?.Properties.InsertScalar("TemperatureAndTintEffect.Temperature", (float)(double)e.NewValue);
    }

    public void LoadImageFromPath(string path)
    {
        // Load the image from a URI (used as a fallback if the stream load fails).
        _surface = LoadedImageSurface.StartLoadFromUri(new Uri(path));
        _surface.LoadCompleted += Load_Completed;
    }

    public void LoadImageFromStream(IRandomAccessStream stream)
    {
        if (stream != null && !_isImageLoading)
        {
            _isImageLoading = true;
            _surface = LoadedImageSurface.StartLoadFromStream(stream);
            _surface.LoadCompleted += Load_Completed;
        }
    }

    private void Load_Completed(LoadedImageSurface sender, LoadedImageSourceLoadCompletedEventArgs e)
    {
        _isImageLoading = false;

        if (e.Status == LoadedImageSourceLoadStatus.Success)
        {
            var compositor = App.MainWindow.Compositor;
            var brush = compositor.CreateSurfaceBrush(_surface);
            brush.Stretch = CompositionStretch.UniformToFill;

            // Create effects chain.
            _saturationEffect = new SaturationEffect
            {
                Name = "SaturationEffect",
                Saturation = (float)SaturationAmount,
                Source = new CompositionEffectSourceParameter("image"),
            };
            _contrastEffect = new ContrastEffect
            {
                Name = "ContrastEffect",
                Contrast = (float)ContrastAmount,
                Source = _saturationEffect,
            };
            _exposureEffect = new ExposureEffect
            {
                Name = "ExposureEffect",
                Source = _contrastEffect,
                Exposure = (float)ExposureAmount,
            };
            _temperatureAndTintEffect = new TemperatureAndTintEffect
            {
                Name = "TemperatureAndTintEffect",
                Source = _exposureEffect,
                Temperature = (float)TemperatureAmount,
                Tint = (float)TintAmount,
            };
            _blurEffect = new GaussianBlurEffect
            {
                Name = "Blur",
                Source = _temperatureAndTintEffect,
                BlurAmount = (float)BlurAmount,
                BorderMode = EffectBorderMode.Hard,
            };

            CompositionEffectFactory effectFactory = compositor.CreateEffectFactory(_blurEffect, new[]
            {
                "SaturationEffect.Saturation", "ExposureEffect.Exposure", "Blur.BlurAmount",
                "TemperatureAndTintEffect.Temperature", "TemperatureAndTintEffect.Tint",
                "ContrastEffect.Contrast",
            });
            _combinedBrush = effectFactory.CreateBrush();
            _combinedBrush.SetSourceParameter("image", brush);

            // The composition brush is what is applied to the UI element.
            CompositionBrush = _combinedBrush;
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
