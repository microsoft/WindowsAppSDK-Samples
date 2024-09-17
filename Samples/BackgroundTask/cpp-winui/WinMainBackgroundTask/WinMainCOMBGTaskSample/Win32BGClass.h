#pragma once

#include "pch.h"

namespace winrt::WinMainCOMBGTaskSample
{
	#define CLSID_Win32BGTask "12345678-1234-1234-1234-1234567890AB"


	struct __declspec(uuid(CLSID_Win32BGTask))
		Win32BGTask : implements<Win32BGTask, winrt::Windows::ApplicationModel::Background::IBackgroundTask>
	{
		volatile bool IsCanceled = false;
		winrt::Windows::ApplicationModel::Background::BackgroundTaskDeferral TaskDeferral = nullptr;
		void Run(_In_ winrt::Windows::ApplicationModel::Background::IBackgroundTaskInstance taskInstance);
		void OnCanceled(_In_ winrt::Windows::ApplicationModel::Background::IBackgroundTaskInstance /* taskInstance */, _In_ winrt::Windows::ApplicationModel::Background::BackgroundTaskCancellationReason /* cancelReason */);

	};

	struct Win32BGTaskFactory : implements<Win32BGTaskFactory, IClassFactory>
	{
		HRESULT __stdcall CreateInstance(_In_opt_ IUnknown* aggregateInterface, _In_ REFIID interfaceId, _Outptr_ VOID** object) noexcept final
		{
			if (aggregateInterface != NULL) {
				return CLASS_E_NOAGGREGATION;
			}

			return make<Win32BGTask>().as(interfaceId, object);
		}

		HRESULT __stdcall LockServer(BOOL lock) noexcept final
		{
			UNREFERENCED_PARAMETER(lock);
			return S_OK;
		}
	};
}
