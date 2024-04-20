using Microsoft.UI.Dispatching;
using System.Configuration;
using System.Data;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace WpfWithIslandApp
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        protected override void OnStartup(StartupEventArgs e)
        {
            _dispacherQueueController = DispatcherQueueController.CreateOnCurrentThread();
            ComponentDispatcher.ThreadFilterMessage += ComponentDispatcher_ThreadFilterMessage;
            base.OnStartup(e);
        }

        private void ComponentDispatcher_ThreadFilterMessage(ref MSG msg, ref bool handled)
        {
            var pinvokeMsg = new PInvokeMsg() { 
                hwnd = msg.hwnd,
                message = msg.message,
                wParam = msg.wParam,
                lParam = msg.lParam };
            handled = ContentPreTranslateMessage(ref pinvokeMsg);
        }

        protected override void OnExit(ExitEventArgs e)
        {
            base.OnExit(e);
            _dispacherQueueController?.ShutdownQueue();
            _dispacherQueueController = null;
        }

        [StructLayout(LayoutKind.Sequential)]
        public struct PInvokeMsg
        {
            public IntPtr hwnd;
            public int message;
            public IntPtr wParam;
            public IntPtr lParam;
        }

        [DllImport("Microsoft.UI.Windowing.Core.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.Bool)]
        public static extern bool ContentPreTranslateMessage(ref PInvokeMsg pmsg);

        DispatcherQueueController? _dispacherQueueController;
    }

}
