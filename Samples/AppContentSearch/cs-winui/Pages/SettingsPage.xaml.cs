// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.AI.Foundry.Local;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Storage;

namespace Notes.Pages
{
    public sealed partial class SettingsPage : Page
    {
        private const string ModelSourceKey = "ModelSource";          // "phi" | "azure" | "foundry"
        private const string FoundryModelKey = "FoundryModelName";
        private const string ShowBoundingBoxesKey = "ShowBoundingBoxes"; // bool

        private FoundryLocalManager? _foundryManager;
        private Task? _foundryInitTask;
        private bool _isInitializing = true; // Track initialization state

        // Original settings values for change tracking
        private string _originalModelSource = "";
        private string _originalAzureApiKey = "";
        private string _originalAzureEndpointUri = "";
        private string _originalAzureDeploymentName = "";
        private string _originalFoundryModel = "";
        private bool _originalShowBoundingBoxes = true;

        public SettingsPage()
        {
            InitializeComponent();

            // Ensure save button starts disabled and stays disabled during initialization
            btnSave.IsEnabled = false;

            LoadSettings();
            _foundryInitTask = PopulateFoundryModelsAsync();

            // Initialize available models list
            _ = PopulateAvailableModelsAsync();

            // Setup event handlers
            cbAvailableModels.SelectionChanged += CbAvailableModels_SelectionChanged;
            cbFoundryModel.SelectionChanged += CbFoundryModel_SelectionChanged;

            // Add change tracking event handlers
            txtAzureApiKey.PasswordChanged += OnSettingChanged;
            txtAzureEndpointUri.TextChanged += OnSettingChanged;
            txtAzureDeploymentName.TextChanged += OnSettingChanged;
            tsShowBoundingBoxes.Toggled += OnSettingChanged;
        }

        private void OnSettingChanged(object sender, object e)
        {
            // Only update save button state if initialization is complete
            if (!_isInitializing)
            {
                UpdateSaveButtonState();
            }
        }

        private async Task PopulateFoundryModelsAsync()
        {
            cbFoundryModel.IsEnabled = false;
            cbFoundryModel.Items.Clear();

            try
            {
                _foundryManager ??= new FoundryLocalManager();

                bool running = _foundryManager.IsServiceRunning;
                Debug.WriteLine($"Foundry service running: {running}");

                var added = new HashSet<string>(StringComparer.OrdinalIgnoreCase);

                string? savedFoundryModel =
                    ApplicationData.Current.LocalSettings.Values[FoundryModelKey] as string;

                int savedModelIndex = -1;
                int currentIndex = 0;

                var cached = await _foundryManager.ListCachedModelsAsync();
                foreach (var m in cached)
                {
                    var alias = m.Alias;
                    if (added.Add(alias))
                    {
                        cbFoundryModel.Items.Add(new ComboBoxItem
                        {
                            Content = alias,
                        });

                        if (savedFoundryModel != null &&
                            string.Equals(alias, savedFoundryModel, StringComparison.OrdinalIgnoreCase))
                        {
                            savedModelIndex = currentIndex;
                        }

                        currentIndex++;
                    }
                }

                if (savedModelIndex >= 0)
                {
                    cbFoundryModel.SelectedIndex = savedModelIndex;
                }
                else if (cbFoundryModel.SelectedItem == null && cbFoundryModel.Items.Count > 0)
                {
                    cbFoundryModel.SelectedIndex = 0;
                }

                // Update downloaded models list
                await UpdateDownloadedModelsList();
            }
            catch (Exception ex)
            {
                Debug.WriteLine("PopulateFoundryModelsAsync error: " + ex.Message);
            }
            finally
            {
                cbFoundryModel.IsEnabled = true;

                // Capture original values after all async initialization is complete
                UpdateOriginalValues();

                // Mark initialization as complete
                _isInitializing = false;

                // Now update save button state
                UpdateSaveButtonState();
            }
        }

        private async Task PopulateAvailableModelsAsync()
        {
            try
            {
                _foundryManager ??= new FoundryLocalManager();

                // Get available models from catalog
                var catalogModels = await _foundryManager.ListCatalogModelsAsync();
                var cachedModels = await _foundryManager.ListCachedModelsAsync();
                var cachedAliases = cachedModels.Select(m => m.Alias).ToHashSet(StringComparer.OrdinalIgnoreCase);

                cbAvailableModels.Items.Clear();
                foreach (var model in catalogModels)
                {
                    // Only show models that aren't already downloaded
                    if (!cachedAliases.Contains(model.Alias))
                    {
                        cbAvailableModels.Items.Add(new ComboBoxItem
                        {
                            Content = model.Alias,
                            Tag = model
                        });
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"PopulateAvailableModelsAsync error: {ex.Message}");
            }
        }

        private async Task UpdateDownloadedModelsList()
        {
            try
            {
                _foundryManager ??= new FoundryLocalManager();
                var cached = await _foundryManager.ListCachedModelsAsync();

                lvDownloadedModels.ItemsSource = cached.ToList();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"UpdateDownloadedModelsList error: {ex.Message}");
            }
        }

        private void CbAvailableModels_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            btnDownloadModel.IsEnabled = cbAvailableModels.SelectedItem != null;
        }

        private void CbFoundryModel_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Only update save button state if initialization is complete
            if (!_isInitializing)
            {
                UpdateSaveButtonState();
            }
        }

        private async void DownloadModel_Click(object sender, RoutedEventArgs e)
        {
            if (cbAvailableModels.SelectedItem is not ComboBoxItem selectedItem || selectedItem.Tag is not ModelInfo modelInfo)
                return;

            // Show progress UI
            spDownloadProgress.Visibility = Visibility.Visible;
            txtDownloadStatus.Text = $"Downloading {modelInfo.Alias}...";
            pbDownloadProgress.IsIndeterminate = false;
            pbDownloadProgress.Value = 0;
            btnDownloadModel.IsEnabled = false;

            try
            {
                _foundryManager ??= new FoundryLocalManager();

                // DownloadModelWithProgressAsync returns IAsyncEnumerable<ModelDownloadProgress>
                // We need to iterate through the progress updates
                await foreach (var progress in _foundryManager.DownloadModelWithProgressAsync(modelInfo.ModelId, null))
                {
                    // Update the UI with progress information
                    DispatcherQueue.TryEnqueue(() =>
                    {
                        pbDownloadProgress.Value = progress.Percentage * 100;
                        txtDownloadStatus.Text = $"Downloading {modelInfo.Alias}... {progress.Percentage:P0}";
                    });
                }

                // Download completed
                DispatcherQueue.TryEnqueue(() =>
                {
                    txtDownloadStatus.Text = $"Successfully downloaded {modelInfo.Alias}";
                    pbDownloadProgress.Value = 100;
                });

                // Refresh the model lists
                await PopulateFoundryModelsAsync();
                await PopulateAvailableModelsAsync();

                // Hide progress after a delay
                await Task.Delay(2000);
                spDownloadProgress.Visibility = Visibility.Collapsed;
            }
            catch (Exception ex)
            {
                txtDownloadStatus.Text = $"Failed to download {modelInfo.Alias}: {ex.Message}";
                pbDownloadProgress.Value = 0;

                Debug.WriteLine($"DownloadModel_Click error: {ex.Message}");
            }
            finally
            {
                btnDownloadModel.IsEnabled = true;
            }
        }

        private async void RefreshModels_Click(object sender, RoutedEventArgs e)
        {
            await PopulateAvailableModelsAsync();
            await PopulateFoundryModelsAsync();
        }

        private async void RemoveModel_Click(object sender, RoutedEventArgs e)
        {
            if (sender is not Button button || button.Tag is not ModelInfo modelInfo)
                return;

            try
            {
                // Show confirmation dialog
                var dialog = new ContentDialog
                {
                    Title = "Remove Model",
                    Content = $"Are you sure you want to remove the model '{modelInfo.Alias}'?",
                    PrimaryButtonText = "Remove",
                    CloseButtonText = "Cancel",
                    XamlRoot = this.XamlRoot
                };

                var result = await dialog.ShowAsync();
                if (result != ContentDialogResult.Primary)
                    return;

                _foundryManager ??= new FoundryLocalManager();

                // Note: You may need to implement model removal based on FoundryLocalManager API
                // This is a placeholder - check if there's a RemoveModelAsync or similar method
                Debug.WriteLine($"Attempting to remove model: {modelInfo.Alias} ({modelInfo.ModelId})");

                // For now, show that removal is not implemented
                var errorDialog = new ContentDialog
                {
                    Title = "Not Implemented",
                    Content = "Model removal functionality is not yet implemented in the FoundryLocalManager API.",
                    CloseButtonText = "OK",
                    XamlRoot = this.XamlRoot
                };
                await errorDialog.ShowAsync();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"RemoveModel_Click error: {ex.Message}");

                // Show error dialog
                var errorDialog = new ContentDialog
                {
                    Title = "Error",
                    Content = $"Failed to remove model: {ex.Message}",
                    CloseButtonText = "OK",
                    XamlRoot = this.XamlRoot
                };
                await errorDialog.ShowAsync();
            }
        }

        private void SaveButton_Click(object sender, RoutedEventArgs e)
        {
            var localSettings = ApplicationData.Current.LocalSettings;

            localSettings.Values[ModelSourceKey] = GetSelectedModelSource();

            // Azure
            localSettings.Values["AzureOpenAIApiKey"] = txtAzureApiKey.Password;
            localSettings.Values["AzureOpenAIEndpointUri"] = txtAzureEndpointUri.Text;
            localSettings.Values["AzureOpenAIDeploymentName"] = txtAzureDeploymentName.Text;

            // Foundry
            localSettings.Values[FoundryModelKey] = GetFoundryModelSelection();

            // Bounding boxes
            localSettings.Values[ShowBoundingBoxesKey] = tsShowBoundingBoxes.IsOn;

            // Update original values to reflect saved state
            UpdateOriginalValues();
            UpdateSaveButtonState();
        }

        private void LoadSettings()
        {
            var localSettings = ApplicationData.Current.LocalSettings;

            string modelSource = localSettings.Values[ModelSourceKey] as string ?? "phi";

            cbPhiSilica.IsSelected = modelSource == "phi";
            cbAzure.IsSelected = modelSource == "azure";
            cbFoundry.IsSelected = modelSource == "foundry";

            txtAzureApiKey.Password = localSettings.Values["AzureOpenAIApiKey"] as string ?? "";
            txtAzureEndpointUri.Text = localSettings.Values["AzureOpenAIEndpointUri"] as string ?? "";
            txtAzureDeploymentName.Text = localSettings.Values["AzureOpenAIDeploymentName"] as string ?? "";

            string? savedFoundryModel = localSettings.Values[FoundryModelKey] as string;
            if (!string.IsNullOrWhiteSpace(savedFoundryModel))
            {
                var item = cbFoundryModel.Items
                    .OfType<ComboBoxItem>()
                    .FirstOrDefault(i => (string)i.Content == savedFoundryModel);
                if (item != null)
                {
                    cbFoundryModel.SelectedItem = item;
                }
            }

            // Bounding boxes (default: true)
            if (localSettings.Values.ContainsKey(ShowBoundingBoxesKey) &&
                localSettings.Values[ShowBoundingBoxesKey] is bool b)
            {
                tsShowBoundingBoxes.IsOn = b;
            }
            else
            {
                tsShowBoundingBoxes.IsOn = true;
            }

            UpdateSourceDependentFields();
            // Don't call UpdateOriginalValues() or UpdateSaveButtonState() here
            // They will be called after async initialization is complete
        }

        private void UpdateOriginalValues()
        {
            _originalModelSource = GetSelectedModelSource();
            _originalAzureApiKey = txtAzureApiKey.Password;
            _originalAzureEndpointUri = txtAzureEndpointUri.Text;
            _originalAzureDeploymentName = txtAzureDeploymentName.Text;
            _originalFoundryModel = GetFoundryModelSelection();
            _originalShowBoundingBoxes = tsShowBoundingBoxes.IsOn;
        }

        private bool HasChanges()
        {
            return GetSelectedModelSource() != _originalModelSource ||
                   txtAzureApiKey.Password != _originalAzureApiKey ||
                   txtAzureEndpointUri.Text != _originalAzureEndpointUri ||
                   txtAzureDeploymentName.Text != _originalAzureDeploymentName ||
                   GetFoundryModelSelection() != _originalFoundryModel ||
                   tsShowBoundingBoxes.IsOn != _originalShowBoundingBoxes;
        }

        private string GetSelectedModelSource()
        {
            if (cbAzure.IsSelected == true) return "azure";
            if (cbFoundry.IsSelected == true) return "foundry";
            return "phi";
        }

        private string GetFoundryModelSelection()
        {
            return (cbFoundryModel.SelectedItem as ComboBoxItem)?.Content?.ToString() ?? "";
        }

        private void ModelSource_SelectionChanged(object sender, RoutedEventArgs e)
        {
            UpdateSourceDependentFields();

            // Only update save button state if initialization is complete
            if (!_isInitializing)
            {
                UpdateSaveButtonState();
            }
        }

        private void UpdateSourceDependentFields()
        {
            bool isAzure = cbAzure.IsSelected == true;
            bool isFoundry = cbFoundry.IsSelected == true;

            if (AzureSettingsCard == null || FoundrySettingsCard == null) return;

            AzureSettingsCard.Visibility = isAzure ? Visibility.Visible : Visibility.Collapsed;
            FoundrySettingsCard.Visibility = isFoundry ? Visibility.Visible : Visibility.Collapsed;

            if (isAzure || isFoundry)
            {
                ModelSourceExpander.IsExpanded = true;
            }
        }

        private void UpdateSaveButtonState()
        {
            // Don't update button state during initialization
            if (_isInitializing)
            {
                btnSave.IsEnabled = false;
                return;
            }

            // Check if there are any changes and if configuration is valid
            bool hasChanges = HasChanges();
            bool isValidConfiguration = true;

            // Additional validation: if Foundry is selected, ensure a model is chosen
            if (cbFoundry.IsSelected == true)
            {
                isValidConfiguration = cbFoundryModel.SelectedItem != null && !string.IsNullOrWhiteSpace(GetFoundryModelSelection());
            }

            // Enable save button only if there are changes AND configuration is valid
            btnSave.IsEnabled = hasChanges && isValidConfiguration;
        }
    }
}