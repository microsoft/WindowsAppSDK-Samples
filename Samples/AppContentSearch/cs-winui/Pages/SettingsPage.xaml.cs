// Copyright (c) Microsoft Corporation. All rights reserved.

using Microsoft.AI.Foundry.Local;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Security.Credentials;
using Windows.Storage;

namespace Notes.Pages
{
    public sealed partial class SettingsPage : Page
    {
        private const string ModelSourceKey = "ModelSource"; // "phi" | "azure" | "foundry"
        private const string FoundryModelKey = "FoundryModelName";
        private const string ShowBoundingBoxesKey = "ShowBoundingBoxes"; // bool

        private const string PasswordVaultResourceName = "NotesApp.AzureOpenAI";
        private const string ApiKeyCredentialKey = "ApiKey";
        private const string EndpointCredentialKey = "Endpoint";
        private const string DeploymentCredentialKey = "Deployment";

        private FoundryLocalManager? _foundryManager;
        private readonly Task? _foundryInitTask;
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
            SaveButton.IsEnabled = false;

            LoadSettings();
            _foundryInitTask = PopulateFoundryModelsAsync();

            // Initialize available models list
            _ = PopulateAvailableModelsAsync();

            // Setup event handlers
            AvailableModelsComboBox.SelectionChanged += AvailableModelsComboBox_SelectionChanged;
            FoundryModelComboBox.SelectionChanged += FoundryModelComboBox_SelectionChanged;

            // Add change tracking event handlers
            AzureApiKeyPasswordBox.PasswordChanged += OnSettingChanged;
            AzureEndpointUriTextBox.TextChanged += OnSettingChanged;
            AzureDeploymentNameTextBox.TextChanged += OnSettingChanged;
            ShowBoundingBoxesToggleSwitch.Toggled += OnSettingChanged;
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
            FoundryModelComboBox.IsEnabled = false;
            FoundryModelComboBox.Items.Clear();

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
                        FoundryModelComboBox.Items.Add(new ComboBoxItem
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
                    FoundryModelComboBox.SelectedIndex = savedModelIndex;
                }
                else if (FoundryModelComboBox.SelectedItem == null && FoundryModelComboBox.Items.Count > 0)
                {
                    FoundryModelComboBox.SelectedIndex = 0;
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
                FoundryModelComboBox.IsEnabled = true;

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

                AvailableModelsComboBox.Items.Clear();
                foreach (var model in catalogModels)
                {
                    // Only show models that aren't already downloaded
                    if (!cachedAliases.Contains(model.Alias))
                    {
                        AvailableModelsComboBox.Items.Add(new ComboBoxItem
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

                DownloadedModelsListView.ItemsSource = cached.ToList();
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"UpdateDownloadedModelsList error: {ex.Message}");
            }
        }

        private void AvailableModelsComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            DownloadModelButton.IsEnabled = AvailableModelsComboBox.SelectedItem != null;
        }

        private void FoundryModelComboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            // Only update save button state if initialization is complete
            if (!_isInitializing)
            {
                UpdateSaveButtonState();
            }
        }

        private async void DownloadModel_Click(object sender, RoutedEventArgs e)
        {
            if (AvailableModelsComboBox.SelectedItem is not ComboBoxItem selectedItem || selectedItem.Tag is not ModelInfo modelInfo)
                return;

            // Show progress UI
            DownloadProgressStackPanel.Visibility = Visibility.Visible;
            DownloadStatusTextBlock.Text = $"Downloading {modelInfo.Alias}...";
            DownloadProgressBar.IsIndeterminate = false;
            DownloadProgressBar.Value = 0;
            DownloadModelButton.IsEnabled = false;

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
                        DownloadProgressBar.Value = progress.Percentage * 100;
                        DownloadStatusTextBlock.Text = $"Downloading {modelInfo.Alias}... {progress.Percentage:P0}";
                    });
                }

                // Download completed
                DispatcherQueue.TryEnqueue(() =>
                {
                    DownloadStatusTextBlock.Text = $"Successfully downloaded {modelInfo.Alias}";
                    DownloadProgressBar.Value = 100;
                });

                // Refresh the model lists
                await PopulateFoundryModelsAsync();
                await PopulateAvailableModelsAsync();

                // Hide progress after a delay
                await Task.Delay(2000);
                DownloadProgressStackPanel.Visibility = Visibility.Collapsed;
            }
            catch (Exception ex)
            {
                DownloadStatusTextBlock.Text = $"Failed to download {modelInfo.Alias}: {ex.Message}";
                DownloadProgressBar.Value = 0;

                Debug.WriteLine($"DownloadModel_Click error: {ex.Message}");
            }
            finally
            {
                DownloadModelButton.IsEnabled = true;
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

            // Azure - Store secrets in PasswordVault
            SaveAzureCredentialsSecurely(
                AzureApiKeyPasswordBox.Password,
                AzureEndpointUriTextBox.Text,
                AzureDeploymentNameTextBox.Text
            );

            // Foundry
            localSettings.Values[FoundryModelKey] = GetFoundryModelSelection();

            // Bounding boxes
            localSettings.Values[ShowBoundingBoxesKey] = ShowBoundingBoxesToggleSwitch.IsOn;

            // Update original values to reflect saved state
            UpdateOriginalValues();
            UpdateSaveButtonState();
        }

        private void SaveAzureCredentialsSecurely(string apiKey, string endpoint, string deployment)
        {
            try
            {
                var vault = new PasswordVault();

                // Remove existing credentials
                try
                {
                    var existingCreds = vault.FindAllByResource(PasswordVaultResourceName);
                    foreach (var cred in existingCreds)
                    {
                        vault.Remove(cred);
                    }
                }
                catch (Exception)
                {
                    // No existing credentials to remove
                }

                // Store new credentials if not empty
                if (!string.IsNullOrWhiteSpace(apiKey))
                {
                    vault.Add(new PasswordCredential(PasswordVaultResourceName, ApiKeyCredentialKey, apiKey));
                }

                if (!string.IsNullOrWhiteSpace(endpoint))
                {
                    vault.Add(new PasswordCredential(PasswordVaultResourceName, EndpointCredentialKey, endpoint));
                }

                if (!string.IsNullOrWhiteSpace(deployment))
                {
                    vault.Add(new PasswordCredential(PasswordVaultResourceName, DeploymentCredentialKey, deployment));
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error saving credentials to PasswordVault: {ex.Message}");
            }
        }

        private (string apiKey, string endpoint, string deployment) LoadAzureCredentialsSecurely()
        {
            string apiKey = "";
            string endpoint = "";
            string deployment = "";

            try
            {
                var vault = new PasswordVault();
                var credentials = vault.FindAllByResource(PasswordVaultResourceName);

                foreach (var cred in credentials)
                {
                    cred.RetrievePassword();

                    if (cred.UserName == ApiKeyCredentialKey)
                    {
                        apiKey = cred.Password;
                    }
                    else if (cred.UserName == EndpointCredentialKey)
                    {
                        endpoint = cred.Password;
                    }
                    else if (cred.UserName == DeploymentCredentialKey)
                    {
                        deployment = cred.Password;
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.WriteLine($"Error loading credentials from PasswordVault: {ex.Message}");
            }

            return (apiKey, endpoint, deployment);
        }

        private void LoadSettings()
        {
            var localSettings = ApplicationData.Current.LocalSettings;

            string modelSource = localSettings.Values[ModelSourceKey] as string ?? "phi";

            PhiSilicaComboBoxItem.IsSelected = modelSource == "phi";
            AzureComboBoxItem.IsSelected = modelSource == "azure";
            FoundryComboBoxItem.IsSelected = modelSource == "foundry";

            // Load Azure credentials from PasswordVault
            var (apiKey, endpoint, deployment) = LoadAzureCredentialsSecurely();
            AzureApiKeyPasswordBox.Password = apiKey;
            AzureEndpointUriTextBox.Text = endpoint;
            AzureDeploymentNameTextBox.Text = deployment;

            string? savedFoundryModel = localSettings.Values[FoundryModelKey] as string;
            if (!string.IsNullOrWhiteSpace(savedFoundryModel))
            {
                var item = FoundryModelComboBox.Items
                    .OfType<ComboBoxItem>()
                    .FirstOrDefault(i => (string)i.Content == savedFoundryModel);
                if (item != null)
                {
                    FoundryModelComboBox.SelectedItem = item;
                }
            }

            // Bounding boxes (default: true)
            ShowBoundingBoxesToggleSwitch.IsOn = localSettings.Values[ShowBoundingBoxesKey] is bool b ? b : true;

            UpdateSourceDependentFields();
            // Don't call UpdateOriginalValues() or UpdateSaveButtonState() here
            // They will be called after async initialization is complete
        }

        private void UpdateOriginalValues()
        {
            _originalModelSource = GetSelectedModelSource();
            _originalAzureApiKey = AzureApiKeyPasswordBox.Password;
            _originalAzureEndpointUri = AzureEndpointUriTextBox.Text;
            _originalAzureDeploymentName = AzureDeploymentNameTextBox.Text;
            _originalFoundryModel = GetFoundryModelSelection();
            _originalShowBoundingBoxes = ShowBoundingBoxesToggleSwitch.IsOn;
        }

        private bool HasChanges()
        {
            return GetSelectedModelSource() != _originalModelSource ||
                   AzureApiKeyPasswordBox.Password != _originalAzureApiKey ||
                   AzureEndpointUriTextBox.Text != _originalAzureEndpointUri ||
                   AzureDeploymentNameTextBox.Text != _originalAzureDeploymentName ||
                   GetFoundryModelSelection() != _originalFoundryModel ||
                   ShowBoundingBoxesToggleSwitch.IsOn != _originalShowBoundingBoxes;
        }

        private string GetSelectedModelSource()
        {
            if (AzureComboBoxItem.IsSelected) return "azure";
            if (FoundryComboBoxItem.IsSelected) return "foundry";
            return "phi";
        }

        private string GetFoundryModelSelection()
        {
            return (FoundryModelComboBox.SelectedItem as ComboBoxItem)?.Content?.ToString() ?? "";
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
            bool isAzure = AzureComboBoxItem.IsSelected;
            bool isFoundry = FoundryComboBoxItem.IsSelected;

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
                SaveButton.IsEnabled = false;
                return;
            }

            // Check if there are any changes and if configuration is valid
            bool hasChanges = HasChanges();
            bool isValidConfiguration = true;

            // Additional validation: if Foundry is selected, ensure a model is chosen
            if (FoundryComboBoxItem.IsSelected)
            {
                isValidConfiguration = FoundryModelComboBox.SelectedItem != null && !string.IsNullOrWhiteSpace(GetFoundryModelSelection());
            }

            // Enable save button only if there are changes AND configuration is valid
            SaveButton.IsEnabled = hasChanges && isValidConfiguration;
        }
    }
}