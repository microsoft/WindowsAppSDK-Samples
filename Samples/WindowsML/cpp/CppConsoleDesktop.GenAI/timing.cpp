// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "timing.h"
#include <cassert>

void Timing::RecordStartTimestamp()
{
    assert(start_timestamp_.time_since_epoch().count() == 0);
    start_timestamp_ = Clock::now();
}

void Timing::RecordFirstTokenTimestamp()
{
    assert(first_token_timestamp_.time_since_epoch().count() == 0);
    first_token_timestamp_ = Clock::now();
}

void Timing::RecordEndTimestamp()
{
    assert(end_timestamp_.time_since_epoch().count() == 0);
    end_timestamp_ = Clock::now();
}

void Timing::Log(const int prompt_tokens_length, const int new_tokens_length)
{
    assert(start_timestamp_.time_since_epoch().count() != 0);
    assert(first_token_timestamp_.time_since_epoch().count() != 0);
    assert(end_timestamp_.time_since_epoch().count() != 0);

    Duration prompt_time = (first_token_timestamp_ - start_timestamp_);
    Duration run_time = (end_timestamp_ - first_token_timestamp_);

    const auto default_precision{std::cout.precision()};
    std::cout << std::endl;
    std::cout << "-------------" << std::endl;
    std::cout << std::fixed << std::showpoint << std::setprecision(2) << "Prompt length: " << prompt_tokens_length
              << ", New tokens: " << new_tokens_length << ", Time to first: " << prompt_time.count() << "s"
              << ", Prompt tokens per second: " << prompt_tokens_length / prompt_time.count() << " tps"
              << ", New tokens per second: " << new_tokens_length / run_time.count() << " tps"
              << std::setprecision(default_precision) << std::endl;
    std::cout << "-------------" << std::endl;
}
