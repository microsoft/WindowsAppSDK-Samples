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
using System.Threading.Tasks;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Windows.Storage;
using Windows.System;

namespace SDKTemplate
{
    /// <summary>
    /// Demonstrates launching a file with the default handler, with a warning prompt,
    /// with an Open With dialog, or after letting the user pick a file.
    /// </summary>
    public sealed partial class Scenario1_LaunchFile : Page
    {
        // A pointer back to the main page. This is needed if you want to call methods in MainPage such as NotifyUser().
        MainPage rootPage = MainPage.Current;

        // A file that ships with the package; launched by the first scenarios.
        string fileToLaunch = @"Assets\StoreLogo.png";

        public Scenario1_LaunchFile()
        {
            this.InitializeComponent();
        }

        private async Task<StorageFile> GetFileToLaunch()
        {
            // First, get the image file from the package's install directory.
            var file = await Windows.ApplicationModel.Package.Current.InstalledLocation.GetFileAsync(fileToLaunch);

            // Files can't be launched directly from the install folder, so copy it over
            // to the temporary folder first.
            file = await file.CopyAsync(ApplicationData.Current.TemporaryFolder, "filetolaunch.png", NameCollisionOption.ReplaceExisting);

            return file;
        }

        /// <summary>
        /// Launch a .png file that came with the package.
        /// </summary>
        private async void LaunchFileDefault()
        {
            // First, get the image file from the package's image directory.
            var file = await GetFileToLaunch();

            // Next, launch the file.
            bool success = await Launcher.LaunchFileAsync(file);
            if (success)
            {
                rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Launch a .png file that came with the package. Show a warning prompt.
        /// </summary>
        private async void LaunchFileWithWarning()
        {
            // First, get the image file from the package's image directory.
            var file = await GetFileToLaunch();

            // Next, configure the warning prompt.
            var options = new LauncherOptions() { TreatAsUntrusted = true };

            // Finally, launch the file.
            bool success = await Launcher.LaunchFileAsync(file, options);
            if (success)
            {
                rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Launch a .png file that came with the package. Show an Open With dialog that lets the
        /// user choose the handler to use.
        /// </summary>
        private async void LaunchFileOpenWith()
        {
            // First, get the image file from the package's image directory.
            var file = await GetFileToLaunch();

            // Next, configure the Open With dialog.
            var options = new LauncherOptions();
            options.DisplayApplicationPicker = true;

            // Finally, launch the file.
            bool success = await Launcher.LaunchFileAsync(file, options);
            if (success)
            {
                rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
            }
            else
            {
                rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
            }
        }

        /// <summary>
        /// Have the user pick a file, then launch it.
        /// </summary>
        private async void PickAndLaunchFile()
        {
            // First, get a file via the Windows App SDK file picker.
            var openPicker = new Microsoft.Windows.Storage.Pickers.FileOpenPicker(this.XamlRoot.ContentIslandEnvironment.AppWindowId);
            openPicker.FileTypeFilter.Add("*");

            Microsoft.Windows.Storage.Pickers.PickFileResult pickResult = await openPicker.PickSingleFileAsync();
            if (pickResult != null)
            {
                StorageFile file = await StorageFile.GetFileFromPathAsync(pickResult.Path);

                // Next, launch the file.
                bool success = await Launcher.LaunchFileAsync(file);
                if (success)
                {
                    rootPage.NotifyUser("File launched: " + file.Name, NotifyType.StatusMessage);
                }
                else
                {
                    rootPage.NotifyUser("File launch failed.", NotifyType.ErrorMessage);
                }
            }
            else
            {
                rootPage.NotifyUser("No file was picked.", NotifyType.ErrorMessage);
            }
        }
    }
}
