// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.UI.Xaml.Controls;
using System.Timers;
using System;
using System.Threading;
using Microsoft.UI.Xaml;
using System.ComponentModel;
using System.Linq;
using System.Runtime.InteropServices;
using System.Diagnostics;
using System.Reflection;
using Windows.Win32.Foundation;
using Windows.Win32.System.Recovery;
using Windows.Win32.System.WindowsProgramming;
using static Windows.Win32.PInvoke;

namespace cs_winui_packaged
{
    public partial class S2_CrashRecovery : Page
    {
        private DispatcherTimer _timer;
        private uint _counter;

        public S2_CrashRecovery()
        {
            this.InitializeComponent();
            setRecoveredCounter();

            // Bsst effort to register for crash and hang restart.
            RegisterApplicationRestart(_counter.ToString(), REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_PATCH | REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_REBOOT);

            unsafe
            {
                // Best effort to register recovery callback to update restart arguments with the latest seconds counter value.
                RegisterApplicationRecoveryCallback(new APPLICATION_RECOVERY_CALLBACK(p =>
                {
                    ApplicationRecoveryInProgress(out BOOL canceled);
                    if (!canceled)
                    {
                        RegisterApplicationRestart(_counter.ToString(), REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_PATCH | REGISTER_APPLICATION_RESTART_FLAGS.RESTART_NO_REBOOT);
                    }

                    ApplicationRecoveryFinished(true);
                    return 0;
                }), null, 5000, 0);
            }

            startTimer(); 
        }

        private void setRecoveredCounter()
        {
            string[] commandLineArguments = Environment.GetCommandLineArgs();
            if (commandLineArguments.Length > 1)
            {
                commandLineArguments = commandLineArguments.Skip(1).ToArray();
                try
                {
                    _counter = Convert.ToUInt32(commandLineArguments[0]);
                }
                catch
                {
                    _counter = 0;
                }
            }
            else
            {
                _counter = 0;
            }

            counterTextBlock.Text = _counter.ToString();
        }

        private void startTimer()
        {
            _timer = new DispatcherTimer();
            _timer.Tick += timer_OnTick;
            _timer.Interval = new TimeSpan(0, 0, 1);
            _timer.Start();
        }

        private void timer_OnTick(object sender, object e)
        {
            _counter++;
            counterTextBlock.Text = _counter.ToString();
        }

        private void crash_Click(object sender, RoutedEventArgs e)
        {
            // Cause application to be unresponsive
            for (int i = 60; i >= 0; i--)
            {
                System.Threading.Thread.Sleep(1000);
            }

            throw new Exception("Fatal crash!");
        }
    }
}
