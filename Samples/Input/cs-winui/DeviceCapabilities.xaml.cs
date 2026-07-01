// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using System.Text;
using Microsoft.UI.Xaml.Controls;
using Windows.Devices.Input;

namespace Input
{
    public sealed partial class DeviceCapabilities : Page
    {
        public DeviceCapabilities()
        {
            this.InitializeComponent();

            KeyboardCapabilities kbdCapabilities = new KeyboardCapabilities();
            keyboardText.Text = "Keyboard present = " + kbdCapabilities.KeyboardPresent.ToString();

            MouseCapabilities mouseCapabilities = new MouseCapabilities();
            StringBuilder sb = new StringBuilder();
            sb.Append("Mouse present = " + mouseCapabilities.MousePresent.ToString() + "\n");
            sb.Append("Number of buttons = " + mouseCapabilities.NumberOfButtons.ToString() + "\n");
            sb.Append("Vertical wheel present = " + mouseCapabilities.VerticalWheelPresent.ToString() + "\n");
            sb.Append("Horizontal wheel present = " + mouseCapabilities.HorizontalWheelPresent.ToString() + "\n");
            sb.Append("Buttons swapped = " + mouseCapabilities.SwapButtons.ToString());
            mouseText.Text = sb.ToString();

            TouchCapabilities touchCapabilities = new TouchCapabilities();
            sb = new StringBuilder();
            sb.Append("Touch present = " + touchCapabilities.TouchPresent.ToString() + "\n");
            sb.Append("Touch contacts supported = " + touchCapabilities.Contacts.ToString());
            touchText.Text = sb.ToString();
        }
    }
}
