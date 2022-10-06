#pragma once
#include "pch.h"
#if __has_include("IEnumWidgetBase.g.cpp")
#include "IEnumWidgetBase.g.cpp"
#endif

namespace winrt::WinUI3App::implementation
{
  struct IEnumWidgetBase
  {
    virtual void text(const winrt::hstring& str) = 0;
    virtual winrt::hstring text() = 0;

    virtual int index() = 0;
    virtual void index(int i) = 0;

    virtual void addItem(const winrt::hstring& item) = 0;
    virtual void clearItems() = 0;

    virtual winrt::event_token changed(Windows::Foundation::EventHandler<bool> const& handler) = 0;
    virtual void changed(winrt::event_token const& handler) = 0;
  };
}