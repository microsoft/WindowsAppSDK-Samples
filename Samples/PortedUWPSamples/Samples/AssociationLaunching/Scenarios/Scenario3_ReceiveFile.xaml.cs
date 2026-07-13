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

using System;
using System.Collections.Generic;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;
using Windows.ApplicationModel.Activation;
using Windows.Storage;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates receiving a file activation. When the user opens a ".alsdk" file (or a file
    /// with no extension via "Open With"), the app is launched or activated and routed here with
    /// the activation payload (see Program.cs / App.RouteActivation).
    /// </summary>
    public sealed partial class Scenario3_ReceiveFile : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser().
        MainPage rootPage = MainPage.Current;

        string Extension = "alsdk";

        public Scenario3_ReceiveFile()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when this page is about to be displayed in a Frame. The Parameter carries the
        /// file activation payload when the app was launched/activated by opening a file.
        /// </summary>
        protected override async void OnNavigatedTo(NavigationEventArgs e)
        {
            // Display the result of the file activation if we got here as a result of being
            // activated for a file. In WinUI 3 the activation payload is surfaced through
            // AppInstance and routed here as the navigation parameter (see App.RouteActivation).
            var args = e.Parameter as IFileActivatedEventArgs;

            if (args != null)
            {
                string output = "Files received: " + args.Files.Count + "\n";
                foreach (IStorageItem item in args.Files)
                {
                    output = output + item.Name + "\n";
                }

                if (args is IFileActivatedEventArgsWithNeighboringFiles neighboringArgs &&
                    neighboringArgs.NeighboringFilesQuery != null)
                {
                    // If supported, we receive the pictures from the same folder as the file that
                    // was launched because we declared the picturesLibrary capability. The order of
                    // the neighboring files matches the sort order of the folder from which the
                    // file was launched.
                    IReadOnlyList<StorageFile> neighboringFiles = await neighboringArgs.NeighboringFilesQuery.GetFilesAsync();
                    if (neighboringFiles.Count > 0)
                    {
                        output = output + "\nNeighboring files: " + neighboringFiles.Count + "\n";
                        int i;
                        for (i = 0; i < Math.Min(neighboringFiles.Count, 3); i++)
                        {
                            output = output + neighboringFiles[i].Name + "\n";
                        }
                        int remaining = neighboringFiles.Count - i;
                        if (remaining > 0)
                        {
                            output = output + "and " + remaining + " more.";
                        }
                    }
                }
                rootPage.NotifyUser(output, NotifyType.StatusMessage);
            }
        }

        private async void CreateTestFile()
        {
            StorageFolder folder = await KnownFolders.GetFolderAsync(KnownFolderId.PicturesLibrary);
            await folder.CreateFileAsync("Test " + Extension + " file." + Extension, CreationCollisionOption.ReplaceExisting);
            await Windows.System.Launcher.LaunchFolderAsync(folder);
        }

        private async void CreateTestFileWithNoExtension()
        {
            StorageFolder folder = await KnownFolders.GetFolderAsync(KnownFolderId.PicturesLibrary);
            await folder.CreateFileAsync("Test file with no extension", CreationCollisionOption.ReplaceExisting);
            await Windows.System.Launcher.LaunchFolderAsync(folder);
        }

        private async void RemoveTestFiles()
        {
            StorageFolder folder = await KnownFolders.GetFolderAsync(KnownFolderId.PicturesLibrary);
            try
            {
                StorageFile file = await folder.GetFileAsync("Test " + Extension + " file." + Extension);
                await file.DeleteAsync();
            }
            catch (Exception)
            {
                // File I/O errors are reported as exceptions. Ignore errors here.
            }

            try
            {
                StorageFile file = await folder.GetFileAsync("Test file with no extension");
                await file.DeleteAsync();
            }
            catch (Exception)
            {
                // File I/O errors are reported as exceptions. Ignore errors here.
            }
        }
    }
}
