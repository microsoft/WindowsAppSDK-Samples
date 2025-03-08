// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
struct Common
{
public:
    static std::optional<std::wstring> ExtractParamFromArgs(std::wstring const& args, std::wstring const& paramName);

    static std::wstring MakeScenarioIdToken(unsigned Id);
    static std::optional<unsigned> ExtractScenarioIdFromArgs(std::wstring const& args);
};

