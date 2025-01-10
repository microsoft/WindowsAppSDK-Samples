// Copyright (c) Microsoft Corporation.  All rights reserved.

#include "precomp.h"
#include "SettingCollection.h"

void SettingCollection::SetSetting(SettingId id, bool value)
{
    if (GetSetting(id) != value)
    {
        if (value)
        {
            m_flags |= 1u << id;
        }
        else
        {
            m_flags &= ~(1u << id);
        }
        for (auto handler : m_handlers)
        {
            handler->OnSettingChanged(id);
        }
    }
}

void SettingCollection::AddEventHandler(ISettingChangedHandler* handler)
{
    m_handlers.push_back(handler);
}

void SettingCollection::RemoveEventHandler(ISettingChangedHandler* handler)
{
    auto p = std::find(m_handlers.begin(), m_handlers.end(), handler);
    if (p != m_handlers.end())
    {
        m_handlers.erase(p);
    }
}

SettingChangedHandler::SettingChangedHandler(std::shared_ptr<SettingCollection> const& settings) :
    m_settings(settings)
{
    m_settings->AddEventHandler(this);
}

SettingChangedHandler::~SettingChangedHandler()
{
    m_settings->RemoveEventHandler(this);
}
