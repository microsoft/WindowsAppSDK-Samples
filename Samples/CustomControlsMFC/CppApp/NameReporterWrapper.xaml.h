#pragma once

#include "winrt/Microsoft.UI.Xaml.h"
#include "winrt/Microsoft.UI.Xaml.Markup.h"
#include "winrt/Microsoft.UI.Xaml.Controls.Primitives.h"
#include "NameReporterWrapper.g.h"

namespace winrt::CppApp::implementation
{
    struct NameReporterWrapper : NameReporterWrapperT<NameReporterWrapper>
    {
        NameReporterWrapper();

        int32_t MyProperty();
        void MyProperty(int32_t value);

        void myButton_Click(Windows::Foundation::IInspectable const& sender, Microsoft::UI::Xaml::RoutedEventArgs const& args);
    };
}

namespace winrt::CppApp::factory_implementation
{
    struct NameReporterWrapper : NameReporterWrapperT<NameReporterWrapper, implementation::NameReporterWrapper>
    {
    };
}
