// Copyright (c) Microsoft Corporation.  All rights reserved.

#pragma once

enum SettingId
{
    Setting_ForceAliasedText,
    Setting_DisablePixelSnapping,
    Setting_ShowSpriteBounds,
    Setting_ShowSpriteGeneration
};

class ISettingChangedHandler
{
public:
    virtual void OnSettingChanged(SettingId id) = 0;
};

class SettingCollection final
{
public:
    bool GetSetting(SettingId id) const noexcept
    {
        return ((m_flags >> id) & 1) != 0;
    }

    void SetSetting(SettingId id, bool value);

    void AddEventHandler(ISettingChangedHandler* handler);
    void RemoveEventHandler(ISettingChangedHandler* handler);

private:
    uint32_t m_flags = 0;
    std::vector<ISettingChangedHandler*> m_handlers;
};

class SettingChangedHandler : public ISettingChangedHandler
{
public:
    SettingChangedHandler(std::shared_ptr<SettingCollection> const& settings);
    ~SettingChangedHandler();

    auto& GetSettings() const noexcept { return m_settings; }

    bool GetSetting(SettingId id) const noexcept
    {
        return m_settings->GetSetting(id);
    }

private:
    std::shared_ptr<SettingCollection> m_settings;
};
