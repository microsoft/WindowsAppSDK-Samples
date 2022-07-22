// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Input;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
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
    public class CursorPanel : Panel
    {
        public CursorPanel() { }

        /* With a proper subclass, we can now write our own function that will dynamically change the cursor. */
        /* Be careful not to change the ProtectedCursor before the control is fully loaded (After InitializeComponent() completes) */
        public void ChangeCursor(Microsoft.UI.Input.InputCursor cursor)
        {
            this.ProtectedCursor = cursor;
        }
    }
    
    public sealed partial class InputCursor : Page
    {
        public InputCursor()
        {
            this.InitializeComponent();
        }
        private void cursors_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (panel == null)
                return;

            InputSystemCursorShape shape;

            switch (cursors.SelectedValue)
            {
                case "Arrow":
                    shape = InputSystemCursorShape.Arrow;
                    break;
                case "Cross":
                    shape = InputSystemCursorShape.Cross;
                    break;
                case "Hand":
                    shape = InputSystemCursorShape.Hand;
                    break;
                case "Help":
                    shape = InputSystemCursorShape.Help;
                    break;
                case "IBeam":
                    shape = InputSystemCursorShape.IBeam;
                    break;
                case "Person":
                    shape = InputSystemCursorShape.Person;
                    break;
                case "Pin":
                    shape = InputSystemCursorShape.Pin;
                    break;
                case "SizeAll":
                    shape = InputSystemCursorShape.SizeAll;
                    break;
                case "SizeNortheastSouthwest":
                    shape = InputSystemCursorShape.SizeNortheastSouthwest;
                    break;
                case "SizeNorthSouth":
                    shape = InputSystemCursorShape.SizeNorthSouth;
                    break;
                case "SizeNorthwestSoutheast":
                    shape = InputSystemCursorShape.SizeNorthwestSoutheast;
                    break;
                case "SizeWestEast":
                    shape = InputSystemCursorShape.SizeWestEast;
                    break;
                case "UniversalNo":
                    shape = InputSystemCursorShape.UniversalNo;
                    break;
                case "UpArrow":
                    shape = InputSystemCursorShape.UpArrow;
                    break;
                case "Wait":
                    shape = InputSystemCursorShape.Wait;
                    break;
                default:
                    shape = InputSystemCursorShape.Arrow;
                    break;
            }

            // Creation of an InputSystemCursor requires that the static factory, 'Create' is used along with a prefined shape.
            // For custom cursors, we would call InputResourceCursor.Create with the appropriate cursor resource ID. 
            panel.ChangeCursor(Microsoft.UI.Input.InputSystemCursor.Create(shape));
        }
    }
}
