// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#pragma once

#include <vector>
#include <string>

namespace WindowsML
{
namespace Shared
{

    /// <summary>
    /// Result processing and display functionality
    /// </summary>
    class ResultProcessor
    {
    public:
        /// <summary>
        /// Apply softmax to results and print top predictions
        /// </summary>
        static void PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results);

        /// <summary>
        /// Apply softmax normalization to raw results
        /// </summary>
        static std::vector<float> ApplySoftmax(const std::vector<float>& results);

        /// <summary>
        /// Get top N predictions with confidence scores
        /// </summary>
        static std::vector<std::pair<int, float>> GetTopPredictions(const std::vector<float>& softmaxResults, int topN = 5);

    private:
        static void PrintPredictionTable(const std::vector<std::string>& labels, const std::vector<std::pair<int, float>>& topPredictions);
    };

} // namespace Shared
} // namespace WindowsML
