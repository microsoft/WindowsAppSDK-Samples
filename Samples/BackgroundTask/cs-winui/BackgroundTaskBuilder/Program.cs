using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using Windows.ApplicationModel.Background;

namespace BackgroundTaskBuilder
{
    public class Program
    {
        static private uint _RegistrationToken;
        static private ManualResetEvent _exitEvent = new ManualResetEvent(false);

        static void Main(string[] args)
        {
            if (args.Contains("-RegisterForBGTaskServer"))
            {
                Guid taskGuid = typeof(BackgroundTask).GUID;
                ComServer.CoRegisterClassObject(ref taskGuid,
                                                new ComServer.BackgroundTaskFactory<BackgroundTask, IBackgroundTask>(),
                                                ComServer.CLSCTX_LOCAL_SERVER,
                                                ComServer.REGCLS_MULTIPLEUSE,
                                                out _RegistrationToken);

                // Wait for the exit event to be signaled before exiting the program
                _exitEvent.WaitOne();
            }
            else
            {
                App.Start(p => new App());
            }
        }

        public static void SignalExit()
        {
            _exitEvent.Set();
        }
    }
}
