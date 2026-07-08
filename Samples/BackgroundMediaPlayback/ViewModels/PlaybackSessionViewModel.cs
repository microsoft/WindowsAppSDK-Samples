using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Playback;
using Microsoft.UI.Dispatching;

namespace SDKTemplate.ViewModels
{
    public class PlaybackSessionViewModel : INotifyPropertyChanged, IDisposable
    {
        bool disposed;
        MediaPlayer player;
        MediaPlaybackSession playbackSession;
        DispatcherQueue dispatcher;

        public MediaPlaybackState PlaybackState => playbackSession.PlaybackState;
        public event PropertyChangedEventHandler PropertyChanged;

        public PlaybackSessionViewModel(MediaPlaybackSession playbackSession, DispatcherQueue dispatcher)
        {
            this.player = playbackSession.MediaPlayer;
            this.playbackSession = playbackSession;
            this.dispatcher = dispatcher;

            playbackSession.PlaybackStateChanged += PlaybackSession_PlaybackStateChanged;
        }
 
        private void PlaybackSession_PlaybackStateChanged(MediaPlaybackSession sender, object args)
        {
            if (disposed) return;
            dispatcher.TryEnqueue(() =>
            {
                if (disposed) return;
                RaisePropertyChanged("PlaybackState");
            });
        }

        public void Dispose()
        {
            if (disposed)
                return;

            playbackSession.PlaybackStateChanged -= PlaybackSession_PlaybackStateChanged;

            disposed = true;
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
