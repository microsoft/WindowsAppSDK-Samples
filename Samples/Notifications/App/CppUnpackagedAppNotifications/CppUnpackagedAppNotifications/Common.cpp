// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "Common.h"

static auto scenarioTag{ L"scenarioId" };

std::optional<std::wstring> Common::ExtractParamFromArgs(std::wstring const& args, std::wstring const& paramName)
{
    auto tag{ paramName };
    tag.append(L"=");

    auto tagStart{ args.find(tag) };
    if (tagStart == std::wstring::npos)
    {
        return std::nullopt;
    }

    auto paramStart{ tagStart + tag.length() };
    auto paramEnd{ args.find(L"&", paramStart) };

    return args.substr(paramStart, paramEnd - paramStart);
}

std::wstring Common::MakeScenarioIdToken(unsigned id)
{
    return std::wstring(scenarioTag) + L"=" + std::to_wstring(id);
}

std::optional <unsigned> Common::ExtractScenarioIdFromArgs(std::wstring const& args)
{
    auto scenarioId{ ExtractParamFromArgs(args, scenarioTag) };

    if (!scenarioId.has_value())
    {
        return std::nullopt;
    }

    return std::stoul(scenarioId.value());
}
