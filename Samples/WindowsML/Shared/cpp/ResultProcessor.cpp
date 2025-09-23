// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License. See LICENSE.md in the repo root for license information.
#include "ResultProcessor.h"
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace WindowsML
{
namespace Shared
{

    void ResultProcessor::PrintResults(const std::vector<std::string>& labels, const std::vector<float>& results)
    {
        // Apply softmax to the results
        std::vector<float> softmaxResults = ApplySoftmax(results);

        // Get top 5 results
        std::vector<std::pair<int, float>> topPredictions = GetTopPredictions(softmaxResults, 5);

        // Display results
        PrintPredictionTable(labels, topPredictions);
    }

    std::vector<float> ResultProcessor::ApplySoftmax(const std::vector<float>& results)
    {
        float maxLogit = *std::max_element(results.begin(), results.end());
        std::vector<float> expScores;
        float sumExp = 0.0f;

        for (float r : results)
        {
            float expScore = std::exp(r - maxLogit);
            expScores.push_back(expScore);
            sumExp += expScore;
        }

        std::vector<float> softmaxResults;
        for (float e : expScores)
        {
            softmaxResults.push_back(e / sumExp);
        }

        return softmaxResults;
    }

    std::vector<std::pair<int, float>> ResultProcessor::GetTopPredictions(const std::vector<float>& softmaxResults, int topN)
    {
        std::vector<std::pair<int, float>> indexedResults;
        for (size_t i = 0; i < softmaxResults.size(); ++i)
        {
            indexedResults.emplace_back(static_cast<int>(i), softmaxResults[i]);
        }

        std::sort(indexedResults.begin(), indexedResults.end(), [](const auto& a, const auto& b) {
            return a.second > b.second;
        });

        indexedResults.resize(std::min<size_t>(topN, indexedResults.size()));
        return indexedResults;
    }

    void ResultProcessor::PrintPredictionTable(const std::vector<std::string>& labels, const std::vector<std::pair<int, float>>& topPredictions)
    {
        std::cout << "Top Predictions:\n";
        std::cout << "-------------------------------------------\n";
        std::cout << std::left << std::setw(32) << "Label" << std::right << std::setw(10) << "Confidence\n";
        std::cout << "-------------------------------------------\n";

        for (const auto& result : topPredictions)
        {
            std::cout << std::left << std::setw(32) << labels[result.first] << std::right << std::setw(10) << std::fixed
                      << std::setprecision(2) << (result.second * 100) << "%\n";
        }

        std::cout << "-------------------------------------------\n";
    }

} // namespace Shared
} // namespace WindowsML
