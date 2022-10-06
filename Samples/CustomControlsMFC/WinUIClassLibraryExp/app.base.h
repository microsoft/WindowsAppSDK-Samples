﻿#pragma once
namespace winrt::MyUWPApp::implementation
{
    template <typename D, typename... I>
    struct App_baseWithProvider : public App_base<D, ::winrt::Windows::UI::Xaml::Markup::IXamlMetadataProvider>
    {
        using IXamlType = ::winrt::Windows::UI::Xaml::Markup::IXamlType;
        IXamlType GetXamlType(::winrt::Windows::UI::Xaml::Interop::TypeName const& type)
        {
            return _appProvider.GetXamlType(type);
        }
        IXamlType GetXamlType(::winrt::hstring const& fullName)
        {
            return _appProvider.GetXamlType(fullName);
        }
        ::winrt::com_array<::winrt::Windows::UI::Xaml::Markup::XmlnsDefinition> GetXmlnsDefinitions()
        {
            return _appProvider.GetXmlnsDefinitions();
        }
    private:
        bool _contentLoaded{ false };
        winrt::MyUWPApp::XamlMetaDataProvider _appProvider;
    };
    template <typename D, typename... I>
    using AppT2 = App_baseWithProvider<D, I...>;
}
