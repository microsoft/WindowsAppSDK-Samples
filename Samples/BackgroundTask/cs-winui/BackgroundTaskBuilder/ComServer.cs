// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;
using WinRT;

namespace BackgroundTaskBuilder
{
    // COM server startup code.
    internal class ComServer
    {
        [DllImport("ole32.dll")]
        public static extern int CoRegisterClassObject(
            ref Guid classId,
            [MarshalAs(UnmanagedType.Interface)] IClassFactory objectAsUnknown,
            uint executionContext,
            uint flags,
            out uint registrationToken);

        public const uint CLSCTX_LOCAL_SERVER = 4;
        public const uint REGCLS_MULTIPLEUSE = 1;

        public const uint S_OK = 0x00000000;
        public const uint CLASS_E_NOAGGREGATION = 0x80040110;
        public const uint E_NOINTERFACE = 0x80004002;

        public const string IID_IUnknown = "00000000-0000-0000-C000-000000000046";
        public const string IID_IClassFactory = "00000001-0000-0000-C000-000000000046";

        [ComImport]
        [ComVisible(false)]
        [Guid(IID_IClassFactory)]
        [InterfaceType(ComInterfaceType.InterfaceIsIUnknown)]
        public interface IClassFactory
        {
            [PreserveSig]
            uint CreateInstance(IntPtr objectAsUnknown, ref Guid interfaceId, out IntPtr objectPointer);

            [PreserveSig]
            uint LockServer(bool Lock);
        }

        internal class BackgroundTaskFactory<TaskType, TaskInterface> : IClassFactory where TaskType : TaskInterface, new()
        {
            public BackgroundTaskFactory()
            {
            }

            public uint CreateInstance(IntPtr objectAsUnknown, ref Guid interfaceId, out IntPtr objectPointer)
            {
                if (objectAsUnknown != IntPtr.Zero)
                {
                    objectPointer = IntPtr.Zero;
                    return CLASS_E_NOAGGREGATION;
                }

                if ((interfaceId != typeof(TaskType).GUID) && (interfaceId != new Guid(IID_IUnknown)))
                {
                    objectPointer = IntPtr.Zero;
                    return E_NOINTERFACE;
                }

                objectPointer = MarshalInterface<TaskInterface>.FromManaged(new TaskType());
                return S_OK;
            }

            public uint LockServer(bool lockServer)
            {
                return S_OK;
            }
        }
    }
}
