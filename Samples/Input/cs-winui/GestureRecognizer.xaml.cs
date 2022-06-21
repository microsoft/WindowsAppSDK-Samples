// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI.Composition;
using Microsoft.UI.Input;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Microsoft.UI.Xaml.Hosting;
using Microsoft.UI;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Input
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class GestureRecognizer : Page
    {
        private Microsoft.UI.Input.GestureRecognizer _gestureRecognizer;
        private Visual _rectangleVisual;
        private Point _relativePoint;
        public GestureRecognizer()
        {
            this.InitializeComponent();
            _rectangleVisual = ElementCompositionPreview.GetElementVisual(GestureRectangle);
            _gestureRecognizer = new Microsoft.UI.Input.GestureRecognizer {
                GestureSettings =
                    GestureSettings.Tap |
                    GestureSettings.DoubleTap |
                    GestureSettings.RightTap |
                    GestureSettings.Drag |
                    GestureSettings.Hold |
                    GestureSettings.HoldWithMouse
            };
            _gestureRecognizer.Tapped += OnTapped;
            _gestureRecognizer.RightTapped += OnRightTapped;
            _gestureRecognizer.Dragging += OnDrag;
            _gestureRecognizer.Holding += OnHold;
        }

        private void OnPointerPressed(object sender, PointerRoutedEventArgs args)
        {
            GestureRectangle.CapturePointer(args.Pointer);

            PointerPoint pointerPoint = args.GetCurrentPoint(Grid);

            // Get coordinates relative to the Rectangle.
            GeneralTransform transform = Grid.TransformToVisual(GestureRectangle);
            _relativePoint = transform.TransformPoint(new Point(pointerPoint.Position.X, pointerPoint.Position.Y));

            _gestureRecognizer.ProcessDownEvent(pointerPoint);
        }

        private void OnPointerMoved(object sender, PointerRoutedEventArgs args)
        {
            PointerPoint currentPoint = args.GetCurrentPoint(Grid);
            if (currentPoint.IsInContact)
            {
                IList<PointerPoint> points = args.GetIntermediatePoints(Grid);
                _gestureRecognizer.ProcessMoveEvents(points);
            }
            else
            {
                _gestureRecognizer.CompleteGesture();
            }
        }

        private void OnPointerReleased(object sender, PointerRoutedEventArgs args)
        {
            PointerPoint pointerPoint = args.GetCurrentPoint(Grid);
            _gestureRecognizer.ProcessUpEvent(pointerPoint);
        }

        private void OnPointerCanceled(object sender, PointerRoutedEventArgs args)
        {
            _gestureRecognizer.CompleteGesture();
        }

        private void OnTapped(object sender, TappedEventArgs args)
        {
            if (args.TapCount == 2)
            {
                GestureResultText.Text = "Double Tapped";
            }
            else
            {
                GestureResultText.Text = "Tapped";
            }

            var color = (GestureRectangle.Fill as SolidColorBrush).Color;
            GestureRectangle.Fill = (color == Colors.Red) ? new SolidColorBrush(Colors.Blue) : new SolidColorBrush(Colors.Red);
        }

        private void OnRightTapped(object sender, RightTappedEventArgs args)
        {
            GestureResultText.Text = "Right Tapped";

            var color = (GestureRectangle.Fill as SolidColorBrush).Color;
            GestureRectangle.Fill = (color == Colors.Red) ? new SolidColorBrush(Colors.Blue) : new SolidColorBrush(Colors.Red);
        }

        private void OnDrag(object sender, DraggingEventArgs args)
        {
            GestureResultText.Text = "Drag";

            var recOffset = _rectangleVisual.Offset;
            recOffset.X = (float)(args.Position.X - _relativePoint.X);
            recOffset.Y = (float)(args.Position.Y - _relativePoint.Y);
            _rectangleVisual.Offset = recOffset;
        }

        private void OnHold(object sender, HoldingEventArgs args)
        {
            GestureResultText.Text = "Holding";

            var color = (GestureRectangle.Fill as SolidColorBrush).Color;
            GestureRectangle.Fill = (color == Colors.Red) ? new SolidColorBrush(Colors.Yellow) : new SolidColorBrush(Colors.Red);
        }
    }
}
