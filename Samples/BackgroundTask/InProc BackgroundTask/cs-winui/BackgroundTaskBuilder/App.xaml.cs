// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using BackgroundTaskBuilder;
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;
using Microsoft.UI.Xaml.Controls.Primitives;
using Microsoft.UI.Xaml.Data;
using Microsoft.UI.Xaml.Input;
using Microsoft.UI.Xaml.Media;
using Microsoft.UI.Xaml.Navigation;
using Microsoft.UI.Xaml.Shapes;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.ApplicationModel.Background;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace BackgroundTaskBuilder
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        static private uint _RegistrationToken;
        public App()
        {
            this.InitializeComponent();
            Guid taskGuid = typeof(BackgroundTask).GUID;
            ComServer.CoRegisterClassObject(in taskGuid,
                                            new ComServer.BackgroundTaskFactory(),
                                            ComServer.CLSCTX_LOCAL_SERVER,
                                            ComServer.REGCLS_MULTIPLEUSE,
                                            out _RegistrationToken);
        }

        ~App()
        {
            ComServer.CoRevokeObject(out _RegistrationToken);
        }

        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            m_window = new MainWindow();
            m_window.Activate();
        }

        public static Window Window => m_window;

        private static Window? m_window;
    }
}
