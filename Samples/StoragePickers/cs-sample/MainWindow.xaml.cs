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
using System.Text;
using System.Text.Json.Serialization;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Storage;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace FilePickersAppSinglePackaged
{

    [JsonSerializable(typeof(Dictionary<string, List<string>>))]
    internal partial class SourceGenerationContext : JsonSerializerContext
    {
    }

    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
            Title = "File Pickers Sample App";
        }

        #region Helper Methods

        private void LogResult(string message)
        {
            ResultsTextBlock.Text = $"[{DateTime.Now:HH:mm:ss}] {message}\n{ResultsTextBlock.Text}";
        }

        private Microsoft.Windows.Storage.Pickers.PickerLocationId GetSelectedNewLocationId()
        {
            switch (StartLocationComboBox.SelectedIndex)
            {
                case 0: return Microsoft.Windows.Storage.Pickers.PickerLocationId.DocumentsLibrary;
                case 1: return Microsoft.Windows.Storage.Pickers.PickerLocationId.ComputerFolder;
                case 2: return Microsoft.Windows.Storage.Pickers.PickerLocationId.Desktop;
                case 3: return Microsoft.Windows.Storage.Pickers.PickerLocationId.Downloads;
                case 4: return (Microsoft.Windows.Storage.Pickers.PickerLocationId)4;
                case 5: return Microsoft.Windows.Storage.Pickers.PickerLocationId.MusicLibrary;
                case 6: return Microsoft.Windows.Storage.Pickers.PickerLocationId.PicturesLibrary;
                case 7: return Microsoft.Windows.Storage.Pickers.PickerLocationId.VideosLibrary;
                case 8: return Microsoft.Windows.Storage.Pickers.PickerLocationId.Objects3D;
                case 9: return Microsoft.Windows.Storage.Pickers.PickerLocationId.Unspecified;
                case 10: return (Microsoft.Windows.Storage.Pickers.PickerLocationId)10;
                default: throw new InvalidOperationException("Invalid location selected");
            }
        }

        private Microsoft.Windows.Storage.Pickers.PickerViewMode GetSelectedNewViewMode()
        {
            switch (ViewModeComboBox.SelectedIndex)
            {
                case 0: return Microsoft.Windows.Storage.Pickers.PickerViewMode.List;
                case 1: return Microsoft.Windows.Storage.Pickers.PickerViewMode.Thumbnail;
                case 2: return (Microsoft.Windows.Storage.Pickers.PickerViewMode)2;
                default: throw new InvalidOperationException("Invalid view mode selected");
            }
        }

        private string[] GetFileFilters()
        {
            string input = FileTypeFilterInput.Text?.Trim() ?? "";
            if (string.IsNullOrEmpty(input))
                return new string[] { "*" };

            return input.Split(new char[] { ',', ';' }, StringSplitOptions.RemoveEmptyEntries)
                        .Select(s => s.Trim())
                        .ToArray();
        }

        #endregion

        #region FileOpenPicker Tests

        private async void NewPickSingleFile_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                // Initialize new picker with AppWindow.Id
                var picker = new Microsoft.Windows.Storage.Pickers.FileOpenPicker(this.AppWindow.Id);

                if (CommitButtonCheckBox.IsChecked == true)
                {
                    picker.CommitButtonText = CommitButtonTextInput.Text;
                }

                if (ViewModeCheckBox.IsChecked == true)
                {
                    picker.ViewMode = GetSelectedNewViewMode();
                }

                if (SuggestedStartLocationCheckBox.IsChecked == true)
                {
                    picker.SuggestedStartLocation = GetSelectedNewLocationId();
                }

                picker.FileTypeFilter.Clear();
                if (FileTypeFilterCheckBox.IsChecked == true)
                {
                    foreach (var filter in GetFileFilters())
                    {
                        picker.FileTypeFilter.Add(filter);
                    }
                }

                var result = await picker.PickSingleFileAsync();
                if (result != null)
                {
                    LogResult($"New FileOpenPicker - PickSingleFileAsync:\nFile: {System.IO.Path.GetFileName(result.Path)}\nPath: {result.Path}");
                }
                else
                {
                    LogResult("New FileOpenPicker - PickSingleFileAsync: Operation cancelled");
                }
            }
            catch (Exception ex)
            {
                LogResult($"Error in New FileOpenPicker: {ex.Message}");
            }
        }

        private async void NewPickMultipleFiles_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var picker = new Microsoft.Windows.Storage.Pickers.FileOpenPicker(this.AppWindow.Id);

                if (CommitButtonCheckBox.IsChecked == true)
                {
                    picker.CommitButtonText = CommitButtonTextInput.Text;
                }

                if (ViewModeCheckBox.IsChecked == true)
                {
                    picker.ViewMode = GetSelectedNewViewMode();
                }

                if (SuggestedStartLocationCheckBox.IsChecked == true)
                {
                    picker.SuggestedStartLocation = GetSelectedNewLocationId();
                }

                picker.FileTypeFilter.Clear();
                if (FileTypeFilterCheckBox.IsChecked == true)
                {
                    foreach (var filter in GetFileFilters())
                    {
                        picker.FileTypeFilter.Add(filter);
                    }
                }

                var results = await picker.PickMultipleFilesAsync();
                if (results != null && results.Count > 0)
                {
                    var sb = new StringBuilder($"New FileOpenPicker - PickMultipleFilesAsync: {results.Count} files\n");
                    foreach (var result in results)
                    {
                        sb.AppendLine($"- {System.IO.Path.GetFileName(result.Path)}: {result.Path}");
                    }
                    LogResult(sb.ToString());
                }
                else
                {
                    LogResult("New FileOpenPicker - PickMultipleFilesAsync: Operation cancelled or no files selected");
                }
            }
            catch (Exception ex)
            {
                LogResult($"Error in New PickMultipleFilesAsync: {ex.Message}");
            }
        }

        #endregion

        #region FileSavePicker Tests

        private async void NewFileTypeChoices_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var picker = new Microsoft.Windows.Storage.Pickers.FileSavePicker(this.AppWindow.Id);

                if (SuggestedFileNameCheckBox.IsChecked == true)
                {
                    picker.SuggestedFileName = SuggestedFileNameInput.Text;
                }

                if (DefaultFileExtensionCheckBox.IsChecked == true)
                {
                    picker.DefaultFileExtension = DefaultFileExtensionInput.Text;
                }

                if (SuggestedFolderCheckBox.IsChecked == true)
                {
                    picker.SuggestedFolder = SuggestedFolderInput.Text;
                }

                picker.FileTypeChoices.Clear();
                if (FileTypeChoicesCheckBox.IsChecked == true)
                {
                    var choicesJson = (string)FileTypeChoicesInput.Text;
                    if (!string.IsNullOrEmpty(choicesJson))
                    {
                        var choices = System.Text.Json.JsonSerializer.Deserialize(choicesJson, SourceGenerationContext.Default.DictionaryStringListString);
                        foreach (var choice in choices)
                        {
                            picker.FileTypeChoices.Add(choice.Key, choice.Value);
                        }
                    }
                }

                if (CommitButtonCheckBox.IsChecked == true)
                {
                    picker.CommitButtonText = CommitButtonTextInput.Text;
                }

                if (SuggestedStartLocationCheckBox.IsChecked == true)
                {
                    picker.SuggestedStartLocation = GetSelectedNewLocationId();
                }

                var result = await picker.PickSaveFileAsync();
                if (result != null)
                {
                    LogResult($"New FileSavePicker picked file: \n{System.IO.Path.GetFileName(result.Path)}\nPath: {result.Path}");
                }
                else
                {
                    LogResult("New FileSavePicker with FileTypeChoices\nOperation cancelled");
                }
            }
            catch (Exception ex)
            {
                LogResult($"Error in New FileTypeChoices: {ex.Message}");
            }
        }

        #endregion

        #region FolderPicker Tests

        private async void NewPickFolder_Click(object sender, RoutedEventArgs e)
        {
            try
            {
                var picker = new Microsoft.Windows.Storage.Pickers.FolderPicker(this.AppWindow.Id);

                if (CommitButtonCheckBox.IsChecked == true)
                {
                    picker.CommitButtonText = CommitButtonTextInput.Text;
                }

                if (SuggestedStartLocationCheckBox.IsChecked == true)
                {
                    picker.SuggestedStartLocation = GetSelectedNewLocationId();
                }

                var result = await picker.PickSingleFolderAsync();
                if (result != null)
                {
                    LogResult($"New FolderPicker - PickSingleFolderAsync:\nFolder: {System.IO.Path.GetFileName(result.Path)}\nPath: {result.Path}");
                }
                else
                {
                    LogResult("New FolderPicker - PickSingleFolderAsync: Operation cancelled");
                }
            }
            catch (Exception ex)
            {
                LogResult($"Error in New FolderPicker: {ex.Message}");
            }
        }

        #endregion
    }

}
