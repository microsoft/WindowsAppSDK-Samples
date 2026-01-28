// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.ComponentModel;
using System.Runtime.CompilerServices;

namespace Notes.Models
{
    public class Attachment : INotifyPropertyChanged
    {
        private bool isProcessed;
        public int Id { get; set; }
        public string? Filename { get; set; }
        public string? FilenameForText { get; set; }

        public string? RelativePath { get; set; }
        public NoteAttachmentType Type { get; set; }
        public bool IsProcessed
        {
            get { return this.isProcessed; }
            set
            {
                if (value != this.isProcessed)
                {
                    this.isProcessed = value;
                    NotifyPropertyChanged();
                }
            }
        }

        public int NoteId { get; set; }

        public Note? Note { get; set; }

        private void NotifyPropertyChanged([CallerMemberName] String propertyName = "")
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }

        public event PropertyChangedEventHandler? PropertyChanged;
    }

    public enum NoteAttachmentType
    {
        Image = 0,
        Audio = 1,
        Video = 2,
        Document = 3
    }
}
