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

using SDKTemplate.Shared;
using System;
using Microsoft.UI.Xaml.Controls;
using Windows.Media.Core;
using Windows.Media.Playback;
using Windows.Media.Protection;
using Windows.Media.Protection.PlayReady;

namespace SDKTemplate.ViewModels
{
    public class HardwareDRMViewModel : ViewModelBase
    {
        private string moviePath = MainPage.SampleMovieURL;
        private string licenseUrl = "http://playready.directtaps.net/pr/svc/rightsmanager.asmx?PlayRight=1&SecurityLevel=2000";

        public RelayCommand CmdUseHardware { get; private set; }
        public RelayCommand CmdUseSoftware { get; private set; }

        public RelayCommand CmdPlayMovie { get; private set; }
        public RelayCommand CmdStopMovie { get; private set; }

        public PlayReadyInfoViewModel PlayReadyInfo { get; private set; }

        // Hold the player's protection manager so the HW/SW configuration commands can update it.
        private MediaPlayer player;

        // flag to determine if hardware or software have been selected.
        // The application should try to stay with one or the other mode 
        // and not frequently jump between the two.
        private bool modeSelected = false;

        /// <summary>
        /// A view model class for the HW/SW DRM Scenario.
        /// </summary>
        public HardwareDRMViewModel(MediaPlayerElement mediaElement)
        {
            player = mediaElement.MediaPlayer;

            this.ProtectionManager = PlayReadyHelpers.InitializeProtectionManager(ServiceRequested);
            player.ProtectionManager = this.ProtectionManager;
            PlayReadyInfo = new PlayReadyInfoViewModel();
            PlayReadyInfo.RefreshStatics();

            player.PlaybackSession.PlaybackStateChanged += (s, a) => {
                ViewModelBase.Log("Media State::" + s.PlaybackState);
            };

            player.MediaFailed += (s, a) => {
                ViewModelBase.Log("Media Failed::" + a.ErrorMessage);
            };

            CmdPlayMovie    = new RelayCommand(() => { player.Source = MediaSource.CreateFromUri(new Uri(moviePath)); SetPlaybackEnabled(true); },
                                               () => { return modeSelected && PlayReadyHelpers.IsIndividualized; });
            CmdStopMovie    = new RelayCommand(() => { player.Source = null; SetPlaybackEnabled(false); });
            CmdUseHardware  = new RelayCommand(() => { ConfigureHardwareDRM(); },
                                               () => { return !modeSelected;  });
            CmdUseSoftware  = new RelayCommand(() => { ConfigureSoftwareDRM(); },
                                               () => { return !modeSelected; });
        }

        /// <summary>
        /// This method will configure the properties used by Media Foundation and PlayReady
        /// to specify Hardware DRM. The existing ProtectionManager assigned to the player
        /// is altered with updated/removed properties. Lastly, proactive individualization is called.
        /// </summary>
        private void ConfigureHardwareDRM()
        {
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            localSettings.CreateContainer("PlayReady", Windows.Storage.ApplicationDataCreateDisposition.Always);
            localSettings.Containers["PlayReady"].Values["SoftwareOverride"] = 0;
            localSettings.Containers["PlayReady"].Values["HardwareOverride"] = 1;
            player.ProtectionManager.Properties.Remove("Windows.Media.Protection.UseSoftwareProtectionLayer");
            player.ProtectionManager.Properties["Windows.Media.Protection.UseHardwareProtectionLayer"] = true;
            Individualize();
        }

        /// <summary>
        /// This method will configure the properties used by Media Foundation and PlayReady
        /// to specify Software DRM. The existing ProtectionManager assigned to the player
        /// is altered with updated/removed properties. Lastly, proactive individualization is called.
        /// </summary>
        private void ConfigureSoftwareDRM()
        {
            var localSettings = Windows.Storage.ApplicationData.Current.LocalSettings;
            localSettings.CreateContainer("PlayReady", Windows.Storage.ApplicationDataCreateDisposition.Always);
            localSettings.Containers["PlayReady"].Values["SoftwareOverride"] = 1;
            localSettings.Containers["PlayReady"].Values["HardwareOverride"] = 0;
            player.ProtectionManager.Properties.Remove("Windows.Media.Protection.UseHardwareProtectionLayer");
            player.ProtectionManager.Properties["Windows.Media.Protection.UseSoftwareProtectionLayer"] = true;
            Individualize();
        }

        // Proactive individualization is used to configure playready to the selected 
        // hardware/software mode
        void Individualize()
        {
            PlayReadyHelpers.ProactiveIndividualization(() => {
                modeSelected = true;
                CmdPlayMovie.RaiseCanExecuteChanged();
                CmdUseHardware.RaiseCanExecuteChanged();
                CmdUseSoftware.RaiseCanExecuteChanged();
            });
        }

        /// <summary>
        /// The ProtectionManager defers the service call to the ServiceRequested handler.
        /// This handler will enable the application to customize the communication (custom data, http headers, manual request)
        /// The ServiceCompletion instance will notify the ProtectionManager in the case of queued requests.
        /// </summary>
        MediaProtectionServiceCompletion serviceCompletionNotifier = null;
        void ServiceRequested(MediaProtectionManager sender, ServiceRequestedEventArgs srEvent)
        {
            serviceCompletionNotifier = srEvent.Completion;
            IPlayReadyServiceRequest serviceRequest = (IPlayReadyServiceRequest)srEvent.Request;
            ViewModelBase.Log(serviceRequest.GetType().Name);
            ProcessServiceRequest(serviceRequest);
        }

        /// <summary>
        /// The helper class will determine the exact type of ServiceRequest in order to customize and send
        /// the service request. ServiceRequests (except for Individualization and Revocation) also support the
        /// GenerateManualEnablingChallenge method. This can be used to read and customize the SOAP challenge
        /// and manually send the challenge.
        /// </summary>
        void ProcessServiceRequest(IMediaProtectionServiceRequest serviceRequest)
        {
            //Alternatively the serviceRequest can be determined by the Guid serviceRequest.Type
            if (serviceRequest is PlayReadyIndividualizationServiceRequest)
            {
                PlayReadyHelpers.ReactiveIndividualization(serviceRequest as PlayReadyIndividualizationServiceRequest, serviceCompletionNotifier, () => PlayReadyInfo.RefreshStatics());
                PlayReadyInfo.RefreshStatics();
            }
            else if (serviceRequest is PlayReadyLicenseAcquisitionServiceRequest)
            {
                var licenseRequest = serviceRequest as PlayReadyLicenseAcquisitionServiceRequest;
                // The initial service request url was taken from the playready header from the dash manifest.
                // This can be overridden to a different license service prior to sending the request (staging, production,...). 
                licenseRequest.Uri = new Uri(licenseUrl);

                PlayReadyHelpers.ReactiveLicenseAcquisition(licenseRequest, serviceCompletionNotifier);
            }
        }

        public string MoviePath
        {
            get
            {
                return moviePath;
            }
            set
            {
                if (moviePath != value)
                {
                    moviePath = value;
                    RaisePropertyChanged();
                }
            }
        }
    }
}
