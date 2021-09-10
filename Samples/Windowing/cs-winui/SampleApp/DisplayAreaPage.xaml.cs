using Microsoft.UI.Windowing;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics;
using WinRT;


namespace SampleApp
{
    public sealed partial class DisplayAreaPage : Page
    {

        ObservableCollection<DisplayArea> displayareas = new ObservableCollection<DisplayArea>();

        AppWindow m_mainAppWindow;
        DisplayAreaWatcher myWatcher = DisplayArea.CreateWatcher();

        public DisplayAreaPage()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            // Gets the AppWindow using the windowing interop methods (see WindowingInterop.cs for details)
            m_mainAppWindow = e.Parameter.As<Window>().GetAppWindow();

            // Add our DisplayAreaWatcher event listeners.
            myWatcher.Added += Watcher_Added;
            myWatcher.Removed += Watcher_Removed;
            myWatcher.Updated += Watcher_Updated;
            myWatcher.EnumerationCompleted += Watcher_EnumerationCompleted;

            // Start the watcher
            myWatcher.Start();
            base.OnNavigatedTo(e);
        }

        private void ResetCachedDisplayAreas()
        {
            // Remove all previously cached DisplayArea objects.
            displayareas.Clear();
            // Get the current list of DisplayAreas
            foreach (DisplayArea displayArea in DisplayArea.FindAll())
            {
                displayareas.Add(displayArea);
            }
        }

        private void Watcher_EnumerationCompleted(DisplayAreaWatcher sender, object args)
        {
            ResetCachedDisplayAreas();
        }

        private void Watcher_Updated(DisplayAreaWatcher sender, object args)
        {
            ResetCachedDisplayAreas();
            //if (args != null)
            //{
            //    displayareas.Remove(args.As<DisplayArea>());
            //    displayareas.Add(args.As<DisplayArea>());
            //}
        }

        private void Watcher_Removed(DisplayAreaWatcher sender, object args)
        {
            ResetCachedDisplayAreas();
            //if (args != null)
            //{
            //    displayareas.Remove(args.As<DisplayArea>());
            //}
        }

        private void Watcher_Added(DisplayAreaWatcher sender, object args)
        {
            ResetCachedDisplayAreas();
            //if (args != null)
            //{
            //    displayareas.Add(args.As<DisplayArea>());
            //}
        }

        protected override void OnNavigatingFrom(NavigatingCancelEventArgs e)
        {
            // Stop the watcher and remove all listeners.
            myWatcher.Stop();
            myWatcher.Added -= Watcher_Added;
            myWatcher.Removed -= Watcher_Removed;
            myWatcher.Updated -= Watcher_Updated;
            myWatcher.EnumerationCompleted += Watcher_EnumerationCompleted;
        }

        private void DisplayAreaMoveBtn_Click(object sender, RoutedEventArgs e)
        {
            // Get the currently selected DisplayArea from the list box.
            if (DisplayAreaListBox.SelectedIndex >= 0)
            {
                DisplayArea selectedArea = DisplayAreaListBox.SelectedItem.As<DisplayArea>();
                // Make sure it's actually something useful.
                if (selectedArea != null)
                {
                    // Calculate our new window size and position based on the DisplayArea WorkArea size.
                    RectInt32 newSize;
                    newSize.X = selectedArea.WorkArea.X + (selectedArea.WorkArea.Width / 4);
                    newSize.Y = selectedArea.WorkArea.Y + (selectedArea.WorkArea.Height / 4);
                    newSize.Height = selectedArea.WorkArea.Height / 2;
                    newSize.Width = selectedArea.WorkArea.Width / 2;

                    m_mainAppWindow.MoveAndResize(newSize, selectedArea);
                }
            }
        }
    }
}