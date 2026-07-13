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
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Windows.Media.Protection;

namespace SDKTemplate.ViewModels
{
    public class ViewModelBase : INotifyPropertyChanged
    {
        public event PropertyChangedEventHandler PropertyChanged;

        /// <summary>
        /// Fires an event when called. Used to update the UI in the MVVM world.
        /// [CallerMemberName] Ensures only the property that calls it gets the event
        /// and not every property.
        /// </summary>
        protected void RaisePropertyChanged([CallerMemberName] string propertyName = "")
        {
            if (this.PropertyChanged != null)
            {
                this.PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public static void Log(string message)
        {
            // CoreDispatcher.RunAsync -> DispatcherQueue.TryEnqueue in WinUI 3.
            App.MainDispatcherQueue.TryEnqueue(() =>
            {
                outputLog.Insert(0, message);
            });
        }

        public void SetPlaybackEnabled(bool enabled)
        {
            App.MainDispatcherQueue.TryEnqueue(() =>
            {
                PlaybackEnabled = enabled;
            });
        }

        /// <summary>
        /// Provides media and drm bindable logging for ViewModels. 
        /// </summary>
        private static ObservableCollection<string> outputLog = new ObservableCollection<string>();
        public ObservableCollection<string> OutputLog
        {
            get { return outputLog; }
            private set { outputLog = value; }
        }

        /// <summary>
        /// A ProtectionManager is assigned to a MediaPlayer instance to
        /// provide two way communication between the player and PlayReady DRM. 
        /// </summary>
        private MediaProtectionManager protectionManager;
        public MediaProtectionManager ProtectionManager
        {
            get
            {
                return protectionManager;
            }
            protected set
            {
                if (protectionManager != value)
                {
                    protectionManager = value;
                    RaisePropertyChanged();
                }
            }
        }

        private bool playbackEnabled = false;
        public bool PlaybackEnabled
        {
            get
            {
                return playbackEnabled;
            }
            protected set
            {
                if (playbackEnabled != value)
                {
                    playbackEnabled = value;
                    RaisePropertyChanged();
                }
            }
        }
    }
}
