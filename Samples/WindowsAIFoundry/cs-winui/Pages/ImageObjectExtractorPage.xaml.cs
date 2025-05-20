// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using WindowsAISampleSample.ViewModels;
using Microsoft.Graphics.Imaging;
using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Shapes;
using Windows.Graphics;

namespace WindowsAISampleSample.Pages;

/// <summary>
/// An empty page that can be used on its own or navigated to within a Frame.
/// </summary>
public sealed partial class ImageObjectExtractorPage : Page
{
    public ImageObjectExtractorPage()
    {
        InitializeComponent();
    }

    private void Canvas_PointerReleased(object sender, PointerRoutedEventArgs e)
    {
        var model = DataContext as ImageObjectExtractorViewModel;
        if (model?.Input is not null)
        {
            var currentPoint = e.GetCurrentPoint(InputImageCanvas);
            var ratioX = InputImage.ActualWidth / model.Input.PixelWidth;
            var ratioY = InputImage.ActualHeight / model.Input.PixelHeight;
            // Get the offset between the canvas and the image
            var offSetX = InputImageCanvas.ActualWidth > InputImage.ActualWidth ? (InputImageCanvas.ActualWidth - InputImage.ActualWidth) / 2 : 0;
            var offSetY = InputImageCanvas.ActualHeight > InputImage.ActualHeight ? (InputImageCanvas.ActualHeight - InputImage.ActualHeight) / 2 : 0;
            var x = (uint)((currentPoint.Position.X - offSetX) / ratioX);
            var y = (uint)((currentPoint.Position.Y - offSetY) / ratioY);
            
            model.SelectionPoints.Add(new PointInt32((int)x, (int)y));
            var ellipse = new Ellipse() { Width = 8, Height = 8, Stroke = new SolidColorBrush(Colors.Red), Fill = new SolidColorBrush(Colors.Red) };
            Canvas.SetLeft(ellipse, currentPoint.Position.X - 4);
            Canvas.SetTop(ellipse, currentPoint.Position.Y - 4);
            InputImageCanvas.Children.Add(ellipse);
        }
    }

    private void ClearSelectionPoints()
    {
        var model = DataContext as ImageObjectExtractorViewModel;
        model?.SelectionPoints.Clear();
        InputImageCanvas.Children.Clear();
    }

    private void Canvas_SizeChanged(object sender, SizeChangedEventArgs e)
    {
        ClearSelectionPoints();
    }

    private void ClearSelections_Click(object sender, RoutedEventArgs e)
    {
        ClearSelectionPoints();
    }
}
