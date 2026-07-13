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
using System.IO;
using Windows.Storage;
using Microsoft.Windows.Storage.Pickers;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Navigation;

namespace SDKTemplate
{
    /// <summary>
    /// Comparing two files to see if they are the same file.
    /// </summary>
    public sealed partial class Scenario9 : Page
    {
        MainPage rootPage = MainPage.Current;

        public Scenario9()
        {
            this.InitializeComponent();
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            rootPage.ValidateFile();
        }

        private async void CompareFilesButton_Click(object sender, RoutedEventArgs e)
        {
            StorageFile file = rootPage.sampleFile;
            if (file != null)
            {
                // Use the Windows App SDK file picker (Microsoft.Windows.Storage.Pickers),
                // which is initialized with the window's AppWindowId instead of requiring
                // HWND interop like the legacy Windows.Storage.Pickers picker.
                FileOpenPicker picker = new FileOpenPicker(this.XamlRoot.ContentIslandEnvironment.AppWindowId);
                picker.SuggestedStartLocation = PickerLocationId.PicturesLibrary;
                picker.FileTypeFilter.Add("*");
                PickFileResult pickResult = await picker.PickSingleFileAsync();
                if (pickResult != null)
                {
                    // The Windows App SDK picker returns a path; open it as a StorageFile
                    // so we can use StorageFile.IsEqual to compare identities.
                    StorageFile comparand = await StorageFile.GetFileFromPathAsync(pickResult.Path);
                    try
                    {
                        if (file.IsEqual(comparand))
                        {
                            rootPage.NotifyUser("Files are equal", NotifyType.StatusMessage);
                        }
                        else
                        {
                            rootPage.NotifyUser("Files are not equal", NotifyType.StatusMessage);
                        }
                    }
                    catch (FileNotFoundException)
                    {
                        rootPage.NotifyUserFileNotExist();
                    }
                }
                else
                {
                    rootPage.NotifyUser("Operation cancelled", NotifyType.StatusMessage);
                }
            }
            else
            {
                rootPage.NotifyUserFileNotExist();
            }
        }
    }
}
