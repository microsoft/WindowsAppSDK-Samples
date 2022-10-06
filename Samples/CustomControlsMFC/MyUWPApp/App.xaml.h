#pragma once
#include "App.g.h"
#include "App.base.h"
namespace winrt::MyUWPApp::implementation
{
    class App : public AppT2<App>
    {
    public:
        App();
        ~App();
    };
}
namespace winrt::MyUWPApp::factory_implementation
{
    class App : public AppT<App, implementation::App>
    {
    };
}
