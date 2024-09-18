using Microsoft.UI.Dispatching;
using WinFormsWithIsland;

namespace WinFormsWithIslandApp
{
    internal static class Program
    {
        /// <summary>
        ///  The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            // Island-support: This is required to use the WindowsAppSDK UI stack on the thread.
            DispatcherQueueController controller = DispatcherQueueController.CreateOnCurrentThread();

            // Island-support: This is necessary for Xaml controls, resources, and metatdata to work correctly.
            var xamlApp = new XamlApp();

            // Island-support: This allows the WindowsAppSDK UI stack to process messages before the WinForms message loop.
            WindowsAppSdkHelper.EnableContentPreTranslateMessageInEventLoop();

            // To customize application configuration such as set high DPI settings or default font,
            // see https://aka.ms/applicationconfiguration.
            ApplicationConfiguration.Initialize();
            Application.Run(new Form1());

            // Island-support: Shut down the DispatcherQueue and all the WindowsAppSDK UI objects on the thread.
            controller.ShutdownQueue();
        }
    }
}