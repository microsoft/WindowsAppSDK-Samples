using System;
using System.Collections.Generic;
using Windows.Devices.Haptics;

namespace SDKTemplate
{
    public partial class MainPage : Microsoft.UI.Xaml.Controls.Page
    {
        public const string FEATURE_NAME = "PenHaptics C# sample";

        // The code for each scenario lives under Scenarios\PenHaptics\.
        List<FeatureGroup> featureGroups = new List<FeatureGroup>
        {
            new FeatureGroup("Pen Haptics", new()
            {
                new Scenario { Title = "Query Tactile Feedback Support", ClassType = typeof(Scenario2_SupportedTactileFeedback) },
                new Scenario { Title = "Inking Feedback", ClassType = typeof(Scenario3_InkingFeedback) },
                new Scenario { Title = "Interaction Feedback", ClassType = typeof(Scenario4_InteractionFeedback) },
                new Scenario { Title = "Inking and Interaction Feedback", ClassType = typeof(Scenario5_InkingAndInteractionFeedback) },
            }),
        };

        // Mapping between waveform names and waveform values.
        public static IReadOnlyDictionary<string, ushort> WaveformNamesMap { get; } = new Dictionary<string, ushort>()
            {
                ["BrushContinuous"] = KnownSimpleHapticsControllerWaveforms.BrushContinuous,
                ["BuzzContinuous"] = KnownSimpleHapticsControllerWaveforms.BuzzContinuous,
                ["ChiselMarkerContinuous"] = KnownSimpleHapticsControllerWaveforms.ChiselMarkerContinuous,
                ["Click"] = KnownSimpleHapticsControllerWaveforms.Click,
                ["EraserContinuous"] = KnownSimpleHapticsControllerWaveforms.EraserContinuous,
                ["Error"] = KnownSimpleHapticsControllerWaveforms.Error,
                ["GalaxyPenContinuous"] = KnownSimpleHapticsControllerWaveforms.GalaxyPenContinuous,
                ["Hover"] = KnownSimpleHapticsControllerWaveforms.Hover,
                ["InkContinuous"] = KnownSimpleHapticsControllerWaveforms.InkContinuous,
                ["MarkerContinuous"] = KnownSimpleHapticsControllerWaveforms.MarkerContinuous,
                ["PencilContinuous"] = KnownSimpleHapticsControllerWaveforms.PencilContinuous,
                ["Press"] = KnownSimpleHapticsControllerWaveforms.Press,
                ["Release"] = KnownSimpleHapticsControllerWaveforms.Release,
                ["RumbleContinuous"] = KnownSimpleHapticsControllerWaveforms.RumbleContinuous,
                ["Success"] = KnownSimpleHapticsControllerWaveforms.Success,
            };

        // Returns the first SimpleHapticsControllerFeedback supported by the provided SimpleHapticsController
        // which supports the specified waveform.
        public static SimpleHapticsControllerFeedback FindSupportedFeedback(SimpleHapticsController hapticsController, ushort waveform)
        {
            foreach (SimpleHapticsControllerFeedback feedback in hapticsController.SupportedFeedback)
            {
                if (feedback.Waveform == waveform)
                {
                    return feedback;
                }
            }
            return null;
        }
    }

    public class Scenario
    {
        public string Title { get; set; }
        public Type ClassType { get; set; }
    }

    public class FeatureGroup
    {
        public FeatureGroup(string title, List<Scenario> scenarios)
        {
            Title = title;
            Scenarios = scenarios;
        }

        public string Title { get; set; }
        public List<Scenario> Scenarios { get; set; }
    }
}
