#include "pch.h"
#include "Win32BGClass.h"
#include "ToastNotificationCreator.h"

using namespace winrt;
namespace winrt
{
    using namespace winrt::Windows::ApplicationModel::Background;
}

namespace winrt::WinMainCOMBGTaskSample {

        void __stdcall Win32BGTask::Run(_In_ IBackgroundTaskInstance taskInstance)
        {
            taskInstance.Canceled({ this, &Win32BGTask::OnCanceled });

            TaskDeferral = taskInstance.GetDeferral();
            ToastNotificationCreator(true);

            TaskDeferral.Complete();
        }

        void __stdcall Win32BGTask::OnCanceled(_In_ IBackgroundTaskInstance /* taskInstance */, _In_ BackgroundTaskCancellationReason /* cancelReason */)
        {
            OutputDebugString(L"BG Task Cancelled");
            IsCanceled = true;
        }

}