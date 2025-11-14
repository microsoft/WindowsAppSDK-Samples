// Copyright (c) Microsoft Corporation. All rights reserved.

using System;

namespace Notes.Models
{
    public class TranscriptionBlock
    {
        public string Text { get; set; }
        public TimeSpan Start { get; set; }
        public TimeSpan End { get; set; }
        public string StartDisplayText { get; set; }

        public TranscriptionBlock(string text, double start, double end)
        {
            Text = text;
            Start = TimeSpan.FromSeconds(start);
            End = TimeSpan.FromSeconds(end);
            StartDisplayText = FormatTimeSpan(Start);
        }

        public static string FormatTimeSpan(TimeSpan span)
        {
            return FixTimeSegmentLength(span.Hours) + ":" + FixTimeSegmentLength(span.Minutes) + ":" + FixTimeSegmentLength(span.Seconds);
        }

        public static string FixTimeSegmentLength(int timeSegment)
        {
            string castedTimeSegment = timeSegment.ToString();
            return timeSegment < 10 ? "0" + castedTimeSegment : castedTimeSegment;
        }
    }
}
