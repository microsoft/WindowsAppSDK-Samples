// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#if Eric
//#include "pch.h"
//#include "Common.h"

std::optional<std::wstring> Common::ExtractParam(std::wstring const& args, std::wstring const& paramName)
{
    auto tag{ paramName };
    tag.append(L"=");

    auto scenarioIdStart{ args.find(tag) };
    if (scenarioIdStart == std::wstring::npos)
    {
        return std::nullopt;
    }

    scenarioIdStart += tag.length();

    auto scenarioIdEnd{ args.find(L"&", scenarioIdStart) };

    return args.substr(scenarioIdStart, scenarioIdEnd - scenarioIdStart);
}
#endif