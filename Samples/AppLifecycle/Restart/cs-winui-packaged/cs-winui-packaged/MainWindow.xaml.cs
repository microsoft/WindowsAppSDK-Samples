using Microsoft.UI.Xaml;
using System;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Microsoft.Windows.AppLifecycle;
using Windows.ApplicationModel.Core;
using Windows.ApplicationModel.Activation;

// To learn more about WinUI, the WinUI project structure,
// and more about our project templates, see: http://aka.ms/winui-project-info.

namespace cs_winui_packaged
{
    /// <summary>
    /// An empty window that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainWindow : Window
    {
        public MainWindow()
        {
            this.InitializeComponent();
            this.setRestartArgs();
        }

        private void setRestartArgs()
        {
            // Using Environment.GetCommandLineArgs() to retrieve the command line arguments
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            if (commandLineArguments.Length > 1)
            {
                commandLineArguments = commandLineArguments.Skip(1).ToArray();
                commandLineArgsText.Text = String.Join(",", commandLineArguments);
            }

            // Using AppInstance.GetActivatedEventArgs() to retrieve the command line arguments
            AppActivationArguments activatedArgs = AppInstance.GetCurrent().GetActivatedEventArgs();
            ExtendedActivationKind kind = activatedArgs.Kind;

            if (kind == ExtendedActivationKind.Launch)
            {
                if (activatedArgs.Data is ILaunchActivatedEventArgs launchArgs)
                {
                    string argString = launchArgs.Arguments;
                    string[] argStrings = argString.Split();

                    if (argStrings.Length > 1)
                    {
                        argStrings = argStrings.Skip(1).ToArray();
                        ActivatedEventArgsText.Text = String.Join(",", argStrings.Where(s => !string.IsNullOrEmpty(s)));
                    }
                }
            }
        }

        private void myButton_Click(object sender, RoutedEventArgs e)
        {
            String restartArgsInput = argumentsInput.Text;
            AppRestartFailureReason restartError = AppInstance.Restart(restartArgsInput);

            switch (restartError)
            {
                case AppRestartFailureReason.RestartPending:
                    statusText.Text = "Another restart is currently pending.";
                    break;
                case AppRestartFailureReason.InvalidUser:
                    statusText.Text = "Current user is not signed in or not a valid user.";
                    break;
                case AppRestartFailureReason.Other:
                    statusText.Text = "Failure restarting.";
                    break;
            }
        }
    }
}
