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
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.ApplicationModel;
using Windows.ApplicationModel.Activation;
using Windows.Foundation;
using Windows.Foundation.Collections;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace CsWinUiDesktopInstancing
{
    /// <summary>
    /// Provides application-specific behavior to supplement the default Application class.
    /// </summary>
    public partial class App : Application
    {
        // Modify this to increase the number of active instances.
        public const int c_maxNumberOfInstances = 2;

        /// <summary>
        /// Initializes the singleton application object.  This is the first line of authored code
        /// executed, and as such is the logical equivalent of main() or WinMain().
        /// </summary>
        public App()
        {
            this.InitializeComponent();
        }

        /// <summary>
        /// Invoked when the application is launched normally by the end user.  Other entry points
        /// will be used such as when the application is launched to open a specific file.
        /// </summary>
        /// <param name="args">Details about the launch request and process.</param>

        // NOTE: Redirection needs to happen here.  If redirection is done anywhere else the behavior is unknown.
        // For example, I got a COM error when redirection was in the constructor for m_Window.
        protected override void OnLaunched(Microsoft.UI.Xaml.LaunchActivatedEventArgs args)
        {
            int numberOfActivations = ActivationTrackerHelper.IncrementAndSetNumberOfRedirectionsReturnNewNumber();
            m_window = new MainWindow();

            // GetInstances also gets the current instance.
            // Meat and potatoes of redirection.
            var instances = Microsoft.Windows.AppLifecycle.AppInstance.GetInstances();

            // Number will be incorrect if instance  (instanceNumber < c_maxNumberOfInstances) is closed.
            // Can be fixed on an OnExit event handler.
            m_window.Title = "Instance: " + ((instances.Count) - 1).ToString();

            // Need activated event args to pass into the redirected instance.
            var activatedArgs = Microsoft.Windows.AppLifecycle.AppInstance.GetCurrent().GetActivatedEventArgs();
            bool isActivationRedirected = false;
            if (instances.Count > c_maxNumberOfInstances)
            {
                // Time to redirect
                // Choose what instance to redirect to.
                int instanceToRedirectTo = (numberOfActivations % c_maxNumberOfInstances);
                isActivationRedirected = true;

                // Need to call RedirectActivationToAsync on the instance to redirect to.
                // Not passing in the instance into RedirectionActivationToAsync.
                instances[instanceToRedirectTo].RedirectActivationToAsync(activatedArgs).GetAwaiter();
            }

            if (isActivationRedirected)
            {
                this.Exit();
            }
            else
            {
                m_window.Activate();
            }
        }

        private Window m_window;
    }
}
