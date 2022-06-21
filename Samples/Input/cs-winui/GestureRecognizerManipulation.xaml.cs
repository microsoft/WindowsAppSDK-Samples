// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Input;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace Input
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class GestureRecognizerManipulation : Page
    {
        private Microsoft.UI.Input.GestureRecognizer _gestureRecognizer;

        private TransformGroup _cumulativeTransform;
        private MatrixTransform _previousTransform;
        private CompositeTransform _deltaTransform;

        public GestureRecognizerManipulation()
        {
            this.InitializeComponent();

            _cumulativeTransform = new TransformGroup();
            _previousTransform = new MatrixTransform();
            _deltaTransform = new CompositeTransform();

            _cumulativeTransform.Children.Add(_previousTransform);
            _cumulativeTransform.Children.Add(_deltaTransform);

            ImageVisual.RenderTransform = _cumulativeTransform;

            _gestureRecognizer = new Microsoft.UI.Input.GestureRecognizer {
                GestureSettings =
                    GestureSettings.ManipulationRotate |
                    GestureSettings.ManipulationScale |
                    GestureSettings.ManipulationTranslateX |
                    GestureSettings.ManipulationTranslateY
            };

            _gestureRecognizer.ManipulationStarted += OnManipulationStarted;
            _gestureRecognizer.ManipulationUpdated += OnManipulationUpdated;
            _gestureRecognizer.ManipulationCompleted += OnManipulationCompleted;
        }

        private void OnPointerPressed(object sender, PointerRoutedEventArgs args)
        {
            ImageVisual.CapturePointer(args.Pointer);

            PointerPoint currentPoint = args.GetCurrentPoint(grid);
            _gestureRecognizer.ProcessDownEvent(currentPoint);
        }

        private void OnPointerMoved(object sender, PointerRoutedEventArgs args)
        {
            PointerPoint currentPoint = args.GetCurrentPoint(grid);
            if (currentPoint.IsInContact)
            {
                IList<PointerPoint> points = args.GetIntermediatePoints(grid);
                _gestureRecognizer.ProcessMoveEvents(points);
            }
            else
            {
                _gestureRecognizer.CompleteGesture();
            }
        }

        private void OnPointerReleased(object sender, PointerRoutedEventArgs args)
        {
            PointerPoint currentPoint = args.GetCurrentPoint(grid);
            _gestureRecognizer.ProcessUpEvent(currentPoint);
        }

        private void OnPointerCanceled(object sender, PointerRoutedEventArgs args)
        {
            _gestureRecognizer.CompleteGesture();
            ImageVisual.ReleasePointerCapture(args.Pointer);
        }

        private void OnManipulationStarted(object sender, ManipulationStartedEventArgs args)
        {
        }

        private void OnManipulationUpdated(object sender, ManipulationUpdatedEventArgs args)
        {
            // TODO: Image gets distorted when "zooming in".
            _previousTransform.Matrix = _cumulativeTransform.Value;

            _deltaTransform.Rotation = args.Delta.Rotation;
            _deltaTransform.ScaleX = _deltaTransform.ScaleY = args.Delta.Scale;
            _deltaTransform.TranslateX = args.Delta.Translation.X;
            _deltaTransform.TranslateY = args.Delta.Translation.Y;
        }

        private void OnManipulationCompleted(object sender, ManipulationCompletedEventArgs args)
        {
        }
    }
}
