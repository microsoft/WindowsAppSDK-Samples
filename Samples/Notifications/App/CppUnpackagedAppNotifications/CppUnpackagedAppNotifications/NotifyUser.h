#pragma once
struct NotifyUser
{
    static void CouldNotSendToast();
    static void ToastSentSuccessfully();
    static void AppLaunchedFromNotification();
    static void NotificationReceived();
    static void UnrecognizedToastOriginator();
};

