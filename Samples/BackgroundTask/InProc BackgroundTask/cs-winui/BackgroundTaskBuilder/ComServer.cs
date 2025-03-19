// Copyright (c) Microsoft Corporation and Contributors.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;
using Windows.ApplicationModel.Background;
using WinRT;

namespace BackgroundTaskBuilder
{
    // COM server startup code.
    internal partial class ComServer
    {
        [LibraryImport("ole32.dll")]
        public static partial int CoRegisterClassObject(
            in Guid classId,
            [MarshalAs(UnmanagedType.Interface)] IClassFactory objectAsUnknown,
            uint executionContext,
            uint flags,
            out uint registrationToken);

        [LibraryImport("ole32.dll")]
        public static partial int CoRevokeObject(
            out uint registrationToken);

        public const uint CLSCTX_LOCAL_SERVER = 4;
        public const uint REGCLS_MULTIPLEUSE = 1;

        public const uint S_OK = 0x00000000;
        public const uint CLASS_E_NOAGGREGATION = 0x80040110;
        public const uint E_NOINTERFACE = 0x80004002;

        public const string IID_IUnknown = "00000000-0000-0000-C000-000000000046";
        public const string IID_IClassFactory = "00000001-0000-0000-C000-000000000046";

        [GeneratedComInterface]
        [Guid(IID_IClassFactory)]
        public partial interface IClassFactory
        {
            [PreserveSig]
            uint CreateInstance(IntPtr objectAsUnknown, in Guid interfaceId, out IntPtr objectPointer);

            [PreserveSig]
            uint LockServer([MarshalAs(UnmanagedType.Bool)] bool Lock);
        }

        [GeneratedComClass]
        internal partial class BackgroundTaskFactory : IClassFactory
        {
            public BackgroundTaskFactory()
            {
            }

            public uint CreateInstance(IntPtr objectAsUnknown, in Guid interfaceId, out IntPtr objectPointer)
            {
                if (objectAsUnknown != IntPtr.Zero)
                {
                    objectPointer = IntPtr.Zero;
                    return CLASS_E_NOAGGREGATION;
                }

                if ((interfaceId != typeof(IBackgroundTask).GUID) && (interfaceId != new Guid(IID_IUnknown)))
                {
                    objectPointer = IntPtr.Zero;
                    return E_NOINTERFACE;
                }

                objectPointer = MarshalInterface<IBackgroundTask>.FromManaged(new BackgroundTask());
                return S_OK;
            }

            public uint LockServer(bool lockServer)
            {
                return S_OK;
            }
        }
    }
}
