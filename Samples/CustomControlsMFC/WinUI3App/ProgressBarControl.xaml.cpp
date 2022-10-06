#include "pch.h"
#include "ProgressBarControl.xaml.h"
#if __has_include("ProgressBarControl.g.cpp")
#include "ProgressBarControl.g.cpp"
#endif

using namespace winrt;
using namespace Microsoft::UI::Xaml;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinUI3App::implementation
{
  ProgressBarControl::ProgressBarControl()
    {
        InitializeComponent();
    }

    int32_t ProgressBarControl::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void ProgressBarControl::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }
}
