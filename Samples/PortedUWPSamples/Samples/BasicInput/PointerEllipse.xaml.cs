// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Media;

namespace SDKTemplate
{
    public sealed partial class PointerEllipse : UserControl
    {
        private bool primaryEllipse;

        public PointerEllipse()
        {
            this.InitializeComponent();
            ApplyPointerStyle(false);
        }

        public uint PointerId { get; set; }

        public bool PrimaryPointer { get; set; }

        public bool PrimaryEllipse
        {
            get => primaryEllipse;
            set
            {
                primaryEllipse = value;
                ApplyPointerStyle(value);
            }
        }

        public double Diameter => 120.0;

        private void ApplyPointerStyle(bool isPrimary)
        {
            if (isPrimary)
            {
                ellipse.Fill = new SolidColorBrush(Colors.White);
                ellipse.Stroke = new SolidColorBrush(Colors.Red);
                PrimaryPointerStoryboard.Begin();
            }
            else
            {
                PrimaryPointerStoryboard.Stop();
                ellipse.Fill = new SolidColorBrush(Colors.LightSkyBlue);
                ellipse.Stroke = new SolidColorBrush(Colors.DodgerBlue);
            }
        }
    }
}
