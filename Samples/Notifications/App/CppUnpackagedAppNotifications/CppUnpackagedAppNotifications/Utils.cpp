#include "pch.h"
#include "Utils.h"
#include "ToastWithAvatar.h"
#include "ToastWithTextBox.h"

std::wstring ExePath() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileName(NULL, buffer, MAX_PATH);
    std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
    return std::wstring(buffer).substr(0, pos);
}

std::wstring Utils::GetFullPathToAsset(std::wstring const& assetName)
{
    return ExePath() + L"\\Assets\\" + assetName;
}
