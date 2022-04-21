#pragma once
class Common
{
public:
    static std::optional<std::wstring> ExtractParam(std::wstring const& args, std::wstring const& paramName);
};

