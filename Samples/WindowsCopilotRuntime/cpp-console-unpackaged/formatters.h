// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <format>

namespace std
{
template <typename CharT>
struct formatter<winrt::hstring, CharT>
{
    template <class ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}')
        {
            throw std::runtime_error("Invalid format specifier");
        }

        return it;
    }

    template <class Context>
    constexpr auto format(winrt::hstring const& str, Context& ctx) const
    {
        return std::ranges::copy(str.c_str(), str.c_str() + str.size(), ctx.out());
    }
};
} // namespace std
