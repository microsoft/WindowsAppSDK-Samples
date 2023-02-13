// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.Widgets.Providers;
using System;
using System.Threading;

namespace WidgetHelper
{
    public class RegistrationManager<TWidgetProvider> : IDisposable
        where TWidgetProvider : IWidgetProvider, new()
    {
        private bool disposedValue = false;
        private ManualResetEvent disposedEvent = new ManualResetEvent(false);

        private class ClassLifetimeUnregister : IDisposable
        {
            public ClassLifetimeUnregister(uint registrationHandle) { COMRegistrationHandle = registrationHandle; }
            private readonly uint COMRegistrationHandle;

            public void Dispose()
            {
                Com.ClassObject.Revoke(COMRegistrationHandle);
            }
        }

        private IDisposable registeredProvider;

        private RegistrationManager(IDisposable provider)
        {
            registeredProvider = provider;
        }

        public static RegistrationManager<TWidgetProvider> RegisterProvider()
        {
            var registration = RegisterClass(typeof(TWidgetProvider).GUID, new WidgetProviderFactory<TWidgetProvider>());
            return new RegistrationManager<TWidgetProvider>(registration);
        }

        private static IDisposable RegisterClass(Guid clsid, Com.IClassFactory factory)
        {
            uint registrationHandle;
            Com.ClassObject.Register(clsid, factory, out registrationHandle);

            return new ClassLifetimeUnregister(registrationHandle);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                registeredProvider.Dispose();
                disposedValue = true;
                disposedEvent.Set();
            }
        }

        ~RegistrationManager()
        {
            Dispose(disposing: false);
        }

        public ManualResetEvent GetDisposedEvent()
        {
            return disposedEvent;
        }
        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }
    }
}