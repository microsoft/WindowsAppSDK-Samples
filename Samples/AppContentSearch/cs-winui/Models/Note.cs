// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Collections.Generic;

namespace Notes.Models
{
    public class Note
    {
        public int Id { get; set; }
        public string? Title { get; set; }
        public string? Filename { get; set; }
        public DateTime Created { get; set; }
        public DateTime Modified { get; set; }
        public List<Attachment> Attachments { get; set; } = new();
    }
}
