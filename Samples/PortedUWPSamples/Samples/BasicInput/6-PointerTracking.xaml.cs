//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

using Microsoft.UI.Input;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using System.Collections.Generic;
using Windows.Foundation;

namespace SDKTemplate
{
    public sealed partial class Scenario6 : Page
    {
        private readonly Dictionary<uint, PointerEllipse> ellipses = new Dictionary<uint, PointerEllipse>();
        private bool primaryExists;

        public Scenario6()
        {
            this.InitializeComponent();
        }

        private void OnPointerEntered(object sender, PointerRoutedEventArgs e)
        {
            if (!e.Pointer.IsInContact)
            {
                AddPointer(e, "Entered");
            }

            e.Handled = true;
        }

        private void OnPointerPressed(object sender, PointerRoutedEventArgs e)
        {
            pointerCanvas.CapturePointer(e.Pointer);
            AddPointer(e, "Pressed");
            e.Handled = true;
        }

        private void OnPointerMoved(object sender, PointerRoutedEventArgs e)
        {
            PointerPoint pointerPoint = e.GetCurrentPoint(pointerCanvas);
            if (ellipses.TryGetValue(pointerPoint.PointerId, out PointerEllipse ellipse))
            {
                UpdateEllipsePosition(ellipse, pointerPoint.Position);
                UpdateLastEvent("Moved", pointerPoint.PointerId);
            }

            e.Handled = true;
        }

        private void OnPointerReleased(object sender, PointerRoutedEventArgs e)
        {
            RemovePointer(e, "Released", true);
            e.Handled = true;
        }

        private void OnPointerExited(object sender, PointerRoutedEventArgs e)
        {
            if (!e.Pointer.IsInContact)
            {
                RemovePointer(e, "Exited", true);
            }

            e.Handled = true;
        }

        private void OnPointerCanceled(object sender, PointerRoutedEventArgs e)
        {
            RemovePointer(e, "Canceled", true);
            e.Handled = true;
        }

        private void OnPointerCaptureLost(object sender, PointerRoutedEventArgs e)
        {
            RemovePointer(e, "CaptureLost", false);
            e.Handled = true;
        }

        private void AddPointer(PointerRoutedEventArgs e, string eventName)
        {
            PointerPoint pointerPoint = e.GetCurrentPoint(pointerCanvas);
            if (ellipses.TryGetValue(pointerPoint.PointerId, out PointerEllipse existingEllipse))
            {
                UpdateEllipsePosition(existingEllipse, pointerPoint.Position);
                UpdateLastEvent(eventName, pointerPoint.PointerId);
                return;
            }

            PointerEllipse pointerEllipse = new PointerEllipse();
            pointerEllipse.PointerId = pointerPoint.PointerId;

            if (pointerPoint.Properties.IsPrimary && !primaryExists)
            {
                pointerEllipse.PrimaryPointer = true;
                pointerEllipse.PrimaryEllipse = true;
                primaryExists = true;
                PointerPrimary.Text = pointerPoint.PointerId.ToString();
            }
            else
            {
                pointerEllipse.PrimaryPointer = false;
                pointerEllipse.PrimaryEllipse = false;
            }

            ellipses[pointerPoint.PointerId] = pointerEllipse;
            UpdateEllipsePosition(pointerEllipse, pointerPoint.Position);
            pointerCanvas.Children.Add(pointerEllipse);
            UpdatePointerCount();
            UpdateLastEvent(eventName, pointerPoint.PointerId);
        }

        private void RemovePointer(PointerRoutedEventArgs e, string eventName, bool releaseCapture)
        {
            if (releaseCapture)
            {
                pointerCanvas.ReleasePointerCapture(e.Pointer);
            }

            PointerPoint pointerPoint = e.GetCurrentPoint(pointerCanvas);
            if (pointerPoint.Properties.IsPrimary)
            {
                PointerPrimary.Text = "n/a";
                primaryExists = false;
            }

            if (ellipses.TryGetValue(pointerPoint.PointerId, out PointerEllipse ellipse))
            {
                pointerCanvas.Children.Remove(ellipse);
                ellipses.Remove(pointerPoint.PointerId);
                UpdatePointerCount();
            }

            UpdateLastEvent(eventName, pointerPoint.PointerId);
        }

        private void UpdateEllipsePosition(PointerEllipse ellipse, Point position)
        {
            Canvas.SetLeft(ellipse, position.X - (ellipse.Diameter / 2.0));
            Canvas.SetTop(ellipse, position.Y - (ellipse.Diameter / 2.0));
        }

        private void UpdatePointerCount()
        {
            PointerCounter.Text = ellipses.Count.ToString();
        }

        private void UpdateLastEvent(string eventName, uint pointerId)
        {
            LastEvent.Text = eventName + " (" + pointerId.ToString() + ")";
        }
    }
}
