// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Windows.Foundation;

namespace Input
{
    public sealed partial class XamlManipulations : Page
    {
        private TransformGroup transforms;
        private MatrixTransform previousTransform;
        private CompositeTransform deltaTransform;
        private bool forceManipulationsToEnd;

        public XamlManipulations()
        {
            this.InitializeComponent();
            forceManipulationsToEnd = false;

            InitOptions();
            InitManipulationTransforms();

            manipulateMe.ManipulationStarted += new ManipulationStartedEventHandler(ManipulateMe_ManipulationStarted);
            manipulateMe.ManipulationDelta += new ManipulationDeltaEventHandler(ManipulateMe_ManipulationDelta);
            manipulateMe.ManipulationCompleted += new ManipulationCompletedEventHandler(ManipulateMe_ManipulationCompleted);
            manipulateMe.ManipulationInertiaStarting += new ManipulationInertiaStartingEventHandler(ManipulateMe_ManipulationInertiaStarting);

            manipulateMe.ManipulationMode =
                ManipulationModes.TranslateX |
                ManipulationModes.TranslateY |
                ManipulationModes.Rotate |
                ManipulationModes.TranslateInertia |
                ManipulationModes.RotateInertia;
        }

        private void InitManipulationTransforms()
        {
            transforms = new TransformGroup();
            previousTransform = new MatrixTransform() { Matrix = Matrix.Identity };
            deltaTransform = new CompositeTransform();

            transforms.Children.Add(previousTransform);
            transforms.Children.Add(deltaTransform);

            manipulateMe.RenderTransform = transforms;
        }

        private void ManipulateMe_ManipulationStarted(object sender, ManipulationStartedRoutedEventArgs e)
        {
            forceManipulationsToEnd = false;
            manipulateMe.Background = new SolidColorBrush(Colors.DeepSkyBlue);
        }

        private void ManipulateMe_ManipulationDelta(object sender, ManipulationDeltaRoutedEventArgs e)
        {
            if (forceManipulationsToEnd)
            {
                e.Complete();
                return;
            }

            previousTransform.Matrix = transforms.Value;

            Point center = previousTransform.TransformPoint(new Point(e.Position.X, e.Position.Y));
            deltaTransform.CenterX = center.X;
            deltaTransform.CenterY = center.Y;

            deltaTransform.Rotation = e.Delta.Rotation;
            deltaTransform.TranslateX = e.Delta.Translation.X;
            deltaTransform.TranslateY = e.Delta.Translation.Y;
        }

        private void ManipulateMe_ManipulationInertiaStarting(object sender, ManipulationInertiaStartingRoutedEventArgs e)
        {
            manipulateMe.Background = new SolidColorBrush(Colors.RoyalBlue);
        }

        private void ManipulateMe_ManipulationCompleted(object sender, ManipulationCompletedRoutedEventArgs e)
        {
            manipulateMe.Background = new SolidColorBrush(Colors.LightGray);
        }

        private void movementAxis_Changed(object sender, SelectionChangedEventArgs e)
        {
            manipulateMe.ManipulationMode |= ManipulationModes.TranslateX | ManipulationModes.TranslateY;

            ComboBoxItem selectedItem = (ComboBoxItem)((ComboBox)sender).SelectedItem;
            switch (selectedItem.Content.ToString())
            {
                case "X only":
                    manipulateMe.ManipulationMode ^= ManipulationModes.TranslateY;
                    break;
                case "Y only":
                    manipulateMe.ManipulationMode ^= ManipulationModes.TranslateX;
                    break;
            }
        }

        private void InertiaSwitch_Toggled(object sender, RoutedEventArgs e)
        {
            if (manipulateMe != null)
            {
                if (inertiaSwitch.IsOn)
                {
                    manipulateMe.ManipulationMode |= ManipulationModes.TranslateInertia | ManipulationModes.RotateInertia;
                }
                else
                {
                    manipulateMe.ManipulationMode &= ~(ManipulationModes.TranslateInertia | ManipulationModes.RotateInertia);
                }
            }
        }

        private void InitOptions()
        {
            movementAxis.SelectedIndex = 0;
            inertiaSwitch.IsOn = true;
        }

        private void resetButton_Pressed(object sender, RoutedEventArgs e)
        {
            forceManipulationsToEnd = true;
            manipulateMe.RenderTransform = null;
            movementAxis.SelectedIndex = 0;
            InitOptions();
            InitManipulationTransforms();
        }
    }
}
