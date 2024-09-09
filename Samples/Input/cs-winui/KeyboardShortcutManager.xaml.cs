// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Input;
using Microsoft.UI;
using System;
using Windows.System;

namespace Input
{
    public sealed partial class KeyboardShortcutManager : Page
    {
        public KeyboardShortcutManager()
        {
            this.InitializeComponent();
            LayoutRoot.KeyDown += OnKeyDown;
            LayoutRoot.KeyUp += OnKeyUp;
            LayoutRoot.CharacterReceived += OnCharacterReceived;
        }

        private void OnKeyDown(object sender, KeyRoutedEventArgs e)
        {
            KeyDownText.Text = $"KeyDown: {e.Key}";
        }

        private void OnKeyUp(object sender, KeyRoutedEventArgs e)
        {
            KeyUpText.Text = $"KeyUp: {e.Key}";
            e.Handled = true;
        }

        private void OnCharacterReceived(UIElement sender, CharacterReceivedRoutedEventArgs e)
        {
            CharacterReceivedText.Text = $"Character Received: {(char)e.Character}";
            e.Handled = true;
        }

        private void OnCtrlRInvoked(KeyboardAccelerator sender, KeyboardAcceleratorInvokedEventArgs e)
        {
            AcceleratorInvokedText.Text = "Accelerator Invoked: Ctrl+R";
            e.Handled = true;
        }
    }
}
