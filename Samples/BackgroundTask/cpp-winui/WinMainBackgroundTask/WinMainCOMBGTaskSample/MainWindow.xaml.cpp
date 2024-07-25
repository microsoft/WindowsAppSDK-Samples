#include "pch.h"
#include "MainWindow.xaml.h"
#if __has_include("MainWindow.g.cpp")
#include "MainWindow.g.cpp"
#endif
#include "Win32BGClass.h"
#include "ToastNotificationCreator.h"

using namespace winrt;
using namespace Microsoft::UI::Xaml;
namespace winrt
{
    using namespace winrt::Windows::ApplicationModel::Background;
}

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace winrt::WinMainCOMBGTaskSample::implementation
{
    int32_t MainWindow::MyProperty()
    {
        throw hresult_not_implemented();
    }

    void MainWindow::MyProperty(int32_t /* value */)
    {
        throw hresult_not_implemented();
    }

    void MainWindow::myButton_Click(IInspectable const&, RoutedEventArgs const&)
    {
        myButton().Content(box_value(L"BG Task Registered, Delete Toast Notification to trigger BG Task"));
        RegisterTask(winrt::ToastNotificationHistoryChangedTrigger(), L"Win32BGTask", __uuidof(Win32BGTask), true);
        ToastNotificationCreator();
    }


    void MainWindow::UnregisterDuplicateTask(_In_ hstring taskName)
    {
        auto allRegistrations = BackgroundTaskRegistration::AllTasks();
        for (auto taskPair : allRegistrations)
        {
            IBackgroundTaskRegistration task = taskPair.Value();
            if (taskName == task.Name())
            {
                task.Unregister(true);
            }
        }
    }

    hresult MainWindow::RegisterTask(_In_ IBackgroundTrigger trigger, _In_ hstring taskName, _In_ guid classId, _In_ bool UnregisterOldTask)
    {
        hresult hr;

        try
        {
            if (UnregisterOldTask)
            {
                UnregisterDuplicateTask(taskName);
            }

            BackgroundTaskBuilder taskBuilder;
            taskBuilder.SetTrigger(trigger);
            taskBuilder.Name(taskName);
            taskBuilder.SetTaskEntryPointClsid(classId);

            taskBuilder.Register();
        }
        catch (...)
        {
            hr = to_hresult();
        }

        return hr;
    }
}
