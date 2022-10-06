#include "pch.h"
#include "NameReporterWrapper.xaml.h"
#if __has_include("NameReporterWrapper.g.cpp")
#include "NameReporterWrapper.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::CppApp::implementation
{
    NameReporterWrapper::NameReporterWrapper()
    {
        InitializeComponent();
    }

    int32_t NameReporterWrapper::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void NameReporterWrapper::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void NameReporterWrapper::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"Clicked"));
    }
}
