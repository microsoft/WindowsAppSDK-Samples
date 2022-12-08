// Copyright (C) Microsoft Corporation.
// Licensed under the MIT License.

using Microsoft.Windows.Widgets.Providers;
using System;
using System.Runtime.InteropServices;
using WinRT;

namespace WidgetHelper
{
    namespace Com
    {
        static class Guids
        {
            public const string IClassFactory = "00000001-0000-0000-C000-000000000046";
            public const string IUnknown = "00000000-0000-0000-C000-000000000046";
        }

        [ComImport(), InterfaceType(ComInterfaceType.InterfaceIsIUnknown), Guid(Guids.IClassFactory)]
        internal interface IClassFactory
        {
            [PreserveSig]
            int CreateInstance(IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject);
            [PreserveSig]
            int LockServer(bool fLock);
        }

        static class ClassObject
        {
            public static void Register(Guid clsid, object pUnk, out uint cookie)
            {
                [DllImport("ole32.dll")]
                static extern int CoRegisterClassObject(
                    [MarshalAs(UnmanagedType.LPStruct)] Guid rclsid,
                    [MarshalAs(UnmanagedType.IUnknown)] object pUnk,
                    uint dwClsContext,
                    uint flags,
                    out uint lpdwRegister);

                int result = CoRegisterClassObject(clsid, pUnk, 0x4, 0x1, out cookie);
                if (result != 0)
                {
                    Marshal.ThrowExceptionForHR(result);
                }
            }

            public static int Revoke(uint cookie)
            {
                [DllImport("ole32.dll")]
                static extern int CoRevokeClassObject(uint dwRegister);

                return CoRevokeClassObject(cookie);
            }
        }
    }

    internal class WidgetProviderFactory<T> : Com.IClassFactory
            where T : IWidgetProvider, new()
    {
        public int CreateInstance(IntPtr pUnkOuter, ref Guid riid, out IntPtr ppvObject)
        {
            ppvObject = IntPtr.Zero;

            if (pUnkOuter != IntPtr.Zero)
            {
                Marshal.ThrowExceptionForHR(CLASS_E_NOAGGREGATION);
            }

            if (riid == typeof(T).GUID || riid == Guid.Parse(Com.Guids.IUnknown))
            {
                // Create the instance of the .NET object
                ppvObject = MarshalInspectable<IWidgetProvider>.FromManaged(new T());
            }
            else
            {
                // The object that ppvObject points to does not support the
                // interface identified by riid.
                Marshal.ThrowExceptionForHR(E_NOINTERFACE);
            }

            return 0;
        }

        int Com.IClassFactory.LockServer(bool fLock)
        {
            return 0;
        }

        private const int CLASS_E_NOAGGREGATION = -2147221232;
        private const int E_NOINTERFACE = -2147467262;
    }
}
