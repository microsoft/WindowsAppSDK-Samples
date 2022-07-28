#include "pch.h"

using namespace winrt;
using namespace Windows::Foundation;
using namespace Microsoft::Windows::ApplicationModel::Resources;


int main()
{
    init_apartment();
    ResourceManager manager(L"SelfContainedDeployment.pri");
    auto message = manager.MainResourceMap().GetValue(L"Resources/Description").ValueAsString();
    printf("%ls\n", message.c_str());
}
