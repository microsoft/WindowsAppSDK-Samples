// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.

using System.IO;
using System.Runtime.InteropServices.WindowsRuntime;

namespace WindowsML.Shared
{
    /// <summary>
    /// Result processing and display functionality
    /// </summary>
    public static class ResultProcessor
    {
        /// <summary>
        /// Apply softmax to results and print top predictions
        /// </summary>
        public static void PrintResults(IList<string> labels, IReadOnlyList<float> results)
        {
            // Apply softmax to the results
            float maxLogit = results.Max();
            var expScores = results.Select(r => MathF.Exp(r - maxLogit)).ToList(); // stability with maxLogit
            float sumExp = expScores.Sum();
            var softmaxResults = expScores.Select(e => e / sumExp).ToList();

            // Get top 5 results
            IEnumerable<(int Index, float Confidence)> topResults = softmaxResults
                .Select((value, index) => (Index: index, Confidence: value))
                .OrderByDescending(x => x.Confidence)
                .Take(5);

            // Display results
            Console.WriteLine("Top Predictions:");
            Console.WriteLine("-------------------------------------------");
            Console.WriteLine("{0,-32} {1,10}", "Label", "Confidence");
            Console.WriteLine("-------------------------------------------");

            foreach (var result in topResults)
            {
                Console.WriteLine("{0,-32} {1,10:P2}", labels[result.Index], result.Confidence);
            }

            Console.WriteLine("-------------------------------------------");
        }

        /// <summary>
        /// Load labels from file
        /// </summary>
        public static IList<string> LoadLabels(string labelsPath)
        {
            return File.ReadAllLines(labelsPath)
                .Select(line => line.Split(',', 2)[1])
                .ToList();
        }

        /// <summary>
        /// Get top predictions with labels and confidence scores
        /// </summary>
        public static List<(string Label, float Confidence)> GetTopPredictions(IReadOnlyList<float> results, IList<string> labels, int topCount = 5)
        {
            // Apply softmax normalization
            float maxLogit = results.Max();
            var expScores = results.Select(r => MathF.Exp(r - maxLogit)).ToArray();
            float sumExp = expScores.Sum();
            var softmaxResults = expScores.Select(e => e / sumExp).ToArray();

            return softmaxResults
                .Select((confidence, index) => (Label: labels[index], Confidence: confidence))
                .OrderByDescending(x => x.Confidence)
                .Take(topCount)
                .ToList();
        }
    }
}
